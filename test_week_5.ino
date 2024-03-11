#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "user1"
#define BLYNK_TEMPLATE_NAME "user1@server.wyns.it"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

int pin = 32;
#define DHTTYPE DHT11  // Type DHT-sensor
#define DHTPIN 4       // Pin waar de DHT11 is aangesloten
#define LEDPIN 5       // Pin voor LED (optioneel)
int AFWPIN = 34;       // Pin voor afwijking (analoog ingang)
int POTPIN = 35;       // Pin voor potentiometer
DHT dht(DHTPIN, DHTTYPE);
float afwijking = 0.0;  // Variabele voor temperatuurafwijking
float gewensteTemperatuur = 0.0;
float potTemp1 = 0.0;
float potTemp2 = 0.0;
float appTemp1 = 0.0;
float appTemp2 = 0.0;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "pepdJoxwdjsgtfCRSLNsverNPFVu9Ryf";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "embed";
char pass[] = "weareincontrol";

BlynkTimer timer;

void setup() {
  pinMode(pin, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(AFWPIN, INPUT);
  pinMode(POTPIN, INPUT);
  dht.begin();
  Serial.begin(9600);

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");

  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);

  timer.setInterval(1000L, updateTemperatures);

  inittemp();
}

void loop() {
  Blynk.run();
  timer.run();
}

void updateTemperatures() {
  float sensorWaarde = analogRead(AFWPIN);
  afwijking = map(sensorWaarde, 0, 4095, -5, 5);  // Schaal de waarde naar een bereik van -5 tot 5 graden Celsius
  float temperatuur = dht.readTemperature() + afwijking;
  float decimaal = int(temperatuur * 10) / 10.0;
  Blynk.virtualWrite(V5, decimaal);
  updateGewensteTemp();
  WijzigThermo(temperatuur);
}

void WijzigThermo(float temperatuur) {
  if (temperatuur < gewensteTemperatuur) {
    digitalWrite(LEDPIN, HIGH);  // Zet de thermostaat aan
    Blynk.virtualWrite(V0, 1023);
  } else {
    digitalWrite(LEDPIN, LOW);  // Zet de thermostaat uit
    Blynk.virtualWrite(V0, 0);
  }
}

void inittemp() {
  //Potentiometer initialiseren
  float potValue = analogRead(POTPIN);
  potTemp1 = map(potValue, 0, 4095, 0, 30);  // Schaal de waarde naar een bereik van 0 tot 30 graden Celsius
  // App temperatuur initialiseren
}


void updateGewensteTemp() {
  float potValue = analogRead(POTPIN);
  potTemp1 = map(potValue, 0, 4095, 0, 30);  // Schaal de waarde naar een bereik van 0 tot 30 graden Celsius
  if (potTemp1 != potTemp2) {
    
    potTemp2 = potTemp1;
    Blynk.virtualWrite(V7, potTemp2);
    gewensteTemperatuur = potTemp2;
  }
  if (appTemp2 != appTemp1){
    appTemp2 = appTemp1;
    Blynk.virtualWrite(V7, appTemp2);
    gewensteTemperatuur = appTemp2;
  }
}


BLYNK_WRITE(V6) {
  float value = param.asFloat(); // Get the float value from the virtual pin
  Serial.print("Float value received from Blynk app: ");
  Serial.println(value);
  
  appTemp1 = value; // Store the value in your variable
}
