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

/* config */
#include "app_config.h"
#include "app_log.h"

/* src */
#include "app_led.h"
#include "battery_monitor.h"
#include "ble_cheese_timer_service.h"

/* private lib */
#include <VL53L0X.h>
#include <VL6180X.h>
#include <thread.h>

/* env lib */
#include <HTTPClient.h>
#include <WiFi.h>

/* std */
/* コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef */
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

#include <thread>

void setup() {
  /* Boot Message */
  logi << "Hello, this is " << DeviceName << "." << std::endl;
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
  int pos = static_cast<int>(position);

  /* ToF Sensor Initialization */
  VL6180X tof;
  // VL53L0X tof;
  tof.i2cMasterInit();
  if (!tof.init()) {
    pErrorStatusLED->on();
    vTaskDelay(portMAX_DELAY);
  }
  /* Determine Threshold of ToF */
  const int Threshold_mm = [&]() {
    int range_mm = tof.read();
    if (range_mm < 135) {
      logi << "This is Micro Mouse Field." << std::endl;
      return 80; /**< Micro Mouse */
    } else {
      logi << "This is Classic Mouse Field." << std::endl;
      return 160; /**< Classic Mouse */
    }
  }();

  /* WiFi connection */
  while (1) {
    WiFi.begin("Cheese Timer", "ssrcheese");
    WiFi.waitForConnectResult();
    if (WiFi.isConnected())
      break;
    loge << "WiFi Failed" << std::endl;
  }
  logi << "WiFi Connected" << std::endl;
  pErrorStatusLED->off();

  int time_offset_ms;
  {
    logi << "GET" << std::endl;
    HTTPClient client;
    client.begin("http://192.168.4.1/time_ms?position=" + String(pos));
    client.GET();
    int time_ms = client.getStream().readString().toInt();
    time_offset_ms = time_ms - millis();
    logi << "Gateway: " << time_ms << ", offset: " << time_offset_ms
         << std::endl;
    client.end();
  }

  FreeRTOSpp::Thread wifiMonitorThread([&]() {
    const auto period = std::chrono::milliseconds(100);
    auto sleep_time_handle = std::chrono::steady_clock::now();
    while (1) {
      /* Periodical EXecution */
      std::this_thread::sleep_until(sleep_time_handle += period);
      // if (!WiFi.isConnected()) {
      //   pErrorStatusLED->blink();
      //   esp_restart();
      // }
    }
  });

  /* Battery Monitor Thread in Background*/
  FreeRTOSpp::Thread batteryMonitorThread([&]() {
    const auto period = std::chrono::seconds(3);
    // const auto period = std::chrono::milliseconds(500);
    auto sleep_time_handle = std::chrono::steady_clock::now();
    while (1) {
      /* Periodical EXecution */
      std::this_thread::sleep_until(sleep_time_handle += period);
      float voltage = pBatteryMonitor->getVoltage();
      uint8_t level = pBatteryMonitor->calcBatteryLevel(voltage);
      logd << "Battery Voltage: " << voltage << " [V], Level: " << (int)level
           << std::endl;
      if (level < 10)
        pSensorStatusLED->blink();
      HTTPClient client;
      client.setTimeout(1000);
      client.begin("http://192.168.4.1/battery?position=" + String(pos) +
                   "&voltage=" + String(voltage));
      int error = client.GET();
      if (error < 0) {
        loge << "HTTPClient Error: " << HTTPClient::errorToString(error)
             << std::endl;
      }
      client.end();
    }
  });

  struct QueueItem {
    enum Event {
      Passing,
      Passed,
    } event;
    uint32_t time_ms;
    QueueItem(Event event, uint32_t t) : event(event), time_ms(t) {}
    QueueItem() {}
  };
  const int queue_size = 10;
  QueueHandle_t event_queue =
      xQueueCreate(queue_size, sizeof(struct QueueItem));

  /* Cheese Time Thread in Background */
  FreeRTOSpp::Thread cheeseTimerThread([&]() {
    bool passing_prev = false;
    while (1) {
      bool passing = (tof.read() < Threshold_mm);
      if (passing_prev != passing) {
        passing_prev = passing;
        if (passing) {
          logd << "ToF Covered" << std::endl;
          pSensorStatusLED->on();
          QueueItem qi(QueueItem::Passing, millis());
          xQueueSendToBack(event_queue, &qi, 0);
        } else {
          logd << "ToF Released" << std::endl;
          pSensorStatusLED->off();
          QueueItem qi(QueueItem::Passed, millis());
          xQueueSendToBack(event_queue, &qi, 0);
        }
        /* Prevent from chattering */
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
    }
  });
  FreeRTOSpp::Thread timeSendThread([&]() {
    while (1) {
      QueueItem qi;
      if (xQueueReceive(event_queue, &qi, portMAX_DELAY) == pdTRUE) {
        int time_ms = time_offset_ms + qi.time_ms;
        HTTPClient client;
        client.begin(String("http://192.168.4.1/") +
                     ((qi.event == QueueItem::Passing) ? "passing" : "passed") +
                     "?position=" + String(pos) +
                     "&time_ms=" + String(time_ms));
        int error = client.GET();
        if (error < 0) {
          loge << "HTTPClient Error: " << HTTPClient::errorToString(error)
               << std::endl;
        }
        client.end();
      }
    }
  });

  /* sleep forever */
  vTaskDelay(portMAX_DELAY);
}

void loop() {}
