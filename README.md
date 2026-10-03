# GTag6

Arduino-библиотека для управления электронным ценником **SES-imagotag G-TAG 6** с разрешением **256×128** на микроконтроллерах **ESP32**.

Библиотека построена поверх `Adafruit_GFX`, поэтому поддерживает стандартные функции рисования, текст, шрифты, битмапы и поворот экрана. Дополнительно в библиотеке есть готовые таблицы, графики, индикаторы, иконки и элементы интерфейса.

## Возможности

- Дисплей G-TAG 6 — 256×128 пикселей
- 1-битный framebuffer размером 4096 байт
- Совместимость с `Adafruit_GFX`
- Текст, линии, прямоугольники, окружности, треугольники и другие графические примитивы
- Поддержка шрифтов `Adafruit_GFX`
- Поворот экрана
- Инверсия изображения
- Полное и обычное обновление дисплея
- Автоматическое периодическое обновление
- Таблицы с текстом и разными ширинами колонок
- Готовые UI-виджеты
- Полосы прогресса и стрелочные индикаторы
- Иконки батареи, Wi-Fi и погоды
- Семисегментные крупные цифры
- Перенос текста
- Графики истории значений до 256 точек

---

## Совместимость

| Параметр | Значение |
|---|---|
| Дисплей | SES-imagotag G-TAG 6 |
| Разрешение | 256×128 |
| Цвет | Монохромный, 1 бит/пиксель |
| Framebuffer | 4096 байт |
| Платформа | ESP32 |
| Arduino Core | 2.x / 3.x |
| Графический API | Adafruit_GFX |
| Зависимость | Adafruit GFX Library |
| Версия библиотеки | 1.2.0 |

---

# Подключение G-TAG 6

В библиотеке используется следующая распиновка тестовых площадок G-TAG 6:

| Пин G-TAG 6 | Назначение | ESP32 |
|:---:|---|:---:|
| **S1** | DisplayCLK (~29 кГц) | **GPIO 17** |
| **8** | RESET / RES | **GPIO 16** |
| **7** | CS | **GPIO 5** |
| **S2** | CLK | **GPIO 18** |
| **S10** | DIO | **GPIO 23** |
| **GND** | Земля | **GND** |

### Схема

```text
G-TAG 6                    ESP32

S1   (DisplayCLK)  ─────── GPIO 17
8    (RESET)       ─────── GPIO 16
7    (CS)          ─────── GPIO 5
S2   (CLK)         ─────── GPIO 18
S10  (DIO)         ─────── GPIO 23
GND                ─────── GND
```

> **Важно:** DisplayCLK на S1 должен работать с частотой около **29 кГц**. Библиотека запускает этот сигнал автоматически при `begin()`.

---

# Установка

Библиотека требует:

- Arduino IDE
- ESP32 Arduino Core
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)

### Через Arduino IDE

Установите `Adafruit GFX Library` через:

```text
Инструменты → Управление библиотеками
```

После этого скопируйте папку `GTag6` в:

```text
Documents/Arduino/libraries/
```

Структура библиотеки:

```text
GTag6/
├── library.properties
├── keywords.txt
├── README.md
├── src/
│   ├── GTag6.h
│   ├── GTag6.cpp
│   ├── GTag6Widgets.h
│   └── GTag6Widgets.cpp
└── examples/
    ├── HelloWorld/
    ├── Dashboard/
    └── Table/
```

---

# Быстрый старт

```cpp
#include <GTag6.h>

//          DIO CLK CS RESET DISCLK
GTag6 display(23, 18, 5, 16, 17);

void setup()
{
    display.begin();

    display.setTextColor(GTAG6_BLACK);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.print("Hello G-TAG 6");

    display.display();
}

void loop()
{
}
```

---

# Инициализация

```cpp
display.begin();
```

По умолчанию DisplayCLK запускается с частотой:

```text
29000 Гц
```

Можно указать собственную частоту:

```cpp
display.begin(29000);
```

---

# Основные методы GTag6

