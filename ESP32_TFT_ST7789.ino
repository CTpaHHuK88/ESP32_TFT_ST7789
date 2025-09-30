#include <TFT_eSPI.h>
#include <WiFi.h>  

const char* ssid = "";  
const char* password = "";  

TFT_eSPI tft = TFT_eSPI();

void start_led(const char* msg) {  // Добавлен const char*
  tft.fillScreen(TFT_BLACK);  // Очищаем экран перед выводом нового сообщения
  
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(msg);
  
  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.println("Resolution: 240x280");
  
  // Выводим статус WiFi
  tft.setCursor(10, 60);
  tft.print("WiFi: ");
  if (WiFi.status() == WL_CONNECTED) {
    tft.println("Connected");
    tft.setCursor(10, 80);
    tft.println(WiFi.localIP());  // Показываем IP адрес
  } else {
    tft.println("Disconnected");
  }
}

void setup() {
  Serial.begin(115200);  
  
  // Инициализация дисплея
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Включаем подсветку (один раз в setup)
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  
  start_led("Starting...");
  
  // Подключаемся к WiFi
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Максимум 20 попыток
    delay(1000);  
    Serial.print(".");
    attempts++;
    
    // Обновляем сообщение о статусе
    tft.fillRect(10, 10, 200, 20, TFT_BLACK);  // Частичное обновление
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("Connecting");
    for (int i = 0; i < attempts % 4; i++) {
      tft.print(".");
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    start_led("WiFi Connected!");
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    start_led("WiFi Failed!");
    Serial.println("\nWiFi connection failed!");
  }
}

void loop() {
  // Добавим индикацию работы
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    lastUpdate = millis();
    
    // Мигаем точкой в углу для индикации работы
    static bool dotState = false;
    dotState = !dotState;
    tft.fillCircle(230, 270, 3, dotState ? TFT_WHITE : TFT_BLACK);
    
    // Показываем время работы
    tft.fillRect(150, 60, 80, 10, TFT_BLACK);
    tft.setCursor(150, 60);
    tft.setTextSize(1);
    tft.print("Uptime: ");
    tft.print(millis() / 1000);
    tft.print("s");
  }
  
  delay(100);
}