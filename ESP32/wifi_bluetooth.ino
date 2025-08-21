#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Wi-Fi 配置
const char* ssid = "ESP32_WIFI";
const char* password = "12345678";
const int tcpPort = 8888; // TCP服务器端口

// 蓝牙配置
BLEServer* pServer = NULL;
BLECharacteristic* pTxCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// 蓝牙服务 UUID
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART 服务 UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// WiFi TCP服务器
WiFiServer server(tcpPort);
WiFiClient client;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("蓝牙已连接");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("蓝牙已断开");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.print("B:");
        
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);
        Serial.println();
      }

      delay(20);
      if(Serial.available() > 0){
        // 获取可用字节数
        int bytesAvailable = Serial.available();
        // 读取所有数据
        char incomingData[64]; // 定义缓冲区大小
        int i = 0;
        
        while (i < bytesAvailable) {
          incomingData[i] = Serial.read();
          i++;
        }
        incomingData[i] = '\0'; // 添加字符串结束符

        pTxCharacteristic->setValue(incomingData);
        pTxCharacteristic->notify(); // Send the value to the app!
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // 初始化 Wi-Fi
  Serial.println("设置 Wi-Fi 接入点...");
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP 地址: ");
  Serial.println(IP);
  
  // 启动 TCP 服务器
  server.begin();
  Serial.print("TCP服务器已启动，端口: ");
  Serial.println(tcpPort);
  
  // 初始化蓝牙
  BLEDevice::init("ESP32_BLUE");
  
  // 创建 BLE 服务器
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // 创建 BLE 服务
  BLEService* pService = pServer->createService(SERVICE_UUID);
  
  // 创建 BLE 特征
  pTxCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pTxCharacteristic->addDescriptor(new BLE2902());
  
  BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                       BLECharacteristic::PROPERTY_WRITE
                     );
                     
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  
  // 启动服务
  pService->start();
  
  // 启动广告
  pServer->getAdvertising()->start();
  Serial.println("等待蓝牙客户端连接...");
}

void loop() {
  // 处理WiFi客户端连接
    if (!client.connected()) {
        // 检查是否有新连接
        if (server.hasClient()) {
            if (client) {
                // 已有客户端，关闭旧连接
                client.stop();
                Serial.println("关闭旧的WiFi连接");
            }
            client = server.available();
            Serial.println("新的WiFi客户端已连接");
        }
    } 
    else {
        // 客户端已连接，检查是否有数据
        unsigned char flag = 0;
        // 读取所有可用字节
        while (client.available()) {
          String data = client.readStringUntil('\n');
          Serial.print("W:");
          Serial.print(data);
          Serial.println();  // 添加空格分隔符
          flag = 1;
        }

        delay(20);
        if(Serial.available() > 0 && flag == 1)
        {
          // 获取可用字节数
          int bytesAvailable = Serial.available();
          // 读取所有数据
          char incomingData[64]; // 定义缓冲区大小
          int i = 0;
          while (i < bytesAvailable) {
            incomingData[i] = Serial.read();
            i++;
          }
          incomingData[i] = '\0'; // 添加字符串结束符
          client.println(incomingData);
        }
    }
  
  // 处理断开连接
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // 给予蓝牙栈一些时间
    pServer->startAdvertising(); // 重新启动广告
    Serial.println("开始广告");
    oldDeviceConnected = deviceConnected;
  }
  
  // 处理新连接
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  
  delay(10); // 主循环延迟，避免CPU占用过高
}