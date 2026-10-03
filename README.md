# GTag6

Библиотека для дисплея электронного ценника **G-TAG 6** (256x128, 1 бит) на **ESP32**.
Наследуется от `Adafruit_GFX`, поэтому работают все привычные функции:
`drawPixel`, `drawLine`, `drawRect`, `fillRect`, `drawCircle`, `fillCircle`, `drawTriangle`,
`drawRoundRect`, `drawBitmap`, `setFont`, `setTextSize`, `setCursor`, `print/println`,
`getTextBounds`, `setRotation` (0-3) и т.д.

## Установка
1. Установить **Adafruit GFX Library** (Менеджер библиотек).
2. Скопировать папку `GTag6` в `Documents/Arduino/libraries/`.

## Использование
```cpp
#include <GTag6.h>
GTag6 display(23, 18, 5, 16, 17);   // DIO, CLK, CS, RESET, DISCLK

void setup() {
  display.begin();
  display.setTextColor(GTAG6_BLACK);
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.print("Hello");
  display.display();
}
void loop() {}
```

## Методы GTag6
| Метод | Описание |
|---|---|
| `begin(hz=29000)` | Пины, 29 кГц DisplayCLK, инициализация |
| `display()` | Отправить буфер на экран |
| `refresh()` | Полный RESET + init + отправка буфера |
| `clearDisplay()` / `fillScreen(c)` | Заливка буфера |
| `setAutoRefresh(ms)` + `tick()` | Периодический refresh из `loop()` |
| `invertDisplay(bool)` | Инверсия при отправке (буфер не меняется) |
| `invertBuffer()` | Инвертировать весь буфер |
| `invertRect(x,y,w,h)` | Инвертировать область |
| `getPixel(x,y)` | Прочитать пиксель (true = чёрный) |
| `drawTable(x,y,rows,cols,cellW,cellH,line)` | Быстрая сетка без текста |
| `displayOn()` / `displayOff()` | Команды 0x29 / 0x28 |
| `getBuffer()` | Прямой доступ к буферу (4096 байт) |

Цвета: `GTAG6_BLACK` (1), `GTAG6_WHITE` (0).

## Таблицы: GTag6Table
```cpp
const int16_t COL_W[] = {96, 78, 78};
GTag6Table table(display, 0, 3, 4, COL_W, 3, 30);  // x, y, строки, ширины колонок, кол-во колонок, высота строки
// или с одинаковыми ячейками: GTag6Table table(display, 0, 3, 4, 3, 80, 30);

table.draw();
table.fillCell(0, 0, GTAG6_BLACK);
table.setCell(0, 0, "Item", GTAG6_ALIGN_LEFT, 2, GTAG6_WHITE);
table.setCell(1, 2, "1.20", GTAG6_ALIGN_RIGHT, 2);
table.invertCell(3, 2);
```
Методы: `setLineWidth`, `width`, `height`, `draw`, `cellRect`, `fillCell`, `invertCell`, `setCell`.
Текст выравнивается `GTAG6_ALIGN_LEFT / CENTER / RIGHT`, по вертикали центрируется, длинный текст обрезается.

## Виджеты: GTag6UI (метеостанции, датчики, дашборды)
Функции работают с любым `Adafruit_GFX`-дисплеем, подключаются автоматически через `#include <GTag6.h>`.

| Функция | Описание |
|---|---|
| `GTag6UI::progressBar(d, x, y, w, h, percent)` | Полоса прогресса 0..100 |
| `GTag6UI::gauge(d, cx, cy, r, value, min, max, ticks)` | Стрелочный индикатор-полукруг (r от 12) |
| `GTag6UI::arc(d, cx, cy, r, thickness, startDeg, endDeg)` | Дуга; 0° = вверх, по часовой стрелке |
| `GTag6UI::thickLine(d, x0, y0, x1, y1, t)` | Линия заданной толщины |
| `GTag6UI::dashedLine(d, x0, y0, x1, y1, dash, gap)` | Пунктирная линия |
| `GTag6UI::batteryIcon(d, x, y, percent, w, h)` | Батарейка |
| `GTag6UI::wifiIcon(d, x, y, level 0..3, size)` | Значок Wi-Fi |
| `GTag6UI::weatherIcon(d, x, y, size, type)` | Погода: `WEATHER_SUN / PARTLY / CLOUD / RAIN / SNOW / STORM` |
| `GTag6UI::sevenSegment(d, x, y, h, "22.5*C")` | Крупные семисегментные цифры. Символы: `0-9 . : - пробел`, `*` = знак градуса, буквы `C F H E P L A U` |
| `GTag6UI::sevenSegmentWidth(h, text)` | Ширина надписи (для выравнивания) |
| `GTag6UI::textBox(d, text, x, y, w, h, align, size)` | Текст в прямоугольнике с выравниванием |
| `GTag6UI::wrappedText(d, text, x, y, w, size)` | Перенос по словам, поддерживает `\n`, возвращает Y под текстом |
| `GTag6UI::textWidth(d, text, size)` | Ширина строки в пикселях |

## График: GTag6Chart
```cpp
GTag6Chart chart(4, 66, 150, 58);   // x, y, ширина, высота (вместе с подписями)
chart.setGrid(true, 3);             // пунктирная сетка
chart.setLabels(true, 1);           // подписи min/max слева, 1 знак после запятой
chart.setAutoRange();               // масштаб по данным (или setRange(-10, 40))
chart.setStyle(GTAG6_CHART_LINE);   // GTAG6_CHART_BARS, GTAG6_CHART_AREA

chart.push(temperature);            // новое значение, старые уходят влево
chart.draw(display);
display.display();
```
Хранит до 256 последних значений (`GTag6Chart(x, y, w, h, points)` - сколько хранить, по умолчанию `w`).
Методы: `push`, `clear`, `count`, `capacity`, `get(i)`, `last`, `setAutoRange`, `setRange`, `setGrid`, `setLabels`, `setStyle`, `draw`.

Полный пример - `examples/Dashboard`.

## Кириллица
Встроенный шрифт Adafruit_GFX только латинский. Для русского текста используйте
`U8g2_for_Adafruit_GFX` или шрифты, сконвертированные `fontconvert`.
