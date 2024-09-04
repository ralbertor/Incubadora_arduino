#include <PubSubClient.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

#define DHT_PIN_TEMPERATURE 16
#define DHT_PIN_HUMIDITY 17
#define DHT_TYPE DHT22
#define BUZZER_PIN 0
const int LED_R = 2; // LED para la alarma de temperatura
const int LED_A = 4; // LED para la alarma de humedad
#define BUTTON_PIN_VERDE 12

// Variables globales para el debouncing
unsigned long lastDebounceTime_VERDE = 0;
const long debounceDelay = 50;       // El tiempo de rebote en milisegundos
int buttonState;                     // El estado actual del botón
int lastButtonState_VERDE = HIGH;          // El estado anterior del botón

DHT_Unified dhtTemperature(DHT_PIN_TEMPERATURE, DHT_TYPE);
DHT_Unified dhtHumidity(DHT_PIN_HUMIDITY, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* mqttServer = "mqtt-dashboard.com";
const int mqttPort = 1883;
const char* mqttClientId = "client-nuevo";
const char* mqttTopic = "testtopic/sensores1";
const char* alarmTopic = "testtopic/alarmas";
bool isTemperatureAlarmActive = false;
bool isHumidityAlarmActive = false;
bool alarmSilenced = false; // Variable para controlar si la alarma está en silencio

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(BUTTON_PIN_VERDE, INPUT_PULLUP); // Configura el pin del botón con pull-up interno
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  dhtTemperature.begin();
  dhtHumidity.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_A, OUTPUT);
  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.println("Setup completo, esperando pulsación del botón...");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  checkButton();  // función para verificar el botón
  publishSensorData();
}

void checkButton() {
  int reading = digitalRead(BUTTON_PIN_VERDE);

  if (reading != lastButtonState_VERDE) {
    // Solo reinicia el tiempo de debounce si el botón ha regresado a su estado inactivo
    if (lastButtonState_VERDE == LOW) {
      lastButtonState_VERDE = millis();
    }
  }

  if ((millis() - lastButtonState_VERDE) > debounceDelay) {
    // Si el botón fue presionado (transición de HIGH a LOW)
    if (reading == LOW && lastButtonState_VERDE == HIGH) {
      
      if (alarmSilenced == false){
        alarmSilenced = true;
        Serial.println("Alarma Silenciada.");
      }else {
        alarmSilenced = false;
        Serial.println("Alarma Activada.");

      }
    } else {
    }
    lastButtonState_VERDE = reading;
  }
}

void publishSensorData() {
  static unsigned long previousMillis = 0;
  const long interval = 3000; // Intervalo de 3 segundos
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sensors_event_t eventTemperature, eventHumidity;
    dhtTemperature.temperature().getEvent(&eventTemperature);
    dhtHumidity.humidity().getEvent(&eventHumidity);
    if (!isnan(eventTemperature.temperature) && !isnan(eventHumidity.relative_humidity)) {
      char payload[100];
      snprintf(payload, sizeof(payload), "{\"Temperatura\":%.2f, \"Humedad\":%.2f}", eventTemperature.temperature, eventHumidity.relative_humidity);
      lcdDisplayData(eventTemperature.temperature, eventHumidity.relative_humidity);
      client.publish(mqttTopic, payload);
      Serial.println("Dato publicado con éxito");
    }
  }
}

void lcdDisplayData(float temperature, float humidity) {
  static bool showHumidity = true; // Variable para alternar entre mostrar humedad y temperatura
  lcd.clear();
  lcd.setCursor(0, 0);

  if (showHumidity) {
    lcd.print("Humedad: ");
    lcd.print(humidity);
    lcd.print(" %");
  } else {
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" *C");
  }

  lcd.setCursor(0, 1);
  lcd.print("                "); // Limpiar la segunda línea

  showHumidity = !showHumidity; // Alternar el valor de la variable para el próximo ciclo
}

//Gestionar alarmas
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, alarmTopic) == 0) {
    int alarmType = payload[0] - '0'; // Convierte el caracter a entero (0 o 1)
  if (!alarmSilenced) {
    if (alarmType == 1) {
      isTemperatureAlarmActive = true;
      digitalWrite(LED_R, HIGH);
      tone(BUZZER_PIN, NOTE_G3);
    } else if (alarmType == 0) {
      isHumidityAlarmActive = true;
      digitalWrite(LED_A, HIGH);
      tone(BUZZER_PIN, NOTE_G5);
    }
  } else {
    // Si la alarma está silenciada, solo encender los LEDs
    if (alarmType == 1) {
      isTemperatureAlarmActive = true;
      digitalWrite(LED_R, HIGH);
    } else if (alarmType == 0) {
      isHumidityAlarmActive = true;
      digitalWrite(LED_A, HIGH);
    }
  }
    delay(1000); // Deja la alarma encendida por 1 segundo
    isTemperatureAlarmActive = false;
    isHumidityAlarmActive = false;
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_A, LOW);
    noTone(BUZZER_PIN);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(mqttClientId)) {
      client.subscribe(mqttTopic);
      client.subscribe(alarmTopic);
    } else {
      delay(5000);
    }
  }
}