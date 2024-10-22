#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define LED 2
#define BUZZER 27

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "test";
const char* password = "bhavs2022";
float sellprice = 21800;
String json_array;
String webhook_res;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  lcd.init();
  lcd.backlight();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("CONNECTING TO WiFi");
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi.");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    lcd.print(".");
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(200);
  }

  lcd.clear();
  lcd.print("Connected WiFi:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.print("Connected to WiFi : ");
  Serial.print(WiFi.localIP());
}

String GET_Request(const char* server) {
  HTTPClient http;
  http.begin(server);
  int httpResponseCode = http.GET();

  String payload = "{}";
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  
  return payload;
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Sellprice set to");
  lcd.setCursor(0, 1);
  lcd.print(sellprice);
  delay(2000);
  
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.print("Fetching Price");
    String server = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

    json_array = GET_Request(server.c_str());
    Serial.println(json_array);
    JSONVar response = JSON.parse(json_array);

    if (JSON.typeof(response) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    Serial.print("BTC: ");
    Serial.println(response["bitcoin"]["usd"]);
    String price = JSON.stringify(response["bitcoin"]["usd"]);
    
    if (price.toFloat() > sellprice) {
      lcd.setCursor(0, 0);
      lcd.print("BTC Price is high");
      lcd.setCursor(0, 1);
      lcd.print("than sell price");

      String webhookendpoint = "https://maker.ifttt.com/trigger/price_high/with/key/cXVaerT8tP7laLxphRfNfZ?value1=" + price + "&value2=" + sellprice;
      webhook_res = GET_Request(webhookendpoint.c_str());
      Serial.println(webhook_res);
      Serial.print("Webhook Notification Sent!");
      
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BTC Live Price:");
    lcd.setCursor(0, 1);
    lcd.print("$");
    lcd.print(response["bitcoin"]["usd"]);
    delay(5000);
  }
}
