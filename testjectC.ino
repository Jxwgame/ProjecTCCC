int ledPin = 13;
int buzzer = 12;
int ledRed = 11;
int analogPin = 0;
int val = 0;
const int sensorPin = 1; // กำหนดขา Analog ที่เชื่อมต่อกับ MCP9700
const float referenceVoltage = 5.0; // ค่าแรงดัน電源 (V) ที่ใช้งาน MCP9700
const float mcpOffsetVoltage = 0.5; // แรงดัน Offset ของ MCP9700 (V)
const float temperatureOffset = 0.5; // อุณหภูมิ Offset ของ MCP9700 (°C)
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(buzzer, INPUT);
  Serial.begin(9600);
  noTone(buzzer);
}

void loop() {
  val = analogRead(analogPin);
  Serial.print("val = ");
  Serial.println(val);
  int sensorValue = analogRead(sensorPin); // อ่านค่า Analog จาก MCP9700
  float voltage = (sensorValue / 1024.0) * referenceVoltage; // แปลงค่า Analog เป็นแรงดัน (V)
  float temperatureC = (voltage - mcpOffsetVoltage) * 100.0; // แปลงแรงดันเป็นอุณหภูมิ (°C)
  temperatureC -= temperatureOffset; // ปรับอุณหภูมิด้วย Offset
  Serial.print("อุณหภูมิ: ");
  Serial.print(temperatureC);
  Serial.println(" °C");
  if(temperatureC > 38){
    digitalWrite(ledRed, HIGH);
  }
  else {
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
  }
  else {
    digitalWrite(ledPin, HIGH);
    noTone(buzzer);
  }
  delay(1000);
}