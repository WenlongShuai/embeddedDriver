#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
 
// 定义一个全局变量来存储特征值
BLECharacteristic *pCharacteristic;

// 定义一个标志，用于判断是否有客户端连接
bool deviceConnected = false;
char BLEbuf[32] = {0};
uint32_t cnt = 0;

// 定义服务和特征的UUID
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
 
// 定义一个回调类，用于处理客户端连接和断开事件
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
 
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      // 在这里添加重新开启广播的代码
        BLEDevice::startAdvertising();
    }
};

// 定义一个回调类，用于处理特征值的读写事件
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue().c_str();
 
      if (rxValue.length() > 0) { 
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }
        Serial.println();
      }
    }
};
void setup() {
	Serial.begin(115200);
	
	// Create the BLE Device
	BLEDevice::init("ESP32 BLE Test"); 
	
	// Create the BLE Server
	BLEServer *pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());
	
	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);
	
	// Create a BLE Characteristic
	pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

	// 为特征添加一个描述符     
	pCharacteristic->addDescriptor(new BLE2902());
	
	//BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
	
	// 设置特征的回调函数
	pCharacteristic->setCallbacks(new MyCallbacks());
	
	// Start the service
	pService->start();
	
	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");
	}
 
void loop() {
	if (deviceConnected) {
		// memset(BLEbuf, 0, 32);
		// memcpy(BLEbuf, (char*)"Hello BLE APP!", 32); 
		// pCharacteristic->setValue(BLEbuf);
		
		// pCharacteristic->notify(); // Send the value to the app!
		// Serial.print("*** Sent Value: ");
		// Serial.print(BLEbuf);
		// Serial.println(" ***");

		if(Serial.available())
		{
			char c = Serial.read();
			pCharacteristic->setValue(&c);
			pCharacteristic->notify(); // Send the value to the app!
			Serial.println(c);
		}
	}
	delay(1000);
}

