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

/* config */
#include "app_config.h"
#include "app_log.h"

/* src */
#include "app_led.h"

/* private lib */
#include <thread.h>

/* env lib */
#include <Arduino.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>

/* std lib */
/* コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef */
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

#include <functional>
#include <iomanip>
#include <iostream>

void setup() {
  /* Boot Message */
  logi << "Hello, this is " << DeviceName << "." << std::endl;

  /* GPIO Initialization */
  LED *pStartLED = new LED(PinStartLED);
  LED *pGoalLED = new LED(PinGoalLED);
  pStartLED->blink();
  pGoalLED->blink();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Cheese Timer", "");
  // WiFi.softAP("Cheese Timer", "", 1, 1, 4);
  IPAddress apip = WiFi.softAPIP();
  logd << "IPAddress: " << WiFi.softAPIP().toString().c_str() << std::endl;

  WebServer server(80);
  server.begin();
  server.on("/0/battery", [&]() {
    logi << "/0/battery" << std::endl;
    logi << "Battery: " << server.arg("level").c_str() << " %%" << std::endl;
    pStartLED->on();
    server.send(200);
  });
  server.on("/0", [&]() {
    logi << "/0" << std::endl;
    pStartLED->on();
    server.send(200);
  });
  server.on("/1", [&]() {
    logi << "/1" << std::endl;
    pGoalLED->on();
    server.send(200);
  });
  server.onNotFound([&]() { logi << "NotFound" << std::endl; });

  while (1) {
    server.handleClient();
  }

  /* wait forever */
  vTaskDelay(portMAX_DELAY);
}

void loop() {}
