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
  BLEDevice::setPower(ESP_PWR_LVL_P4);

  /* BLE Advertising */
  BLEAdvertising *pBLEAdvertising = BLEDevice::getAdvertising();
  pBLEAdvertising->start();

  /* BLE Scan */
  auto handleDevice = [&](BLECheeseTimerService::Position position) {
    FreeRTOS::Semaphore reconnect_semaphore;
    reconnect_semaphore.take();
    while (1) {
      /* Scan Device */
      BLEAdvertisedDevice *pDevice = new BLEAdvertisedDevice();
      *pDevice = BLECheeseTimerServiceClient::findDevice(position);
      /* BLE Client */
      BLEClient *pClient = BLEDevice::createClient();
      pClient->setClientCallbacks(new MyBLEClientCallbacks(
          [&](BLEClient *pClient) {
            logi << "onConnect" << std::endl;
            BLECheeseTimerServiceClient::update_params(
                pClient->getPeerAddress().getNative());
            /* update status LED */
            switch (position) {
            case BLECheeseTimerService::Position::Start:
              pStartLED->on();
              break;
            case BLECheeseTimerService::Position::Goal:
              pGoalLED->on();
              break;
            }
          },
          [&](BLEClient *pClient) {
            logi << "onDisconnect" << std::endl;
            /* update status LED */
            switch (position) {
            case BLECheeseTimerService::Position::Start:
              pStartLED->blink();
              break;
            case BLECheeseTimerService::Position::Goal:
              pGoalLED->blink();
              break;
            }
            reconnect_semaphore.give();
          }));
      pClient->connect(pDevice);

      /* GATT BAS */
      BLEBatteryServiceClient *pBLEBatteryServiceClient
          __attribute__((unused)) = new BLEBatteryServiceClient(
              pClient, [](uint8_t level) { logi << (int)level << std::endl; });

      /* GATT Cheese Timer Service */
      BLECheeseTimerServiceClient *pBLECheeseTimerServiceClient
          __attribute__((unused)) = new BLECheeseTimerServiceClient(
              pClient, [](uint32_t timer) { logi << (int)timer << std::endl; });

      /* wait for reconnection requested */
      reconnect_semaphore.take();
    }
  };

  /* handle devices */
  FreeRTOSpp::Thread deviceStartThread(
      [&]() { handleDevice(BLECheeseTimerService::Position::Start); });
  // FreeRTOSpp::Thread deviceGoalThread(
  //     [&]() { handleDevice(BLECheeseTimerService::Position::Goal); });

  /* sleep forever */
  vTaskDelay(portMAX_DELAY);
}
