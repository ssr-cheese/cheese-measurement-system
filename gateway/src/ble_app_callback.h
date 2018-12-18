#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <functional>

class AppBLEServerCallbacks : public BLEServerCallbacks {
public:
  AppBLEServerCallbacks(
      std::function<void(BLEServer *)> onConnectCallback = nullptr,
      std::function<void(BLEServer *)> onDisconnectCallback = nullptr)
      : onConnectCallback(onConnectCallback),
        onDisconnectCallback(onDisconnectCallback) {}
  virtual void onConnect(BLEServer *pServer) override {
    if (onConnectCallback != nullptr)
      onConnectCallback(pServer);
  }
  virtual void onDisconnect(BLEServer *pServer) override {
    if (onDisconnectCallback != nullptr)
      onDisconnectCallback(pServer);
  }

private:
  std::function<void(BLEServer *)> onConnectCallback;
  std::function<void(BLEServer *)> onDisconnectCallback;
};

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
public:
  MyAdvertisedDeviceCallbacks(
      std::function<void(BLEAdvertisedDevice)> onResultCallback)
      : onResultCallback(onResultCallback) {}
  virtual void onResult(BLEAdvertisedDevice advertisedDevice) override {
    if (onResultCallback != nullptr)
      onResultCallback(advertisedDevice);
  }

private:
  std::function<void(BLEAdvertisedDevice)> onResultCallback;
};
