#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  
  // Инициализация дисплея
  tft.init();
  tft.setRotation(1); // Попробуйте 0, 1, 2, 3 если изображение перевернуто
  
  // Включение подсветки
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(40, 40);
  tft.println("ST7789 Test");
  tft.setCursor(50, 50);
  tft.println("ESP32 Connected!");
  
  // Тест цветов
   tft.setCursor(60, 60);
  tft.fillRect(10, 60, 50, 30, TFT_RED);
  tft.fillRect(70, 60, 50, 30, TFT_GREEN);
  tft.fillRect(130, 60, 50, 30, TFT_BLUE);
}

void loop() {
  delay(1000);
}