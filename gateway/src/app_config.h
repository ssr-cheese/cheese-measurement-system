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
 * @brief LED Mapping
 */
static constexpr gpio_num_t PinStartLED = GPIO_NUM_27;
static constexpr gpio_num_t PinGoalLED = GPIO_NUM_26;
