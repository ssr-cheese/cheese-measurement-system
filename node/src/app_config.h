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

#include <driver/adc.h>
#include <driver/gpio.h>

/**
 * @brief name of this device
 */
static constexpr const char *DeviceName = "Cheese Timer Node";

/**
 * @brief Battery Monitor Parameters
 */
static constexpr adc1_channel_t BatteryAdcChannel =
    ADC1_CHANNEL_7; //< ADC1_Channel7 is GPIO35
// static constexpr float BatteryMinVoltage = 4.8f;
// static constexpr float BatteryMaxVoltage = 6.0f;
static constexpr float BatteryMinVoltage = 3.6f;
static constexpr float BatteryMaxVoltage = 4.2f;

/**
 * @brief LED Mapping
 */
static constexpr gpio_num_t PinSensorStatusLED = GPIO_NUM_27;
static constexpr gpio_num_t PinErrorStatusLED = GPIO_NUM_26;
