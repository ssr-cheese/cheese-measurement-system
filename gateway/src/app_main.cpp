/**
 * @file app_main.cpp
 * @author Ryotaro Onuki (kerikun11@gmail.com)
 * @brief Cheese Timer Gateway Part
 * @version 0.1
 * @date 2018-12-11
 *
 * @copyright Copyright (c) 2018 Ryotaro Onuki
 *
 */
#include <BLEClient.h>
#include <BLEDevice.h>
#include <FreeRTOS.h>
#include <nvs_flash.h>

#include <functional>
#include <iomanip>
#include <iostream>

// コンパイルエラーを防ぐため， Arduino.h で定義されているマクロをundef
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <chrono>

#include "app_log.h"
#include "ble_battery_service.h"
#include "ble_cheese_timer_service.h"

extern "C" void app_main() {
  /* NVS flash initialization */
  nvs_flash_init();

  /* BLE Initialization */
  BLEDevice::init("Cheese Timer Gateway");

  /* BLE Scan */
  BLEAdvertisedDevice advertisedDevice;
  {
    BLEScan *pScan = BLEDevice::getScan();
    /* set scan callback */
    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    public:
      MyAdvertisedDeviceCallbacks(
          BLEAdvertisedDevice *pAdvertisedDevice,
          std::function<bool(BLEAdvertisedDevice *pAdvertisedDevice)>
              isTargetDevice)
          : pAdvertisedDevice(pAdvertisedDevice),
            isTargetDevice(isTargetDevice) {}
      virtual void onResult(BLEAdvertisedDevice advertisedDevice) override {
        logi << "onResult(): " << advertisedDevice.toString() << std::endl;
        if (isTargetDevice(&advertisedDevice)) {
          logi << "device found" << std::endl;
          *pAdvertisedDevice = advertisedDevice;
          BLEDevice::getScan()->stop();
        }
      }

    private:
      BLEAdvertisedDevice *pAdvertisedDevice;
      std::function<bool(BLEAdvertisedDevice *pAdvertisedDevice)>
          isTargetDevice;
    };
    MyAdvertisedDeviceCallbacks advertisedDeviceCallbacks(
        &advertisedDevice, [](BLEAdvertisedDevice *pAdvertisedDevice) {
          return pAdvertisedDevice->isAdvertisingService(
              BLECheeseTimerService::ServiceUUID);
        });
    pScan->setAdvertisedDeviceCallbacks(&advertisedDeviceCallbacks);
    /* conduct scan */
    // this blocks until the target device is found
    BLEScanResults scanResults = pScan->start(60);
    /* unset scan callback */
    pScan->setAdvertisedDeviceCallbacks(nullptr);
  }

  /* BLE Client */
  BLEClient *pClient = BLEDevice::createClient();
  pClient->connect(&advertisedDevice);

  /* GATT BAS */
  BLEBatteryServiceClient *pBLEBatteryServiceClient __attribute__((unused)) =
      new BLEBatteryServiceClient(pClient,[](uint8_t level){
        logi << (int)level << std::endl;
      });
  /* BLE Remote Service: CCMS */
  BLERemoteService *pBLECheeseService =
      pClient->getService(BLECheeseTimerService::ServiceUUID);
  std::cout << pBLECheeseService->toString() << std::endl;
  /* BLE Remote Characteristic */
  BLERemoteCharacteristic *pBLECheeseData =
      pBLECheeseService->getCharacteristic(
          BLECheeseTimerService::DataCharacteristicUUID);
  std::cout << pBLECheeseService->toString() << std::endl;
  /* setup notify callback */
  pBLECheeseData->registerForNotify(
      [](BLERemoteCharacteristic *pBLECheeseService, uint8_t *pData,
         size_t length, bool isNotify) {
        logi << pBLECheeseService->toString() << std::endl;
        logi << "Length:" << length << std::endl;
      });

  /* BLE Advertising */
  BLEAdvertising *pBLEAdvertising = BLEDevice::getAdvertising();
  pBLEAdvertising->start();

  /* Main Loop */
  while (1) {
    FreeRTOS::sleep(2222);
  }

  /* sleep forever */
  vTaskDelay(portMAX_DELAY);
}
