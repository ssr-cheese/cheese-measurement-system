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
      return 75; /**< Micro Mouse */
    } else {
      logi << "This is Classic Mouse Field." << std::endl;
      return 150; /**< Classic Mouse */
    }
  }();

  /* WiFi connection */
  WiFi.begin("Cheese Timer", "");
  WiFi.setAutoReconnect(true);
  WiFi.waitForConnectResult();
  if (!WiFi.isConnected()) {
    loge << "WiFi Failed" << std::endl;
    vTaskDelay(portMAX_DELAY);
  }
  logi << "WiFi Connected" << std::endl;
  pErrorStatusLED->off();

  {
    logi << "GET" << std::endl;
    HTTPClient client;
    client.begin("http://192.168.4.1/0");
    client.GET();
    client.end();
  }

  FreeRTOSpp::Thread wifiMonitorThread([&]() {
    const auto period = std::chrono::milliseconds(100);
    auto sleep_time_handle = std::chrono::steady_clock::now();
    while (1) {
      /* Periodical EXecution */
      std::this_thread::sleep_until(sleep_time_handle += period);
      if (!WiFi.isConnected()) {
        pErrorStatusLED->blink();
        WiFi.begin();
        WiFi.waitForConnectResult();
      }
    }
  });

  /* Battery Monitor Thread in Background*/
  FreeRTOSpp::Thread batteryMonitorThread([&]() {
    const auto period = std::chrono::seconds(1);
    // const auto period = std::chrono::milliseconds(500);
    auto sleep_time_handle = std::chrono::steady_clock::now();
    while (1) {
      /* Periodical EXecution */
      std::this_thread::sleep_until(sleep_time_handle += period);
      float voltage = pBatteryMonitor->getVoltage();
      uint8_t level = pBatteryMonitor->calcBatteryLevel(voltage);
      logd << "Battery Voltage: " << voltage << "[V], Level: " << (int)level
           << std::endl;
      if (level < 10)
        pSensorStatusLED->blink();
      HTTPClient client;
      client.begin("http://192.168.4.1/0/battery?level=" + String(level));
      client.GET();
      client.end();
    }
  });

  /* Cheese Time Thread in Background */
  FreeRTOSpp::Thread cheeseTimerThread([&]() {
    bool passing = false;
    while (1) {
      int range_mm = tof.read();
      if (range_mm < Threshold_mm) {
        if (!passing) {
          passing = true;
          pSensorStatusLED->on();
          uint32_t value =
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now().time_since_epoch())
                  .count();
          logd << "New Passed: " << (int)value << " [ms], " << range_mm
               << " [mm]" << std::endl;
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
  /* sleep forever */
  vTaskDelay(portMAX_DELAY);
}

void loop() {}
