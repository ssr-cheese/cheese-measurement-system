#pragma once

/**
 * @brief Battery Monitor Parameters
 */
static constexpr adc1_channel_t BatteryAdcChannel =
    ADC1_CHANNEL_7; //< ADC1_Channel7 is GPIO35
static constexpr float BatteryMinVoltage = 4.8f;
static constexpr float BatteryMaxVoltage = 6.0f;

/**
 * @brief LED Mapping
 *
 */

static constexpr gpio_num_t PinSensorStatusLED = GPIO_NUM_27;
static constexpr gpio_num_t PinErrorStatusLED = GPIO_NUM_26;
