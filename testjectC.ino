#include<WiFiEspAT.h>
#include<PubSubClient.h>
const char ssid[] = "Balaball";    // your network SSID (name)
const char pass[] = "1212312121";    // your network password (use for WPA, or use as key for WEP)
const char* mqtt_server = "broker.mqtt-dashboard.com";
const int ledPin = 13;
const int buzzer = 12;
const int ledRed = 11;
const int analogPin = 0;
int val = 0;  

const int analogPin2 = 1;
const float referenceVoltage = 5.0; // ค่าแรงดัน(V)
const float mcpOffsetVoltage = 0.5; // แรงดัน Offset
const float temperatureOffset = 0.5; // อุณหภูมิ Offset(°C)
// Emulate Serial1 on pins 6/7 if not present
#if defined(ARDUINO_ARCH_AVR) && !defined(HAVE_HWSERIAL1)
#include <SoftwareSerial.h>
SoftwareSerial Serial1(2, 3); // RX, TX
#define AT_BAUD_RATE 9600
#else
#define AT_BAUD_RATE 115200
#endif

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


void setup_wifi() {
  while (!Serial);

  Serial1.begin(AT_BAUD_RATE);
  WiFi.init(Serial1);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println();
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  WiFi.disconnect(); // to clear the way. not persistent

  WiFi.setPersistent(); // set the following WiFi connection as persistent

  WiFi.endAP(); // to disable default automatic start of persistent AP at startup

//  uncomment this lines for persistent static IP. set addresses valid for your network
//  IPAddress ip(192, 168, 1, 9);
//  IPAddress gw(192, 168, 1, 1);
//  IPAddress nm(255, 255, 255, 0);
//  WiFi.config(ip, gw, gw, nm);

  Serial.println();
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

//  use following lines if you want to connect with bssid
//  const byte bssid[] = {0x8A, 0x2F, 0xC3, 0xE9, 0x25, 0xC0};
//  int status = WiFi.begin(ssid, pass, bssid);

  int status = WiFi.begin(ssid, pass);

  if (status == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected to WiFi network.");
    printWifiStatus();
  } else {
    WiFi.disconnect(); // remove the WiFi connection
    Serial.println();
    Serial.println("Connection to WiFi network failed.");
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(13, HIGH);   
  } else {
    digitalWrite(13, LOW);  
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ClientID-RandomPanwit";

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("FireTemp", "arduino เชื่อมต่อกับ broker แล้ว");
      // ... and resubscribe
      client.subscribe("FireTemp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzer, INPUT);
  pinMode(ledRed, OUTPUT);
  noTone(buzzer);
}

void loop() {
  val = analogRead(analogPin);
    Serial.print("val = ");
    Serial.println(val);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(1000);
  unsigned long now = millis();
  
  int sensorValue = analogRead(analogPin2);
  float voltage = (sensorValue / 1024.0) * referenceVoltage; // แปลงค่า Analog เป็นแรงดัน (V)
  float temperatureC = (voltage - mcpOffsetVoltage) * 100.0;
  temperatureC -= temperatureOffset;
  Serial.print("อุณหภูมิ: ");
  Serial.print(temperatureC);
  Serial.println(" °C");
  if(temperatureC > 38){ //ใช้สำหรับเทสในอุณหภูมิห้อง ถ้าหากนำไปใช้จริงต้องปรับอุณหภูมิให้สูงขึ้น
    digitalWrite(ledRed, HIGH);
    if (now - lastMsg > 5000) {
      lastMsg = now;
      ++value;
      snprintf (msg, MSG_BUFFER_SIZE, "อุณหภูมิสูงผิดปกติจะทำการหยุดจ่ายไฟ", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("FireTemp", msg);
    }
  } else{
    digitalWrite(ledRed, LOW);
  }

  if (val > 300) {
    digitalWrite(ledPin, LOW);
    tone(buzzer, 800);
    delay(500);
    tone(buzzer, 2000);
    delay(500);
    tone(buzzer, 800);
    delay(500);
    tone(buzzer, 2000);
    delay(500);
    tone(buzzer, 800);
    delay(500);
    tone(buzzer, 2000);
    delay(500);
    if (now - lastMsg > 5000) {
      lastMsg = now;
      ++value;
      snprintf (msg, MSG_BUFFER_SIZE, "ไฟไหม้แล้วววววว #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("FireTemp", msg);
    }
  }
  else {
    digitalWrite(ledPin, HIGH);
    noTone(buzzer);
  }
  delay(1000);


}

void printWifiStatus() {

  // print the SSID of the network you're attached to:
  char ssid[33];
  WiFi.SSID(ssid);
  Serial.print("SSID: ");
  Serial.println(ssid);

  // print the BSSID of the network you're attached to:
  uint8_t bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  printMacAddress(mac);

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
