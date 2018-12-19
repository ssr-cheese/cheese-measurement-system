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

  /* BLE Scan */
  auto handleDevice = [&](BLEAdvertisedDevice *pDevice,
                          BLECheeseTimerService::Position position) {
    FreeRTOS::Semaphore reconnect_semaphore;
    /* BLE Client */
    BLEClient *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyBLEClientCallbacks(
        [&](BLEClient *pClient) {
          BLECheeseTimerServiceClient::update_params(
              pClient->getPeerAddress().getNative());
          /* update status LED */
          switch (position) {
          case BLECheeseTimerService::Position::Start:
            logi << "Connected Start" << std::endl;
            pStartLED->on();
            break;
          case BLECheeseTimerService::Position::Goal:
            logi << "Connected Goal" << std::endl;
            pGoalLED->on();
            break;
          }
        },
        [&](BLEClient *pClient) {
          reconnect_semaphore.give();
          /* update status LED */
          switch (position) {
          case BLECheeseTimerService::Position::Start:
            pStartLED->blink();
            logw << "Disconnected Start" << std::endl;
            break;
          case BLECheeseTimerService::Position::Goal:
            pGoalLED->blink();
            logw << "Disconnected Goal" << std::endl;
            break;
          }
        }));
    reconnect_semaphore.take();
    std::string position_string = BLECheeseTimerService::toString(position);
    while (1) {
      pClient->disconnect();
      /* Connect */
      logi << "Connect " << position_string << std::endl;
      while (!pClient->connect(pDevice->getAddress())) {
        loge << "Failed to Connect" << position_string << std::endl;
      }

      /* GATT BAS Client */
      BLEBatteryServiceClient batteryServiceClient(pClient);
      /* GATT Cheese Timer Service Client */
      BLECheeseTimerServiceClient cheeseTimerServiceClient(pClient);
      /* keep until disconnect */
      reconnect_semaphore.take();
    }
  };

  /* Find Device */
  BLEAdvertisedDevice deviceStart = BLECheeseTimerServiceClient::findDevice(
      BLECheeseTimerService::Position::Start);
  // BLEAdvertisedDevice deviceGoal = BLECheeseTimerServiceClient::findDevice(
  //     BLECheeseTimerService::Position::Goal);

  /* handle devices */
  FreeRTOSpp::Thread deviceStartThread([&]() {
    handleDevice(&deviceStart, BLECheeseTimerService::Position::Start);
  });
  // FreeRTOSpp::Thread deviceGoalThread([&]() {
  //   handleDevice(&deviceGoal, BLECheeseTimerService::Position::Goal);
  // });

  /* never return since the objects will be destructed */
  vTaskDelay(portMAX_DELAY);
}
