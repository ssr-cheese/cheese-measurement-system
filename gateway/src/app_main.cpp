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

#include <functional> /**< std::function */
#include <iomanip>
#include <iostream>

void setup() {
  /* Boot Message */
  logi << "Hello, this is " << DeviceName << "." << std::endl;

  /* GPIO Initialization */
  LED *pConnectionLED[2] = {new LED(PinConnectionLED[0]),
                            new LED(PinConnectionLED[1])};
  LED *pSensorLED[2] = {new LED(PinSensorLED[0]), new LED(PinSensorLED[1])};
  for (auto i : {0, 1})
    pConnectionLED[i]->blink();
  for (auto i : {0, 1})
    pSensorLED[i]->off();

  /* WiFi Accesspoint Configuration */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WiFiSSID, WiFiPSK);
  IPAddress apip = WiFi.softAPIP();
  logd << "IPAddress: " << WiFi.softAPIP().toString().c_str() << std::endl;

  /* Keep Alive Timer */
  int prev_connection_time_ms[2] = {0, 0};
  bool isConnected[2] = {false, false};

  /* HTTP Web Server Configuration */
  WebServer server(80); /**< TCP/IP port: 80 */
  server.begin();
  server.on("/time_ms", [&]() {
    const int pos = server.arg("position").toInt();
    pConnectionLED[pos]->on();
    server.send(200, "text/plain", String(millis()));
  });
  server.on("/passing", [&]() {
    const int pos = server.arg("position").toInt();
    const int time_ms = server.arg("time_ms").toInt();
    logi << "Position: " << pos << " Passing at " << time_ms << std::endl;
    pSensorLED[pos]->on();
    /* to matlab */
    std::cout << "1145141919 " << pos << " " << time_ms << std::endl;
    server.send(200);
  });
  server.on("/passed", [&]() {
    const int pos = server.arg("position").toInt();
    int time_ms = server.arg("time_ms").toInt();
    logi << "Position: " << pos << " Passed at " << time_ms << std::endl;
    pSensorLED[pos]->off();
    server.send(200);
  });
  server.on("/battery", [&]() {
    const int pos = server.arg("position").toInt();
    logi << "Position: " << pos
         << ", Battery: " << server.arg("voltage").c_str() << " [V]"
         << std::endl;
    prev_connection_time_ms[pos] = millis();
    isConnected[pos] = true;
    pConnectionLED[pos]->on();
    server.send(200);
  });
  server.onNotFound([&]() { logi << "NotFound" << std::endl; });

  /* Handling Requests */
  while (1) {
    server.handleClient();
    /* 一定時間未接続となったデバイスを検出 */
    for (int i = 0; i < 2; ++i) {
      if (isConnected[i] && millis() > prev_connection_time_ms[i] + 4000) {
        isConnected[i] = false;
        pConnectionLED[i]->blink();
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }

  /* it never reaches here */
  vTaskDelay(portMAX_DELAY);
}

void loop() {}
