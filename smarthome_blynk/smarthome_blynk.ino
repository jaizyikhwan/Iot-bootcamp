#define BLYNK_PRINT Serial //Untuk Komunikasi Serial Blynk
#include <ESP8266WiFi.h> //Untuk Library NodeMCU WiFi
#include <BlynkSimpleEsp8266.h> //Untuk Konek Blynk dan NodeMCU
#include <DHT.h> //Library DHT
#include <MQUnifiedsensor.h> //Library MQ
#include <Servo.h> //Library Servo

char auth[] = "2WzM1VhL7gouPkH1wX5VAURU0zCwjxWK"; //Token Blynk
char ssid[] = "Galaxy"; //SSID Wi-Fi
char pass[] = "12345678"; //Password Wi-Fi

#define DHTPIN D5 //Pin DHT di NodeMCU
#define DHTTYPE DHT11 //Jenis DHT yang digunakan
#define Board ("ESP8266") //Type Microcontroller
#define Pin (A0) //Pin MQ-135 di NodeMCU
#define Type ("MQ-135") //Type Sensor MQ-135
#define Voltage_Resolution (5) //Tegangan yang digunakan
#define ADC_Bit_Resolution (10) //Nilai Bit ADC NodeMCU
#define RatioMQ135CleanAir (9.6) //Parameter
#define Lampu D0 //Relay 1
#define Kipas D1 //Relay 2

DHT dht(DHTPIN, DHTTYPE); //Identifikasi DHT dan aktifkan fungsi DHT
Servo servo; ////Mengaktifkan fungsi Servo
MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type); //Identifikasi MQ dan Aktifkan fungsi MQ
BlynkTimer timer; //Blynk Timer

//Untuk melakukan pembacaan sensor
void sendSensor()
{

//Untuk melakukan pembacaan nilai LPG dan CO
  MQ135.update();
  MQ135.setA(1000.5); MQ135.setB(-2.186);
  float LPG = MQ135.readSensor();
  MQ135.setA(599.65); MQ135.setB(-2.244);
  float CO2 = MQ135.readSensor();

//Untuk melakukan pembacaan nilai Suhu dan Humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

//Mengirimkan nilai ke Blynk
  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
  Blynk.virtualWrite(V1, LPG);
  Blynk.virtualWrite(V4, CO2);
}

//Untuk fungsi menggerakkan Servo
BLYNK_WRITE(V5)
{
  //Kondisi apabila nilai yang diterima 1, 
  //maka servo bergerak ke 180 derajat, dan apabila nilai yang diterima 0, maka servo bergerak ke 0 derajat
  int pinValue = param.asInt();
  if (pinValue == 1) {
    servo.write(180);
  }
  if (pinValue == 0) {
    servo.write(0);
  }
}

void setup() {
  Serial.begin(115200); //Komunikasi Serial Baudrate 115200
  pinMode(Lampu, OUTPUT); //Set Relay 1 jadi Output
  pinMode(Kipas, OUTPUT); //Set Relay 2 jadi Output
  digitalWrite(Lampu, HIGH); //Set nilai Awal relay 2
  digitalWrite(Kipas, HIGH); //Set nilai Awal relay 2
  Blynk.begin(auth, ssid, pass); //Komunikasi ke Wifi dan Blynk
  servo.attach(D4); //Pin Servo di NodeMCU
  dht.begin(); //Memulai aktif DHT
  MQ135.setRegressionMethod(1); //Mengatur Metode Kalibrasi MQ
  MQ135.init(); //Memulai aktif MQ

//Kalkulasi untuk Kalibrasi MQ-135
  float calcR0 = 0;
  for(int i = 1; i<=10; i++)
  {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }

  MQ135.setR0(calcR0/10);

//Timer untuk interval pengiriman hasil pembacaan sensor ke Blynk
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run(); //Menjalankan Blynk
  timer.run(); //Mengaktifkan fungsi Timer
}
