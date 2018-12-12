/**
 * @file ble_cheese_measurement_service.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief BLE GATT Server Cheese Measurement Service
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
#include <BLEUUID.h>

class BLECheeseMeasurementService {
public:
  static const BLEUUID ServiceUUID;
  static const BLEUUID DataCharacteristicUUID;

public:
  /**
   * @brief Construct a new BLECheeseMeasurementService object
   * @param pServer should not be nullptr
   */
  BLECheeseMeasurementService(BLEServer *pServer);
  /**
   * @brief Set the Value object
   * @param value passed time?
   */
  void setValue(uint32_t value);
  /**
   * @brief BLE notify the value
   */
  void notify();

protected:
  BLEServer *pServer;
  BLEService *pCheeseService;
  BLECharacteristic *pCheeseCharacteristic;
};
