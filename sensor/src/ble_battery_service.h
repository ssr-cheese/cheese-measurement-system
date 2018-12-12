/**
 * @file ble_battery_service.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief BLE GATT Server Battery Service
 * @version 0.1
 * @date 2018-12-12
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 *
 */
#pragma once

#include <BLECharacteristic.h>
#include <BLEServer.h>
#include <BLEService.h>

class BLEBatteryService {
public:
  /**
   * @brief Construct a new BLEBatteryService object
   * @param pServer should not be nullptr
   */
  BLEBatteryService(BLEServer *pServer);
  /**
   * @brief Set the Battery Level object
   * @param level 0-100
   */
  void setBatteryLevel(uint8_t level);
  /**
   * @brief notify the battery level characteristic
   */
  void notify();

protected:
  BLEServer *pServer;
  BLEService *pBatteryService;
  BLECharacteristic *pBatteryLevelCharacteristic;
};
