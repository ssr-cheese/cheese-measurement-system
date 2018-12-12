/**
 * @file ble_cheese_measurement_service.cpp
 * @author Ryotaro Onuki (kerikun11+github@gmail.com)
 * @brief BLE GATT Server Cheese Measurement Service
 * @version 0.1
 * @date 2018-12-12
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 *
 */
#include "ble_cheese_measurement_service.h"

#include <BLE2902.h>
#include <BLE2904.h>

#include <iomanip>
#include <iostream>

#include "app_log.h"

const BLEUUID BLECheeseMeasurementService::ServiceUUID =
    BLEUUID("e2830000-fdb1-11e8-8eb2-f2801f1b9fd1");

const BLEUUID BLECheeseMeasurementService::DataCharacteristicUUID =
    BLEUUID("e2830001-fdb1-11e8-8eb2-f2801f1b9fd1");

BLECheeseMeasurementService::BLECheeseMeasurementService(BLEServer *pServer)
    : pServer(pServer) {
  /* Cheese Measurement Service */
  pCheeseService = pServer->createService(ServiceUUID);
  /* Cheese Measurement Characteristic */
  pCheeseCharacteristic = pCheeseService->createCharacteristic(
      DataCharacteristicUUID,
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
  pCheeseCharacteristic->setCallbacks(new CheeseCharacteristicCallbacks());
  /* BLE CUD: Characteristic User Description (0x2901) */
  BLEDescriptor *pBLE2901 = new BLEDescriptor(static_cast<uint16_t>(0x2901));
  pBLE2901->setValue("CheeseMeasurementData");
  pCheeseCharacteristic->addDescriptor(pBLE2901);
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
  pCheeseCharacteristic->addDescriptor(pBLE2902);
  /* BLE CPFD: Characteristic Presentation Format Descriptor (0x2904) */
  BLE2904 *pBLE2904 = new BLE2904();
  pBLE2904->setFormat(BLE2904::FORMAT_UINT32);
  pCheeseCharacteristic->addDescriptor(pBLE2904);
  /* start service */
  pCheeseService->start();
}

void BLECheeseMeasurementService::setValue(uint32_t value) {
  pCheeseCharacteristic->setValue(value);
}

void BLECheeseMeasurementService::notify() { pCheeseCharacteristic->notify(); }
