/**
 * @file ble_cheese_timer_service.h
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief BLE GATT Server Cheese Timer Service
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

class BLECheeseTimerService {
public:
  static const BLEUUID ServiceUUID;
  static const BLEUUID TimeCharacteristicUUID;
  static const BLEUUID MessageCharacteristicUUID;
  static const BLEUUID PositionCharacteristicUUID;

  enum class Position : uint8_t {
    Start,
    Goal,
  };

public:
  /**
   * @brief Construct a new BLECheeseTimerService object
   * @param pServer should not be nullptr
   */
  BLECheeseTimerService(BLEServer *pServer, Position pos = Position::Start);
  /**
   * @brief Set the Value object
   * @param value passed time
   */
  void setPassedTime(uint32_t value);
  /**
   * @brief BLE notify the value
   */
  void notify();
  /**
   * @brief notify the message
   * @param msg string (utf-8)
   */
  void notifyMessage(std::string msg);
  /**
   * @brief Set the Position object
   * @param pos New Position
   */
  void setPosition(Position pos);

protected:
  BLEServer *pServer;
  BLEService *pCheeseService;
  BLECharacteristic *pTimeCharacteristic;
  BLECharacteristic *pMessageCharacteristic;
  BLECharacteristic *pPositionCharacteristic;
};