| Метод | Описание |
|---|---|
| `begin(hz)` | Настройка пинов, запуск DisplayCLK и инициализация дисплея |
| `display()` | Передача текущего framebuffer на дисплей |
| `refresh()` | Полный RESET + инициализация + передача framebuffer |
| `clearDisplay()` | Очистка framebuffer |
| `fillScreen(color)` | Заполнение всего framebuffer указанным цветом |
| `setAutoRefresh(ms)` | Установка периода автоматического `refresh()` |
| `tick()` | Обслуживание автоматического обновления |
| `displayOn()` | Команда включения дисплея |
| `displayOff()` | Команда выключения дисплея |
| `invertDisplay(bool)` | Инверсия изображения при отправке |
| `invertBuffer()` | Физически инвертировать framebuffer |
| `invertRect(x,y,w,h)` | Инвертировать область framebuffer |
| `getPixel(x,y)` | Получить состояние пикселя |
| `drawTable(...)` | Быстро нарисовать сетку таблицы |
| `getBuffer()` | Получить прямой доступ к framebuffer |

---

# Цвета

Используются две константы:

```cpp
GTAG6_BLACK
GTAG6_WHITE
```

Значения:

```text
GTAG6_BLACK = 1
GTAG6_WHITE = 0
```

Пример:

```cpp
display.fillScreen(GTAG6_WHITE);
display.drawRect(10, 10, 100, 50, GTAG6_BLACK);
```

---

# Adafruit_GFX

`GTag6` наследуется от `Adafruit_GFX`, поэтому доступны стандартные методы библиотеки.

Например:

```cpp
display.drawPixel(...);
display.drawLine(...);
display.drawRect(...);
display.fillRect(...);
display.drawCircle(...);
display.fillCircle(...);
display.drawTriangle(...);
display.drawRoundRect(...);
display.fillRoundRect(...);
display.drawBitmap(...);

display.setCursor(...);
display.setTextColor(...);
display.setTextSize(...);
display.setFont(...);
display.print(...);
display.println(...);
display.getTextBounds(...);

display.setRotation(...);
```

Это позволяет использовать обычный код `Adafruit_GFX` практически без изменений.

---

# Автоматическое обновление

Для стабильной работы G-TAG 6 можно включить периодический полный `refresh()`:

```cpp
display.setAutoRefresh(5000);
```

Это означает обновление каждые 5 секунд.

В `loop()` необходимо регулярно вызывать:

```cpp
display.tick();
```

Пример:

```cpp
void loop()
{
    display.tick();
}
```

Автоматическое обновление можно отключить:

```cpp
display.setAutoRefresh(0);
```

---

# `display()` и `refresh()`

Библиотека предоставляет два варианта обновления:

### `display()`

Передаёт текущий framebuffer:

```cpp
display.display();
```

### `refresh()`

Выполняет полный цикл:

```text
RESET
 ↓
Инициализация
 ↓
Передача framebuffer
```

Используется:

```cpp
display.refresh();
```

Для G-TAG 6 рекомендуется периодически использовать именно `refresh()`.

---

# Инверсия

Инверсия только при отправке:

```cpp
display.invertDisplay(true);
```

Выключить:

```cpp
display.invertDisplay(false);
```

При этом сам framebuffer не изменяется.

Для физической инверсии всего framebuffer:

```cpp
display.invertBuffer();
```

Инверсия отдельной области:

```cpp
display.invertRect(10, 10, 100, 30);
```

---

# Работа с framebuffer

Размер framebuffer:

```text
256 × 128 / 8 = 4096 байт
```

Получить указатель:

```cpp
uint8_t *buffer = display.getBuffer();
```

Формат:

- 32 байта на одну строку
- MSB соответствует левому пикселю
- `1` — чёрный
- `0` — белый

---

# GTag6Table

`GTag6Table` предназначен для создания таблиц с текстом.

Можно использовать:

- одинаковую ширину колонок;
- индивидуальную ширину каждой колонки;
- разные цвета текста;
- выравнивание;
- инверсию ячеек.

## Пример

```cpp
#include <GTag6.h>

GTag6 display(23, 18, 5, 16, 17);

const int16_t COL_W[] = {96, 78, 78};

GTag6Table table(
    display,
    0,      // x
    3,      // y
    4,      // rows
    COL_W,  // ширины колонок
    3,      // columns
    30      // высота строки
);
```

Нарисовать таблицу:

