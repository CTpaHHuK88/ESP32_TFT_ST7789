### 1. Аппаратное подключение (SPI)

Большинство дисплеев ST7789v3 имеют разрешение **240x240** или **240x320** пикселей. Подключение осуществляется по **4-проводному SPI интерфейсу**.

#### Необходимые компоненты:
*   Плата ESP32 (любая, например, ESP32 DevKit V1)
*   Дисплей ST7789 (например, 1.3", 1.54", 1.69" с разрешением 240x240)
*   Провода "папа-папа"

#### Распиновка дисплея (типичная для модулей 240x240):

| Обозначение на дисплее | Назначение | Пины ESP32 (можно менять!) |
| :-------------------- | :----------------------------- | :-------------------------- |
| **VCC** | Питание (+3.3V) | **3.3V** |
| **GND** | Земля | **GND** |
| **SCL** | Тактовый сигнал SPI | **GPIO 18** |
| **SDA** | Данные (MOSI) | **GPIO 23** |
| **RES** | Сброс (Reset) | **GPIO 4** |
| **DC** | Выбор данных/команды (Data/Command) | **GPIO 2** |
| **CS** | Выбор чипа (Chip Select) | **GPIO 5** |
| **BL** | Подсветка (Backlight) | **GPIO 32** (через резистор 100-220 Ом) |

**Схема подключения:**
```
ESP32          ST7789 TFT
3.3V    ---->   VCC
GND     ---->   GND
GPIO 18 ---->   SCL
GPIO 23 ---->   SDA
GPIO 4  ---->   RES
GPIO 2  ---->   DC
GPIO 5  ---->   CS
GPIO 32 ---->   BL (через резистор)
```

**Важно:**
*   Подключайте питание **ТОЛЬКО к 3.3V** ESP32. 5V убьет дисплей!
*   Если пины ESP32 заняты, вы можете использовать другие свободные GPIO, просто не забудьте указать их в коде.
*   Подсветку (BL) можно подключить напрямую к 3.3V, если вы хотите, чтобы она горела всегда. Но лучше управлять через GPIO, чтобы иметь возможность ее отключать.

---

### 2. Программная часть (Библиотеки и код)

#### Установка библиотек в Arduino IDE

Есть две популярные библиотеки. Рекомендую использовать **TFT_eSPI**.

1.  **Установите библиотеку TFT_eSPI:**
    *   В Arduino IDE: **Скетч** -> **Подключить библиотеку** -> **Управлять библиотеками...**
    *   В поиске введите "TFT_eSPI".
    *   Найдите библиотеку от **Bodmer** и установите ее.

2.  **Настройте библиотеку TFT_eSPI:**
    Это самый важный шаг! Библиотека требует настройки под ваш конкретный дисплей.
    *   В папке с документами Arduino найдите папку `libraries/TFT_eSPI`.
    *   Внутри нее найдите файл `User_Setup.h` и откройте его в текстовом редакторе.
    *   **Закомментируйте** все строки с `#define` для других дисплеев (например, `ILI9341_DRIVER`).
    *   **Раскомментируйте** (уберите `//`) строку для вашего драйвера и укажите правильные пины:
    
    ```cpp
    #define ST7789_DRIVER      // Раскомментируйте эту строку
    
    // Определите разрешение вашего дисплея (раскомментируйте одну из строк)
    #define TFT_WIDTH  240
    #define TFT_HEIGHT 240
    // #define TFT_HEIGHT 320 // Если у вас дисплей 240x320
    
    // --- УКАЖИТЕ ВАШИ ПИНЫ ESP32 ---
    #define TFT_CS   5   // Chip select
    #define TFT_DC   2   // Data/Command
    #define TFT_RST  4   // Reset
    //#define TFT_RST  -1  // Если пин Reset не подключен, используйте -1
    
    #define TFT_MOSI 23  // Data out (SDA)
    #define TFT_SCLK 18  // Clock out (SCL)
    
    // Для ESP32 также рекомендуется определить порт SPI
    #define USE_HSPI_PORT
    ```
    *   Сохраните файл и закройте его.

#### Пример кода (Тестовый скетч)

Создайте новый скетч в Arduino IDE и скопируйте в него следующий код:

```cpp
#include <TFT_eSPI.h> // Подключаем графическую библиотеку

TFT_eSPI tft = TFT_eSPI(); // Создаем объект "tft"

void setup() {
  Serial.begin(115200);
  
  // Инициализация дисплея
  tft.init();
  tft.setRotation(2); // Поворот дисплея (0, 1, 2, 3) - подберите под свое крепление
  tft.fillScreen(TFT_BLACK); // Заливаем экран черным цветом

  // Включаем подсветку (если она подключена к GPIO)
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);

  // Тест вывода текста и графики
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Цвет текста и фона
  tft.setTextSize(2); // Размер текста
  tft.drawString("Hello, ESP32!", 10, 10, 2); // Вывод текста (x, y, шрифт)
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("ST7789 TFT", 10, 40, 2);

  // Рисуем разноцветные фигуры
  tft.drawRect(10, 70, 100, 50, TFT_RED);    // Прямоугольник (x, y, w, h, color)
  tft.fillCircle(180, 95, 25, TFT_BLUE);     // Закрашенный круг (x, y, radius, color)
  tft.drawLine(10, 150, 230, 150, TFT_YELLOW); // Линия (x1, y1, x2, y2, color)
}

void loop() {
  // Меняем цвет пикселя в случайном месте
  int x = random(tft.width());
  int y = random(tft.height());
  tft.drawPixel(x, y, random(0xFFFF)); // Случайный цвет
  
  delay(10);
}
```

---

### 3. Решение распространенных проблем

1.  **Белый экран или нет изображения:**
    *   Проверьте, что пины `DC` и `RES` подключены правильно.
    *   Убедитесь, что в `User_Setup.h` раскомментирован `ST7789_DRIVER`.
    *   Попробуйте добавить небольшую задержку `delay(100);` после `tft.init();`.

2.  **Изображение перевернутое или сдвинутое:**
    *   Поэкспериментируйте с `tft.setRotation(0);` `tft.setRotation(1);` и т.д.
    *   Убедитесь, что `TFT_WIDTH` и `TFT_HEIGHT` заданы правильно.

3.  **Дисплей не определяется (все время черный):**
    *   Дважды проверьте пайку/контакты. Чаще всего проблема в этом.
    *   Убедитесь, что питание 3.3V стабильное. ESP32 может не давать достаточный ток для дисплея при питании от USB. Попробуйте внешний источник питания.
    *   Проверьте, не используются ли выбранные пины ESP32 для чего-то еще (например, загрузки).

4.  **Ошибка компиляции:**
    *   Убедитесь, что библиотека `TFT_eSPI` установлена правильно.
    *   Закомментируйте в `User_Setup.h` все ненужные драйверы.





### Анализ вашего файла настроек:

**Правильные настройки:**
- `#define ST7789_DRIVER` - активирован правильный драйвер ✅
- `#define TFT_WIDTH 240` - правильная ширина ✅
- `#define TFT_HEIGHT 320` - правильная высота для дисплея 240x320 ✅
- `#define USE_HSPI_PORT` - использование HSPI порта на ESP32 ✅
- `#define SPI_FREQUENCY 27000000` - хорошая частота для ST7789 ✅

**Потенциальные проблемы и настройки, которые нужно проверить:**

### 1. Настройки пинов ESP32 (Секция 2)

В вашем файле настроены пины по умолчанию для ESP32:

```cpp
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin
```

**Проверьте:** Совпадают ли эти пины с вашим реальным подключением? Если вы подключали по схеме из предыдущего ответа (GPIO 23, 18, 5, 2, 4), то нужно исправить `TFT_CS` с 15 на 5.

### 2. Подсветка (Backlight)

Раскомментируйте и настройте управление подсветкой:

```cpp
#define TFT_BL   32            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)
```

### 3. Цветовой порядок (RGB/BGR)

Если цвета отображаются неправильно (синий вместо красного и т.д.), раскомментируйте одну из строк:

```cpp
// #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
// #define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red
```

### Исправленная версия секции с пинами для ESP32:

```cpp
// ###### EDIT THE PIN NUMBERS IN THE LINES FOLLOWING TO SUIT YOUR ESP32 SETUP ######

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5  // Chip select control pin - ИЗМЕНИТЕ ЕСЛИ НУЖНО!
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

#define TFT_BL   32  // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light
```

### Тестовый скетч для проверки:

После настройки `User_Setup.h` используйте этот код для тестирования:

```cpp
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  
  // Инициализация дисплея
  tft.init();
  tft.setRotation(0); // Попробуйте 0, 1, 2, 3 если изображение перевернуто
  
  // Включение подсветки
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ST7789 Test");
  tft.println("ESP32 Connected!");
  
  // Тест цветов
  tft.fillRect(10, 60, 50, 30, TFT_RED);
  tft.fillRect(70, 60, 50, 30, TFT_GREEN);
  tft.fillRect(130, 60, 50, 30, TFT_BLUE);
}

void loop() {
  delay(1000);
}
```

### Если дисплей не работает:

1. **Проверьте питание:** 3.3V и GND
2. **Перепроверьте пины:** Убедитесь, что физическое подключение совпадает с настройками в коде
3. **Попробуйте разные rotation:** `tft.setRotation(0)`, `tft.setRotation(1)`, etc.
4. **Если цвета неправильные:** Раскомментируйте строку с `TFT_RGB_ORDER TFT_BGR`



Для дисплея **1.69 дюйма с контроллером ST7789** наиболее вероятное разрешение — **240x280 пикселей**.

### Правильные настройки для User_Setup.h:

```cpp
#define ST7789_DRIVER

// Разрешение для 1.69" ST7789
#define TFT_WIDTH  240
#define TFT_HEIGHT 280

// Настройки пинов ESP32 (измените под ваше подключение)
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5   // Chip select
#define TFT_DC   2   // Data/Command
#define TFT_RST  4   // Reset

// Подсветка (если управляется через GPIO)
#define TFT_BL   32
#define TFT_BACKLIGHT_ON HIGH

// Дополнительные настройки
#define SPI_FREQUENCY 27000000
#define USE_HSPI_PORT
```

### Почему именно 240x280?

1.69-дюймовые дисплеи ST7789 обычно имеют одно из этих разрешений:
- **240x280** (самое распространенное для 1.69")
- 240x240 (редко для 1.69")
- 170x320 (вертикальное)

### Как точно определить разрешение?

**Способ 1: Посмотрите на маркировку дисплея или документацию**
- Обычно указано в спецификации: "1.69 inch 240*280"

**Способ 2: Эмпирическая проверка**
Загрузите тестовый скетч с разными настройками:

```cpp
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void testResolution(int width, int height, String label) {
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  
  // Рисуем рамку по краям
  tft.drawRect(0, 0, width, height, TFT_RED);
  tft.drawRect(1, 1, width-2, height-2, TFT_RED);
  
  // Выводим информацию
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 10);
  tft.print("Testing: ");
  tft.print(label);
  tft.setCursor(10, 25);
  tft.print("Size: ");
  tft.print(width);
  tft.print("x");
  tft.print(height);
  
  delay(3000);
}

void setup() {
  Serial.begin(115200);
  
  // Тестируем возможные разрешения
  testResolution(240, 280, "240x280 (most common)");
  testResolution(240, 240, "240x240");
  testResolution(170, 320, "170x320");
  testResolution(240, 320, "240x320");
}

void loop() {}
```

### Если не уверены в размере:

1. **Сначала попробуйте 240x280** — это наиболее вероятный вариант
2. Если изображение обрезано или есть черные полосы — пробуйте другие варианты
3. Ориентируйтесь на пропорции дисплея: 1.69" обычно ближе к квадратной форме

### Тестовый скетч для окончательной проверки:

```cpp
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  
  tft.init();
  tft.setRotation(0); // Пробуйте 0, 1, 2, 3 для правильной ориентации
  tft.fillScreen(TFT_BLACK);
  
  // Включаем подсветку
  pinMode(32, OUTPUT);
  digitalWrite(32, HIGH);
  
  // Рисуем тестовую сетку
  for (int i = 0; i < 240; i += 20) {
    tft.drawLine(i, 0, i, 280, TFT_BLUE);
  }
  for (int i = 0; i < 280; i += 20) {
    tft.drawLine(0, i, 240, i, TFT_BLUE);
  }
  
  // Выводим информацию
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("1.69\" ST7789");
  tft.setTextSize(1);
  tft.println("Resolution: 240x280");
  
  // Заливаем цветами углы
  tft.fillCircle(20, 20, 15, TFT_RED);
  tft.fillCircle(220, 20, 15, TFT_GREEN);
  tft.fillCircle(20, 260, 15, TFT_BLUE);
  tft.fillCircle(220, 260, 15, TFT_YELLOW);
}

void loop() {}
```

**Начните с 240x280** — в 90% случаев это правильное разрешение для 1.69-дюймового ST7789!