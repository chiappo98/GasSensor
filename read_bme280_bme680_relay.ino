#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BME680.h>

Adafruit_BME280 bme;  // BME_ADDR 0x77
Adafruit_BME680 bme1;  // BME_ADDR 0x76

const int relayPin = 7;   // compatibile con MKR

const int hum_limit = 45; //change  
const int pres_diff_limit = -10; //change

void setup() {
  Serial.begin(9600);
  delay(2000);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);   // relè OFF (logica invertita)

  // --- I2C su MKR WiFi 1010 ---
  Wire.begin();  // SDA=11, SCL=12 automaticamente

if (!bme.begin(0x77)) {
  Serial.println("Errore: BME280 non trovato!");
  while (1);
}

if (!bme1.begin(0x76)) {
  Serial.println("Errore: BME680 non trovato!");
  while (1);
}

  bme1.setGasHeater(320, 150); // 320°C per 150 ms
  Serial.println("Sistema MKR pronto.");
}

float compute_AH(float RH, float T) {
  float num = 6.112*std::exp( (17.67*T) / (T + 243.5) ) * RH *18.02;
  float den = (273.15 + T) * 100 * 0.08314;
  return num/den;
}

float compute_ppm(float AH, float T, float pres) {
  float vap_press = AH * 8.314 * T / 18.015;
  return 1e6 * vap_press / (pres * 100);
}

void loop() {
  // --- Lettura sensore interno ---
  float temp = bme.readTemperature();
  float hum  = bme.readHumidity();
  float pres = bme.readPressure() / 100.0F;
  float abs_hum = compute_AH(hum, temp);
  float hum_ppm = compute_ppm(abs_hum, temp, pres) * 10;  // why x10 ??

  // --- Lettura sensore esterno ---
  float temp_ext = bme1.readTemperature();
  float hum_ext  = bme1.readHumidity();
  float pres_ext = bme1.readPressure() / 100.0F;
  float gasQ_ext = bme1.gas_resistance;

  if ((hum_ext > hum_limit) || (pres-pres_ext < pres_diff_limit)) {
    digitalWrite(relayPin, HIGH);
    Serial.println("Relè ON"); // to be removed
  }
  if ((hum_ext <= hum_limit) && (pres-pres_ext >= pres_diff_limit)) digitalWrite(relayPin, LOW);
  

  // --- Invio dati al PC ---
  Serial.print("T:"); Serial.print(temp);
  Serial.print("C|RH:"); Serial.print(hum);
  Serial.print("%|AH:"); Serial.print(abs_hum);
  Serial.print("g/m3|AH:"); Serial.print(hum_ppm);
  Serial.print("ppm|P:"); Serial.print(pres);
  Serial.print("hPa|T_ext:"); Serial.print(temp_ext);
  Serial.print("C|RH_ext:"); Serial.print(hum_ext);
  Serial.print("%|P_ext:"); Serial.print(pres_ext);
  Serial.println("hPa");
  //Serial.print("Gas: "); Serial.print(bme1.gas_resistance / 1000.0); Serial.println(" kΩ");
  delay(5000);
}
