#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Print.h>

// 定义 AP 模式的网络信息
const char* ssid = "ESP32_TEST";
const char* password = "12345678";

// 创建 TCP 服务器，端口为 8888
WiFiServer server(8888);

void setup() 
{
  Serial.begin(115200);

  // 开启 AP 模式
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // 启动服务器
  server.begin();
  Serial.println("Server started");
}

void loop() 
{
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        // 读取所有可用字节
        while (client.available()) {
          byte data = client.read();
          
          // 输出十六进制格式（两位，含前导零）
          if (data < 16) Serial.print("0");  // 补零
          Serial.print(data, HEX);
          Serial.print(" ");  // 添加空格分隔符
        }
        Serial.println();  // 换行
        // 向客户端发送响应
        client.println("Server received");
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}    