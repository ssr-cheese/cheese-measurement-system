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
 * @brief LED Mapping
 * [0]: Start
 * [1]: Goal
 */
static constexpr gpio_num_t PinSensorLED[2] = {GPIO_NUM_27, GPIO_NUM_26};
static constexpr gpio_num_t PinConnectionLED[2] = {GPIO_NUM_23, GPIO_NUM_25};