```cpp
table.draw();
```

Заполнить ячейку:

```cpp
table.fillCell(0, 0, GTAG6_BLACK);
```

Вывести текст:

```cpp
table.setCell(
    0,
    0,
    "Item",
    GTAG6_ALIGN_LEFT,
    2,
    GTAG6_WHITE
);
```

Инвертировать ячейку:

```cpp
table.invertCell(3, 2);
```

---

## Выравнивание текста

Доступны:

```cpp
GTAG6_ALIGN_LEFT
GTAG6_ALIGN_CENTER
GTAG6_ALIGN_RIGHT
```

Пример:

```cpp
table.setCell(0, 0, "Left", GTAG6_ALIGN_LEFT);
table.setCell(0, 1, "Center", GTAG6_ALIGN_CENTER);
table.setCell(0, 2, "Right", GTAG6_ALIGN_RIGHT);
```

Текст вертикально центрируется внутри ячейки.

Если текст не помещается по ширине, лишняя часть обрезается.

---

# GTag6UI

`GTag6UI` содержит готовые графические элементы для создания интерфейсов.

Все функции работают с объектами `Adafruit_GFX`, включая `GTag6`.

---

## Полоса прогресса

```cpp
GTag6UI::progressBar(
    display,
    10, 10,
    100, 12,
    75
);
```

`percent` ограничивается диапазоном `0..100`.

---

## Стрелочный индикатор

```cpp
GTag6UI::gauge(
    display,
    128, 80,
    35,
    75,
    0,
    100,
    10
);
```

Параметры:

```text
cx, cy  — центр
r       — радиус
value   — текущее значение
min     — минимум
max     — максимум
ticks   — количество делений
```

---

## Дуга

```cpp
GTag6UI::arc(
    display,
    128, 64,
    30,
    2,
    -90,
    90
);
```

Углы:

```text
  0°  — вверх
 90°  — вправо
180°  — вниз
-90°  — влево
```

Угол увеличивается по часовой стрелке.

---

## Толстая линия

```cpp
GTag6UI::thickLine(
    display,
    10, 10,
    200, 50,
    3
);
```

---

## Пунктирная линия

```cpp
GTag6UI::dashedLine(
    display,
    10, 50,
    200, 50,
    4,
    3
);
```

---

# Иконки

## Батарея

```cpp
GTag6UI::batteryIcon(
    display,
    220, 5,
    80,
    24,
    12
);
```

Процент заряда:

```text
0..100
```

---

## Wi-Fi

```cpp
GTag6UI::wifiIcon(
    display,
    200, 5,
    3,
    12
);
```

Уровень:

```text
0..3
```

---

## Погода

Доступные типы:

```cpp
GTag6UI::WEATHER_SUN
GTag6UI::WEATHER_PARTLY
GTag6UI::WEATHER_CLOUD
GTag6UI::WEATHER_RAIN
GTag6UI::WEATHER_SNOW
GTag6UI::WEATHER_STORM
```

Пример:

```cpp
GTag6UI::weatherIcon(
    display,
    10, 20,
    40,
    GTag6UI::WEATHER_RAIN
);
```

---

# Семисегментные цифры

Для крупных чисел есть собственный семисегментный шрифт.

```cpp
GTag6UI::sevenSegment(
    display,
    20, 20,
    34,
    "22.5*C"
);
```

Символ `*` используется как знак градуса.

Поддерживаются:

```text
0-9
.
:
-
пробел
*
C F H E P L A U
```

Для определения ширины без рисования:

```cpp
int16_t w = GTag6UI::sevenSegmentWidth(
    34,
    "22.5*C"
);
```

Это удобно для центрирования.

---

# Работа с текстом

Получить ширину строки:

```cpp
int16_t w = GTag6UI::textWidth(
    display,
    "Hello",
    2
);
```

Вывести текст внутри прямоугольной области:

```cpp
GTag6UI::textBox(
    display,
    "Temperature",
    10, 10,
    150, 20,
    GTAG6_ALIGN_CENTER,
    1
);
```

Поддерживаются:

```cpp
GTAG6_ALIGN_LEFT
GTAG6_ALIGN_CENTER
GTAG6_ALIGN_RIGHT
```

