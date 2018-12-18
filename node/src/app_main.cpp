/**
 * @file app_main.cpp
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief Cheese Timer Sensor Node Part
 * @version 0.1
 * @date 2018-12-11
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 *
 */

#include "battery_monitor.h"
#include "ble_battery_service.h"
#include "ble_callback_utils.h"
#include "ble_cheese_timer_service.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <nvs_flash.h>
#include <thread.h>

#include <VL53L0X.h>
#include <VL6180X.h>

#include <thread>

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

#include "app_config.h"
#include "app_led.h"
#include "app_log.h"

/* No Global Variable */

extern "C" void app_main() {
  /* Boot Message */
  logi << "Hello, this is " << DeviceName << "." << std::endl;

  /* NVS flash initialization */
  nvs_flash_init();

  /* GPIO Initialization */
  LED *pSensorStatusLED = new LED(PinSensorStatusLED);
  LED *pErrorStatusLED = new LED(PinErrorStatusLED);
  pErrorStatusLED->blink();

  /* Battery Monitor */
  BatteryMonitor *pBatteryMonitor = new BatteryMonitor(
      BatteryAdcChannel, BatteryMinVoltage, BatteryMaxVoltage);

  /* Determine Position, Start or Goal */
  gpio_set_direction(PinPositionSelector, GPIO_MODE_INPUT);
  gpio_pullup_en(PinPositionSelector);
  int sw = gpio_get_level(PinPositionSelector);
  BLECheeseTimerService::Position position =
      sw == 0 ? BLECheeseTimerService::Position::Start
              : BLECheeseTimerService::Position::Goal;
  std::string positionString = BLECheeseTimerService::toString(position);
  logi << "Position: " << positionString << std::endl;

  /* BLE Initialization */
  BLEDevice::init(std::string{DeviceName} + " " + positionString);
  BLEDevice::setPower(ESP_PWR_LVL_P4);

  /* BLE Server */
  BLEServer *pServer = BLEDevice::createServer();
  /* setup event callback*/
  pServer->setCallbacks(new AppBLEServerCallbacks(
      [&](BLEServer *pServer) {
        logi << "Connected" << std::endl;
        BLEDevice::getAdvertising()->stop();
        pErrorStatusLED->off();
      },
      [&](BLEServer *pServer) {
        logi << "Disconnected" << std::endl;
        if (pServer->getConnectedCount() == 0)
          pErrorStatusLED->blink();
      }));

  /* Cheese Timer Service */
  BLECheeseTimerService *pCheeseService =
      new BLECheeseTimerService(pServer, position);

  /* Battery Service */
  BLEBatteryService *pBatteryService = new BLEBatteryService(pServer);

  /* BLE Advertising */
  BLEAdvertising *pBLEAdvertising = BLEDevice::getAdvertising();
  {
    BLEAdvertisementData advData;
    std::string data;
    data += static_cast<const char>(position);
    advData.setServiceData(BLECheeseTimerService::ServiceUUID, data);
    pBLEAdvertising->setAdvertisementData(advData);
    pBLEAdvertising->start();
  }

  /* ToF Sensor Initialization */
  VL6180X tof;
  // VL53L0X tof;
  tof.i2cMasterInit();
  if (!tof.init()) {
    pCheeseService->notifyMessage("Failed to Initialize ToF sensor :(");
    pErrorStatusLED->on();
    vTaskDelay(portMAX_DELAY);
  }

  /* Setup Complete */
  pCheeseService->notifyMessage("Setup Completed :)");

  /* Battery Monitor Thread in Background*/
  FreeRTOSpp::Thread batteryMonitorThread([&]() {
    // const auto period = std::chrono::seconds(10);
    const auto period = std::chrono::milliseconds(500);
    auto sleep_time_handle = std::chrono::steady_clock::now();
    while (1) {
      /* Periodical EXecution */
      std::this_thread::sleep_until(sleep_time_handle += period);
      uint8_t level =
          pBatteryMonitor->calcBatteryLevel(pBatteryMonitor->getVoltage());
      pBatteryService->setBatteryLevel(level);
      pBatteryService->notify();
      if (level < 10)
        pSensorStatusLED->blink();
    }
  });

  /* Cheese Time Thread in Background */
  FreeRTOSpp::Thread cheeseTimerThread([&]() {
    const int Threshold_mm = 150; //< BLEで可変にしたい
    bool passing = false;
    while (1) {
      int range_mm = tof.read();
      if (range_mm < Threshold_mm) {
        if (!passing) {
          passing = true;
          pSensorStatusLED->on();
          logd << "New Passed: " << range_mm << " [mm]" << std::endl;
          uint32_t value =
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch())
                  .count();
          pCheeseService->setPassedTime(value);
          pCheeseService->notify();
          /* Prevent from chattering */
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
      } else {
        passing = false;
        pSensorStatusLED->off();
      }
    }
  });

  /* wait forever */
  batteryMonitorThread.join();
  cheeseTimerThread.join();
}
