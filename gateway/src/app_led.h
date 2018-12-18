/**
 * @file app_led.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief LEDの点灯・点滅マネージャー．キューで渡し，バックグラウンドで駆動．
 * @version 0.1
 * @date 2018-12-18
 *
 * @copyright Copyright (c) 2018
 *
 */
#pragma once

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

class LED {
public:
  LED(gpio_num_t pin, int queue_size = 5) : pin(pin) {
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    playList = xQueueCreate(queue_size, sizeof(struct QueueItem));
    xTaskCreate([](void *obj) { static_cast<LED *>(obj)->task(); }, "LED", 1024,
                this, 0, &pxCreatedTask);
  }
  ~LED() {
    vTaskDelete(pxCreatedTask);
    vQueueDelete(playList);
    gpio_reset_pin(pin);
  }
  void on() {
    QueueItem qi;
    qi.type = ON;
    xQueueSendToBack(playList, &qi, 0);
  }
  void off() {
    QueueItem qi;
    qi.type = OFF;
    xQueueSendToBack(playList, &qi, 0);
  }
  void blink() {
    QueueItem qi;
    qi.type = Blink;
    xQueueSendToBack(playList, &qi, 0);
  }

private:
  gpio_num_t pin;
  QueueHandle_t playList = NULL;
  TaskHandle_t pxCreatedTask = NULL;

  enum Type {
    ON,
    OFF,
    Blink,
  };
  struct QueueItem {
    enum Type type;
  };

  void task() {
    const TickType_t blink_period = 100 / portTICK_PERIOD_MS;
    Type type = OFF;
    bool prev_blink_state = false;
    while (1) {
      switch (type) {
      case Type::ON: {
        write(true);
        QueueItem qi;
        if (xQueueReceive(playList, &qi, portMAX_DELAY) == pdTRUE) {
          type = qi.type;
        }
      } break;
      case Type::OFF: {
        write(false);
        QueueItem qi;
        if (xQueueReceive(playList, &qi, portMAX_DELAY) == pdTRUE) {
          type = qi.type;
        }
      } break;
      case Type::Blink: {
        write(prev_blink_state = !prev_blink_state);
        QueueItem qi;
        if (xQueueReceive(playList, &qi, blink_period) == pdTRUE) {
          type = qi.type;
        }
      } break;
      }
    }
  }
  void write(bool on) { gpio_set_level(pin, on > 0 ? 1 : 0); }
};
