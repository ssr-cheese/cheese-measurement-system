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
#include <nvs_flash.h>

#include <iostream>

#include "ble_battery_service.h"
#include "ble_cheese_timer_service.h"

#include "app_log.h"

extern "C" void app_main() {
  /* Boot Message */
  logi << "Cheese Timer Node" << std::endl;

  /* NVS flash initialization */
  nvs_flash_init();

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

  /* Main Loop */
  while (1) {
    /* for example */
    FreeRTOS::sleep(1000);
    {
      uint32_t value = FreeRTOS::getTimeSinceStart();
      pCheeseService->setValue(value);
      pCheeseService->notify();
    }
    {
      uint8_t value = FreeRTOS::getTimeSinceStart() % 101;
      pBatteryService->setBatteryLevel(value);
      pBatteryService->notify();
    }
  }

  /* sleep forever */
  vTaskDelay(portMAX_DELAY);
}
