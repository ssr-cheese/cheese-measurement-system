/**
 * @file app_main.cpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief Cheese Timer Gateway Part
 * @version 0.1
 * @date 2018-12-11
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 *
 */
#include <BLEClient.h>
#include <BLEDevice.h>
#include <FreeRTOS.h>
#include <nvs_flash.h>

#include <functional>
#include <iomanip>
#include <iostream>

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
#include "ble_battery_service.h"
#include "ble_callback_utils.h"
#include "ble_cheese_timer_service.h"

extern "C" void app_main() {
  /* Boot Message */
  logi << "Hello, this is " << DeviceName << "." << std::endl;

  /* NVS flash initialization */
  nvs_flash_init();

  /* GPIO Initialization */
  LED *pStartLED = new LED(PinStartLED);
  LED *pGoalLED = new LED(PinGoalLED);
  pStartLED->blink();
  pGoalLED->blink();

  /* BLE Initialization */
  FreeRTOS::sleep(3000);
  BLEDevice::init("Cheese Timer Gateway");

  /* BLE Advertising */
  BLEAdvertising *pBLEAdvertising = BLEDevice::getAdvertising();
  pBLEAdvertising->start();

  /* BLE Scan */
  BLEAdvertisedDevice *pDeviceStart = nullptr;
  pDeviceStart = new BLEAdvertisedDevice();
  // BLEAdvertisedDevice *pDeviceGoal = nullptr;
  // pDeviceGoal = new BLEAdvertisedDevice();
  auto findDeviceByPosition =
      [&](BLECheeseTimerService::Position target_position) {
        BLEScan *pScan = BLEDevice::getScan();
        BLEAdvertisedDevice foundDevice;
        /* set scan callback */
        pScan->setAdvertisedDeviceCallbacks(
            new MyAdvertisedDeviceCallbacks([&](BLEAdvertisedDevice dev) {
              if (dev.getServiceDataUUID().equals(
                      BLECheeseTimerService::ServiceUUID)) {
                std::string data = dev.getServiceData();
                BLECheeseTimerService::Position position =
                    static_cast<BLECheeseTimerService::Position>((int)data[0]);
                if (position == target_position) {
                  logi << "Device Found: " << dev.toString() << std::endl;
                  logi << "Position: "
                       << BLECheeseTimerService::toString(position)
                       << std::endl;
                  foundDevice = dev;
                  BLEDevice::getScan()->stop();
                }
              }
            }));
        /* conduct scan */
        logi << "Scan: " << BLECheeseTimerService::toString(target_position)
             << std::endl;
        // this blocks until the target device is found
        BLEScanResults scanResults = pScan->start(3600 * 24);
        /* unset scan callback */
        pScan->setAdvertisedDeviceCallbacks(nullptr);
        return foundDevice;
      };
  *pDeviceStart = findDeviceByPosition(BLECheeseTimerService::Position::Start);
  // *pDeviceGoal = findDeviceByPosition(BLECheeseTimerService::Position::Goal);

  /* BLE Client */
  BLEClient *pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyBLEClientCallbacks(
      [&](auto *pClient) {
        logi << "onConnect" << std::endl;
        pStartLED->on();
      },
      [&](auto *pClient) {
        logi << "onDisconnect" << std::endl;
        pStartLED->blink();
        *pDeviceStart =
            findDeviceByPosition(BLECheeseTimerService::Position::Start);
      }));
  pClient->connect(pDeviceStart);
  // pClient->connect(pDeviceGoal);

  /* GATT BAS */
  BLEBatteryServiceClient *pBLEBatteryServiceClient __attribute__((unused)) =
      new BLEBatteryServiceClient(
          pClient, [](uint8_t level) { logi << (int)level << std::endl; });

  /* GATT Cheese Timer Service */
  BLECheeseTimerServiceClient *pBLECheeseTimerServiceClient
      __attribute__((unused)) = new BLECheeseTimerServiceClient(
          pClient, [](uint32_t timer) { logi << (int)timer << std::endl; });

  /* Main Loop */
  while (1) {
    FreeRTOS::sleep(2222);
  }

  /* sleep forever */
  vTaskDelay(portMAX_DELAY);
}
