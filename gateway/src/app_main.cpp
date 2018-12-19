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

#include "app_config.h"
#include "app_led.h"
#include "app_log.h"
#include "ble_battery_service.h"
#include "ble_callback_utils.h"
#include "ble_cheese_timer_service.h"

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
  BLEDevice::init("Cheese Timer Gateway");
  BLEDevice::setPower(ESP_PWR_LVL_P4);

  /* Find Device */
  BLEAdvertisedDevice deviceStart = BLECheeseTimerServiceClient::findDevice(
      BLECheeseTimerService::Position::Start);
  BLEAdvertisedDevice deviceGoal = BLECheeseTimerServiceClient::findDevice(
      BLECheeseTimerService::Position::Goal);

  /* Connect Devices */
  logi << "Connect Devices" << std::endl;
  FreeRTOS::Semaphore reconnect_semaphore;
  LED *pLED[2] = {pStartLED, pGoalLED};
  BLEAdvertisedDevice *pBLEAdvertisedDevice[2] = {&deviceStart, &deviceGoal};
  std::string pos_str[2] = {
      BLECheeseTimerService::toString(BLECheeseTimerService::Position::Start),
      BLECheeseTimerService::toString(BLECheeseTimerService::Position::Goal),
  };
  BLEClient *pClient[2];
  for (int i = 0; i < 2; ++i) {
    pClient[i] = BLEDevice::createClient();
    pClient[i]->setClientCallbacks(new MyBLEClientCallbacks(
        [&, i](BLEClient *pClient) {
          pLED[i]->on();
          logi << "onConnect " << pos_str[i] << std::endl;
          BLECheeseTimerServiceClient::update_params(
              pClient->getPeerAddress().getNative());
        },
        [&, i](BLEClient *pClient) {
          pLED[i]->blink();
          logi << "onDisconnect " << pos_str[i] << std::endl;
          reconnect_semaphore.give();
        }));
  }
  reconnect_semaphore.take();
  while (1) {
    for (int i = 0; i < 2; ++i) {
      if (!pClient[i]->isConnected()) {
        logi << "Connect: " << pos_str[i] << ": "
             << pBLEAdvertisedDevice[i]->toString() << std::endl;
        if (!pClient[i]->connect(pBLEAdvertisedDevice[i]->getAddress())) {
          loge << "Failed to Connect " << std::endl;
          continue;
        }
        logi << "Configure BLE Remote Client" << std::endl;
        /* GATT BAS Client */
        BLEBatteryServiceClient batteryServiceClient(pClient[i]);
        /* GATT Cheese Timer Service Client */
        BLECheeseTimerServiceClient cheeseTimerServiceClient(pClient[i]);
      }
    }
    reconnect_semaphore.take(100);
  }

  /* never return since the objects will be destructed */
  vTaskDelay(portMAX_DELAY);
}
