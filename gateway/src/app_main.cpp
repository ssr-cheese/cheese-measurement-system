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
  LED *pLED[2] = {new LED(PinStartLED), new LED(PinGoalLED)};
  for (auto i : {0, 1})
    pLED[i]->blink();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("Cheese Timer", "");
  // WiFi.softAP("Cheese Timer", "", 1, 1, 4);
  IPAddress apip = WiFi.softAPIP();
  logd << "IPAddress: " << WiFi.softAPIP().toString().c_str() << std::endl;

  WebServer server(80);
  server.begin();
  server.on("/time_ms", [&]() {
    int pos = server.arg("position").toInt();
    pLED[pos]->off();
    server.send(200, "text/plain", String(millis()));
  });
  server.on("/passing", [&]() {
    int pos = server.arg("position").toInt();
    int time_ms = server.arg("time_ms").toInt();
    logi << "Position: " << pos << " Passing at " << time_ms << std::endl;
    pLED[pos]->on();
    server.send(200);
  });
  server.on("/passed", [&]() {
    int pos = server.arg("position").toInt();
    int time_ms = server.arg("time_ms").toInt();
    logi << "Position: " << pos << " Passed at " << time_ms << std::endl;
    pLED[pos]->off();
    server.send(200);
  });
  server.on("/battery", [&]() {
    logi << "Position: " << server.arg("position").c_str()
         << ", Battery: " << server.arg("level").c_str() << " %%" << std::endl;
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
