#include <ESP8266WiFi.h> //Library untuk konek ke Wi-Fi
#include <ThingerESP8266.h> //Library untuk terhubung ke Thinger
#include <NewPing.h> //Library untuk HC-SR04
#include <DHT.h> //Library untuk DHT

#define USERNAME "jaizyikhwan" //Username Thinger IO
#define DEVICE_ID "MCUNR" //Device ID Thinger IO
#define DEVICE_CREDENTIAL "!A5@11ichD+3tgTX" //Device Credential Thinger IO
#define SSID "4u" //SSID Wi-Fi
#define SSID_PASSWORD "password123" //Password Wi-Fi

#define TRIGGER_PIN D5 //Pin Trigger HC-SR04 pada NodeMCU
#define ECHO_PIN D6 //Pin Echo HC-SR04 pada NodeMCU
#define MAX_DISTANCE 250 //Maksimum Pembacaan Jarak (cm)

#define DHTPIN D4 //Pin DHT pada NodeMCU
#define DHTTYPE DHT11 //Jenis DHT yang digunakan
#define pinYL A0 //pin Analog YL-69
#define WaterPump D0 //Pin Relay 1
#define WaterPump2 D1 //Pin Relay 2
#define Lampu D2 //Pin LED

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); //Aktifkan Fungsi Thinger IO
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); //Aktifkan Fungsi HCSR-04
DHT dht(DHTPIN, DHTTYPE); //Aktifkan Fungsi DHT

int outputValue; //Variabel untuk Output YL-69

void setup() {
  // put your setup code here, to run once:
pinMode(Lampu, OUTPUT); //Set LED sebagai Output
pinMode(WaterPump, OUTPUT); //Set Relay 1 sebagai Output
pinMode(WaterPump2, OUTPUT); //Set Relay 2 sebagai Output
digitalWrite(Lampu, LOW); //Set nilai LED = 0
digitalWrite(WaterPump, HIGH); //Set nilai Relay 1 = 1
digitalWrite(WaterPump2, HIGH); //Set nilai Relay 2 = 1
dht.begin(); //Memulai fungsi DHT
thing.add_wifi(SSID, SSID_PASSWORD); //Mencoba terhubung ke Wi- Fi dan Thinger IO

//Fungsi untuk Kontrol Lampu dengan Topik Lampu
thing["Lampu"] << [](pson& in){
if(in.is_empty()){
in = (bool) digitalRead(Lampu);
}
else{
digitalWrite(Lampu, in ? HIGH : LOW);
}
};

//Fungsi untuk Kontrol Relay 1 dengan Topik Pompa Air
thing["Pompa Air"] << [](pson& in){
if(in.is_empty()){
in = (bool) digitalRead(WaterPump);
}
else{
digitalWrite(WaterPump, in ? HIGH : LOW);
}
};

//Fungsi untuk Kontrol Relay 2 dengan Topik Watering
thing["Watering"] << [](pson& in){
if(in.is_empty()){
in = (bool) digitalRead(WaterPump2);
}
else{
digitalWrite(WaterPump2, in ? HIGH : LOW);
}
};

//Mengirim Pembacaan Sensor DHT 11 ke Thinger IO
thing["DHT11"] >> [](pson& out){
out["Humidity"] = dht.readHumidity();
out["Suhu"] = dht.readTemperature();
};

//Mengirim Pembacaan Sensor HCSR-04 ke Thinger IO
thing["HCSR04"] >> [](pson& out){
out["Jarak"] = sonar.ping_cm();
};

//Mengirim Pembacaan Sensor YL-69 ke Thinger IO
thing["Soil_Moisture"] >> [](pson& out){
outputValue = analogRead(pinYL);
outputValue = map(outputValue, 1024, 250, 0, 100);
out["Moisture"] = outputValue;
};

}

void loop() {

//Menjalankan fungsi Thinger IO
thing.handle();
}
