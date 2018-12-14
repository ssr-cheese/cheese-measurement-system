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

#include <BLEDevice.h>
#include <BLEServer.h>
#include <FreeRTOS.h>
#include <VL6180X.h>
#include <nvs_flash.h>

#include <iostream>
#include <thread>

#include "battery_monitor.h"
#include "ble_battery_service.h"
#include "ble_cheese_timer_service.h"

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
  tof.i2cMasterInit();
  if (!tof.init()) {
    pCheeseService->notifyMessage("Failed to Initialize ToF sensor :(");
    // vTaskDelay(portMAX_DELAY);
  }

  /* Battery Monitor Thread in Background*/
  std::thread batteryMonitorThread([&] {
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
  std::thread cheeseTimerThread([&] {
    const int Threshold_mm = 80;
    while (1) {
      int32_t range_mm;
      if (!tof.read(&range_mm))
        continue;
      logi << "ToF Range: " << (int)range_mm << std::endl;
      if (range_mm < Threshold_mm) {
        uint32_t value = range_mm;
        pCheeseService->setPassedTime(value);
        pCheeseService->notify();
        // Prevent from chattering
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  });

  /* wait forever */
  batteryMonitorThread.join();
  cheeseTimerThread.join();
}
