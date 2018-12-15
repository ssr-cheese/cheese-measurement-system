/**
 * @file ble_cheese_timer_service.cpp
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief BLE GATT Server Cheese Timer Service
 * @version 0.1
 * @date 2018-12-12
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 *
 */
#include "ble_cheese_timer_service.h"

#include <BLE2902.h>
#include <BLE2904.h>

#include <iomanip>
#include <iostream>

#include "app_log.h"

const BLEUUID BLECheeseTimerService::ServiceUUID =
    BLEUUID("e2830000-fdb1-11e8-8eb2-f2801f1b9fd1");

const BLEUUID BLECheeseTimerService::TimeCharacteristicUUID =
    BLEUUID("e2830001-fdb1-11e8-8eb2-f2801f1b9fd1");

const BLEUUID BLECheeseTimerService::MessageCharacteristicUUID =
    BLEUUID("e2830002-fdb1-11e8-8eb2-f2801f1b9fd1");

const BLEUUID BLECheeseTimerService::PositionCharacteristicUUID =
    BLEUUID("e2830003-fdb1-11e8-8eb2-f2801f1b9fd1");

BLECheeseTimerService::BLECheeseTimerService(BLEServer *pServer, Position pos)
    : pServer(pServer) {
  /* Cheese Timer Service */
  pCheeseService = pServer->createService(ServiceUUID);
  {
    /* Cheese Timer Characteristic */
    pTimeCharacteristic = pCheeseService->createCharacteristic(
        TimeCharacteristicUUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    /* setup event callback */
    class CheeseCharacteristicCallbacks : public BLECharacteristicCallbacks {
    public:
      CheeseCharacteristicCallbacks() {}
      virtual void onRead(BLECharacteristic *pCharacteristic) override {
        logi << pCharacteristic->toString() << std::endl;
      }
      virtual void onWrite(BLECharacteristic *pCharacteristic) override {
        logi << pCharacteristic->toString() << std::endl;
      }
    };
    pTimeCharacteristic->setCallbacks(new CheeseCharacteristicCallbacks());
    /* BLE CUD: Characteristic User Description (0x2901) */
    BLEDescriptor *pBLE2901 = new BLEDescriptor(static_cast<uint16_t>(0x2901));
    pBLE2901->setValue("PassedTime");
    pTimeCharacteristic->addDescriptor(pBLE2901);
    /* BLE CCCD: Client Characteristic Configuration Description (0x2902) */
    BLE2902 *pBLE2902 = new BLE2902();
    class BLE2902Callback : public BLEDescriptorCallbacks {
    public:
      BLE2902Callback() {}
      virtual void onWrite(BLEDescriptor *pDescriptor) override {
        logi << pDescriptor->toString() << std::endl;
      }
    };
    pBLE2902->setCallbacks(new BLE2902Callback());
    pTimeCharacteristic->addDescriptor(pBLE2902);
    /* BLE CPFD: Characteristic Presentation Format Descriptor (0x2904) */
    BLE2904 *pBLE2904 = new BLE2904();
    pBLE2904->setFormat(BLE2904::FORMAT_UINT32);
    pTimeCharacteristic->addDescriptor(pBLE2904);
  }
  {
    /* Message Characteristic */
    pMessageCharacteristic = pCheeseService->createCharacteristic(
        MessageCharacteristicUUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    pMessageCharacteristic->setValue("Inital Message");
    /* BLE CUD: Characteristic User Description (0x2901) */
    BLEDescriptor *pBLE2901 = new BLEDescriptor(static_cast<uint16_t>(0x2901));
    pBLE2901->setValue("Message");
    pMessageCharacteristic->addDescriptor(pBLE2901);
    /* BLE CCCD: Client Characteristic Configuration Description (0x2902) */
    BLE2902 *pBLE2902 = new BLE2902();
    pMessageCharacteristic->addDescriptor(pBLE2902);
    /* BLE CPFD: Characteristic Presentation Format Descriptor (0x2904) */
    BLE2904 *pBLE2904 = new BLE2904();
    pBLE2904->setFormat(BLE2904::FORMAT_UTF8);
    pMessageCharacteristic->addDescriptor(pBLE2904);
  }
  {
    /* Position Characteristic */
    pPositionCharacteristic = pCheeseService->createCharacteristic(
        PositionCharacteristicUUID, BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_NOTIFY |
                                        BLECharacteristic::PROPERTY_WRITE);
    pPositionCharacteristic->setValue((uint8_t *)(&pos), sizeof(Position));
    /* BLE CUD: Characteristic User Description (0x2901) */
    BLEDescriptor *pBLE2901 = new BLEDescriptor(static_cast<uint16_t>(0x2901));
    pBLE2901->setValue("Position");
    pPositionCharacteristic->addDescriptor(pBLE2901);
    /* BLE CCCD: Client Characteristic Configuration Description (0x2902) */
    BLE2902 *pBLE2902 = new BLE2902();
    pMessageCharacteristic->addDescriptor(pBLE2902);
    /* BLE CPFD: Characteristic Presentation Format Descriptor (0x2904) */
    BLE2904 *pBLE2904 = new BLE2904();
    pBLE2904->setFormat(BLE2904::FORMAT_UINT8);
    pPositionCharacteristic->addDescriptor(pBLE2904);
  }
  /* start service */
  pCheeseService->start();
}

void BLECheeseTimerService::setPassedTime(uint32_t value) {
  pTimeCharacteristic->setValue(value);
}

void BLECheeseTimerService::notify() { pTimeCharacteristic->notify(); }

void BLECheeseTimerService::notifyMessage(std::string msg) {
  pMessageCharacteristic->setValue(msg);
  pMessageCharacteristic->notify();
}

void BLECheeseTimerService::setPosition(Position pos) {
  pPositionCharacteristic->setValue((uint8_t *)(&pos), sizeof(Position));
}
