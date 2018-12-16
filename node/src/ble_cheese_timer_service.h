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
#include <BLEClient.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLEUUID.h>

#include <functional>

#include "app_log.h"

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

class BLECheeseTimerServiceClient {
public:
  BLECheeseTimerServiceClient(
      BLEClient *pClient,
      std::function<void(uint32_t)> notifyCallback = nullptr) {
    /* BLE Remote Service */
    pCheeseTimerService =
        pClient->getService(BLECheeseTimerService::ServiceUUID);
    /* Timer Characteristic */
    pTimerCharacteristic = pCheeseTimerService->getCharacteristic(
        BLECheeseTimerService::TimeCharacteristicUUID);
    pTimerCharacteristic->registerForNotify(
        [notifyCallback](BLERemoteCharacteristic *pCharacteristic,
                         uint8_t *pData, size_t length, bool isNotify) {
          if (length != 4) {
            loge << "Length Error" << std::endl;
            return;
          }
          uint32_t timer = pData[3];
          timer = (timer << 8) | pData[2];
          timer = (timer << 8) | pData[1];
          timer = (timer << 8) | pData[0];
          logi << "Timer: " << (int)timer << std::endl;
          if (notifyCallback != nullptr)
            notifyCallback(timer);
        });
    /* Message Characteristic */
    pMessageCharacteristic = pCheeseTimerService->getCharacteristic(
        BLECheeseTimerService::MessageCharacteristicUUID);
    // logi << "Message: " << readMessage() << std::endl;
    pMessageCharacteristic->registerForNotify(
        [](BLERemoteCharacteristic *pCharacteristic, uint8_t *pData,
           size_t length, bool isNotify) {
          logi << "Message: " << std::string((const char *)pData) << std::endl;
        });
    /* Position Characteristic */
    pPositionCharacteristic = pCheeseTimerService->getCharacteristic(
        BLECheeseTimerService::PositionCharacteristicUUID);
    // logi << "Position: " << static_cast<int>(readPosition()) << std::endl;
    pPositionCharacteristic->registerForNotify(
        [](BLERemoteCharacteristic *pCharacteristic, uint8_t *pData,
           size_t length, bool isNotify) {
          logi << "Position: " << (int)pData[0] << std::endl;
        });
  }

  uint32_t readTimer() {
    pTimerCharacteristic->readValue();
    uint32_t timer = pTimerCharacteristic->readUInt32();
    return timer;
  }
  std::string readMessage() {
    std::string msg = pMessageCharacteristic->readValue();
    return msg;
  }
  BLECheeseTimerService::Position readPosition() {
    pPositionCharacteristic->readValue();
    BLECheeseTimerService::Position *pos =
        reinterpret_cast<BLECheeseTimerService::Position *>(
            pPositionCharacteristic->readRawData());
    return *pos;
  }

protected:
  BLERemoteService *pCheeseTimerService;
  BLERemoteCharacteristic *pTimerCharacteristic;
  BLERemoteCharacteristic *pMessageCharacteristic;
  BLERemoteCharacteristic *pPositionCharacteristic;
};
