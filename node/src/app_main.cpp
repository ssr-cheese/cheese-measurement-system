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
#include "ble_cheese_timer_service.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <Thread.h>
#include <nvs_flash.h>

#include <VL53L0X.h>
#include <VL6180X.h>

#include <thread>

#include "app_config.h"
#include "app_log.h"

extern "C" void app_main() {
  /* Boot Message */
  logi << "Cheese Timer Node" << std::endl;

  /* NVS flash initialization */
  nvs_flash_init();

  /* Battery Monitor */
  BatteryMonitor *pBatteryMonitor = new BatteryMonitor(
      BatteryAdcChannel, BatteryMinVoltage, BatteryMaxVoltage);

  /* BLE Initialization */
  BLEDevice::init("Cheese Timer Node");

  /* BLE Server */
  BLEServer *pServer = BLEDevice::createServer();
  /* setup event callback*/
  class MyBLEServerCallbacks : public BLEServerCallbacks {
  public:
    MyBLEServerCallbacks() {}
    virtual void onConnect(BLEServer *pServer) override {
      BLEDevice::getAdvertising()->stop();
    }
    virtual void onDisconnect(BLEServer *pServer) override {
      BLEDevice::getAdvertising()->start();
    }
  };
  pServer->setCallbacks(new MyBLEServerCallbacks());

  /* Cheese Timer Service */
  BLECheeseTimerService *pCheeseService = new BLECheeseTimerService(pServer);

  /* Battery Service */
  BLEBatteryService *pBatteryService = new BLEBatteryService(pServer);

  /* BLE Advertising */
  BLEAdvertising *pBLEAdvertising = BLEDevice::getAdvertising();
  /* add service UUID to display */
  pBLEAdvertising->addServiceUUID(BLECheeseTimerService::ServiceUUID);
  pBLEAdvertising->start();

  /* ToF Sensor Initialization */
  VL6180X tof;
  // VL53L0X tof;
  tof.i2cMasterInit();
  if (!tof.init()) {
    pCheeseService->notifyMessage("Failed to Initialize ToF sensor :(");
    // vTaskDelay(portMAX_DELAY);
  }

  /* Battery Monitor Thread in Background*/
  FreeRTOSpp::Thread batteryMonitorThread([&]() {
    const auto period = std::chrono::seconds(3);
    auto sleep_time_handle = std::chrono::steady_clock::now();
    while (1) {
      /* Periodical EXecution */
      std::this_thread::sleep_until(sleep_time_handle += period);
      uint8_t level =
          pBatteryMonitor->calcBatteryLevel(pBatteryMonitor->getVoltage());
      pBatteryService->setBatteryLevel(level);
      pBatteryService->notify();
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
          logd << "New Passed: " << range_mm << " [mm]" << std::endl;
          uint32_t value = range_mm;
          // 一時的に，時刻ではなく測定データを送信している．
          pCheeseService->setPassedTime(value);
          pCheeseService->notify();
          /* Prevent from chattering */
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
      } else {
        passing = false;
      }
    }
  });

  /* wait forever */
  batteryMonitorThread.join();
  cheeseTimerThread.join();
}
