#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <SPI.h>
#include <Ethernet.h>

Adafruit_BME280 bme;

// MAC inventato (va bene così)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Server di destinazione (esempio)
IPAddress serverIP(192, 168, 1, 100);
int serverPort = 5000;

EthernetClient client;

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("Inizializzazione BME280...");
  if (!bme.begin(0x76)) {
    Serial.println("Errore: BME280 non trovato!");
    while (1);
  }
  Serial.println("BME280 OK.");

  Serial.println("Inizializzo Ethernet (DHCP)...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Errore DHCP. Controlla il cavo o il router.");
    while (1);
  }

  Serial.print("IP ottenuto: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  float temp = bme.readTemperature();
  float hum  = bme.readHumidity();
  float pres = bme.readPressure() / 100.0;

  Serial.println("---- Letture BME280 ----");
  Serial.print("Temperatura: "); Serial.println(temp);
  Serial.print("Umidità: "); Serial.println(hum);
  Serial.print("Pressione: "); Serial.println(pres);

  // Connessione al server
  if (client.connect(serverIP, serverPort)) {
    client.print("T=");
    client.print(temp);
    client.print("C, H=");
    client.print(hum);
    client.print("%, P=");
    client.print(pres);
    client.println("hPa");

    client.stop();
    Serial.println("Dati inviati via Ethernet.");
  } else {
    Serial.println("Connessione al server fallita.");
  }

  delay(3000);
}