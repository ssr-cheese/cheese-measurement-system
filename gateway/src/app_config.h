/**
 * @file app_config.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief ボード特有の定数などを定義
 * @version 0.1
 * @date 2018-12-18
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 */
#pragma once

#include <driver/gpio.h>

/**
 * @brief name of this device
 */
static constexpr const char *DeviceName = "Cheese Timer Node";

/**
 * @brief WiFI Information
 */
static constexpr const char *WiFiSSID = "Cheese Timer";
static constexpr const char *WiFiPSK = "ssrcheese";

/**
 * @brief LED Mapping
 * [0]: Start
 * [1]: Goal
 */
static constexpr gpio_num_t PinConnectionLED[2] = {GPIO_NUM_32, GPIO_NUM_33};
static constexpr gpio_num_t PinSensorLED[2] = {GPIO_NUM_26, GPIO_NUM_27};