---

# Перенос текста

Для автоматического переноса по словам:

```cpp
GTag6UI::wrappedText(
    display,
    "Light rain, wind west",
    10,
    90,
    100,
    1
);
```

Функция поддерживает `\n`.

Возвращаемое значение — координата `Y` после последней строки.

---

# GTag6Chart

`GTag6Chart` предназначен для отображения истории значений датчиков.

Подходит для:

- температуры;
- влажности;
- давления;
- напряжения;
- скорости;
- загрузки CPU;
- других числовых данных.

## Создание графика

```cpp
GTag6Chart chart(
    4, 66,    // x, y
    150, 58   // width, height
);
```

Можно задать количество хранимых значений:

```cpp
GTag6Chart chart(4, 66, 150, 58, 60);
```

Максимум:

```text
256 значений
```

---

## Добавление значения

```cpp
chart.push(22.5);
```

При добавлении новых значений старые автоматически уходят влево после заполнения буфера.

---

## Масштаб

Автоматический масштаб:

```cpp
chart.setAutoRange(true);
```

Фиксированный диапазон:

```cpp
chart.setRange(-10, 40);
```

---

## Сетка

```cpp
chart.setGrid(true, 4);
```

---

## Подписи

```cpp
chart.setLabels(true, 1);
```

Второй параметр — количество знаков после запятой.

---

## Стиль

Линия:

```cpp
chart.setStyle(GTAG6_CHART_LINE);
```

Столбики:

```cpp
chart.setStyle(GTAG6_CHART_BARS);
```

Заполненная область:

```cpp
chart.setStyle(GTAG6_CHART_AREA);
```

---

## Вывод графика

```cpp
chart.draw(display);
display.display();
```

---

# Полный пример Dashboard

В библиотеке есть готовый пример:

```text
examples/Dashboard/Dashboard.ino
```

Он демонстрирует:

- иконку погоды;
- температуру крупными цифрами;
- влажность;
- стрелочный индикатор;
- скорость ветра;
- progress bar;
- Wi-Fi;
- батарею;
- график температуры;
- перенос текста.

Запустить его можно через:

```text
Arduino IDE
→ Файл
→ Примеры
→ GTag6
→ Dashboard
```

---

# Примеры

В библиотеке доступны три примера:

### HelloWorld

```text
examples/HelloWorld/
```

Базовая работа с дисплеем, текстом и графикой.

### Dashboard

```text
examples/Dashboard/
```

Демонстрация UI, графика, погоды и индикаторов.

### Table

```text
examples/Table/
```

Работа с таблицами и ячейками.

---

# Кириллица

Стандартный встроенный шрифт `Adafruit_GFX` содержит латинские символы и не поддерживает русский алфавит.

Для вывода кириллицы можно использовать:

- `U8g2_for_Adafruit_GFX`;
- пользовательские шрифты `Adafruit_GFX`;
- шрифты, преобразованные с помощью `fontconvert`.

---

# Пример минимального проекта

```cpp
#include <GTag6.h>

GTag6 display(23, 18, 5, 16, 17);

void setup()
{
    display.begin();

    display.setAutoRefresh(5000);

    display.clearDisplay();
    display.setTextColor(GTAG6_BLACK);
    display.setTextSize(3);

    display.setCursor(20, 30);
    display.print("G-TAG 6");

    display.drawRect(
        2, 2,
        252, 124,
        GTAG6_BLACK
    );

    display.display();
}

void loop()
{
    display.tick();
}
```

---

# Структура проекта

```text
GTag6/
├── library.properties
├── keywords.txt
├── README.md
│
├── src/
│   ├── GTag6.h
│   ├── GTag6.cpp
│   ├── GTag6Widgets.h
│   └── GTag6Widgets.cpp
│
└── examples/
    ├── HelloWorld/
    │   └── HelloWorld.ino
    │
    ├── Dashboard/
    │   └── Dashboard.ino
    │
    └── Table/
        └── Table.ino
```

---

# Лицензия

Проект распространяется на условиях лицензии, указанной в репозитории.

---

## Автор

**GTag6**

Arduino-библиотека для работы с дисплеем **SES-imagotag G-TAG 6** на ESP32.
