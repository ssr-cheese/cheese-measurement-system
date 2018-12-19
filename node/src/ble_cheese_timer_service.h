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
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLEUUID.h>
#include <thread.h>

#include <functional>

#include "app_log.h"
#include "ble_callback_utils.h"

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
  static std::string toString(Position pos) {
    static const char *position_string[] = {"Start", "Goal"};
    return position_string[static_cast<int>(pos)];
  }

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
  BLECheeseTimerServiceClient(BLEClient *pClient) {
    /* BLE Remote Service */
    pCheeseTimerService =
        pClient->getService(BLECheeseTimerService::ServiceUUID);
    /* Timer Characteristic */
    pTimerCharacteristic = pCheeseTimerService->getCharacteristic(
        BLECheeseTimerService::TimeCharacteristicUUID);
    pTimerCharacteristic->registerForNotify(
        [](BLERemoteCharacteristic *pCharacteristic, uint8_t *pData,
           size_t length, bool isNotify) {
          if (length != 4) {
            loge << "Length Error" << std::endl;
            return;
          }
          uint32_t timer = pData[3];
          timer = (timer << 8) | pData[2];
          timer = (timer << 8) | pData[1];
          timer = (timer << 8) | pData[0];
          logi << "Timer: " << (int)timer << std::endl;
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
    const BLECheeseTimerService::Position *pos =
        reinterpret_cast<const BLECheeseTimerService::Position *>(
            pPositionCharacteristic->readValue().c_str());
    return *pos;
  }
  static BLEAdvertisedDevice
  findDevice(BLECheeseTimerService::Position target_position) {
    // static FreeRTOS::Semaphore scan_semaphore;
    // scan_semaphore.take();
    BLEScan *pScan = BLEDevice::getScan();
    BLEAdvertisedDevice foundDevice;
    bool deviceFound = false;
    /* set scan callback */
    pScan->setAdvertisedDeviceCallbacks(
        new MyAdvertisedDeviceCallbacks([&](BLEAdvertisedDevice dev) {
          if (dev.getServiceDataUUID().equals(
                  BLECheeseTimerService::ServiceUUID)) {
            std::string data = dev.getServiceData();
            BLECheeseTimerService::Position position =
                static_cast<BLECheeseTimerService::Position>((int)data[0]);
            if (position == target_position) {
              logi << "Device Found: " << dev.toString() << std::endl;
              logi << "Position: " << BLECheeseTimerService::toString(position)
                   << std::endl;
              foundDevice = dev;
              deviceFound = true;
              BLEDevice::getScan()->stop();
            }
          }
        }));
    while (1) {
      /* conduct scan */
      logi << "Scan: " << BLECheeseTimerService::toString(target_position)
           << std::endl;
      // this blocks until the target device is found
      BLEScanResults scanResults = pScan->start(1);
      /* unset scan callback */
      if (deviceFound) {
        pScan->setAdvertisedDeviceCallbacks(nullptr);
        // scan_semaphore.give();
        return foundDevice;
      }
    }
  }
  static void update_params(esp_bd_addr_t *addr) {
    /* Update Connection Parameters */
    logi << "update connection params" << std::endl;
    esp_ble_conn_update_params_t conn_param;
    memcpy(conn_param.bda, *addr, sizeof(esp_bd_addr_t));
    conn_param.min_int = 50 / 1.25f;
    conn_param.max_int = 30 / 1.25f;
    conn_param.latency = 0;
    conn_param.timeout = 100;
    ESP_ERROR_CHECK(esp_ble_gap_update_conn_params(&conn_param));
  }

protected:
  BLERemoteService *pCheeseTimerService;
  BLERemoteCharacteristic *pTimerCharacteristic;
  BLERemoteCharacteristic *pMessageCharacteristic;
  BLERemoteCharacteristic *pPositionCharacteristic;
};
