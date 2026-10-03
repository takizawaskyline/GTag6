#pragma once
// ============================================================
//  GTag6Widgets - готовые элементы интерфейса для дисплея G-TAG 6
//  (метеостанции, датчики, дашборды, часы и т.п.)
//
//  Все функции работают с любым Adafruit_GFX-дисплеем, в том числе GTag6.
//  Подключается автоматически через #include <GTag6.h>
// ============================================================

#include <Arduino.h>
#include <Adafruit_GFX.h>

#ifndef GTAG6_BLACK
#define GTAG6_BLACK     1             // любой цвет != 0 считается чёрным
#endif
#ifndef GTAG6_WHITE
#define GTAG6_WHITE     0
#endif

enum GTag6Align : uint8_t {
  GTAG6_ALIGN_LEFT = 0,
  GTAG6_ALIGN_CENTER = 1,
  GTAG6_ALIGN_RIGHT = 2
};

namespace GTag6UI {

// Типы иконок погоды
enum Weather : uint8_t {
  WEATHER_SUN = 0,        // солнце
  WEATHER_PARTLY,         // солнце за облаком
  WEATHER_CLOUD,          // облако
  WEATHER_RAIN,           // дождь
  WEATHER_SNOW,           // снег
  WEATHER_STORM           // гроза
};

// ---------- Линии и дуги ----------

// Линия заданной толщины
void thickLine(Adafruit_GFX &d, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
               uint8_t thickness = 2, uint16_t color = GTAG6_BLACK);

// Пунктирная линия
void dashedLine(Adafruit_GFX &d, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                uint8_t dash = 3, uint8_t gap = 3, uint16_t color = GTAG6_BLACK);

// Дуга окружности. Углы в градусах: 0 = вверх, 90 = вправо, 180 = вниз, -90 = влево
// (по часовой стрелке). thickness - толщина внутрь от радиуса r.
void arc(Adafruit_GFX &d, int16_t cx, int16_t cy, int16_t r, uint8_t thickness,
         float startDeg, float endDeg, uint16_t color = GTAG6_BLACK);

// ---------- Индикаторы ----------

// Полоса прогресса, percent 0..100
void progressBar(Adafruit_GFX &d, int16_t x, int16_t y, int16_t w, int16_t h,
                 float percent, uint16_t color = GTAG6_BLACK);

// Стрелочный индикатор-полукруг (спидометр). (cx,cy) - центр, r >= 12.
// Занимает область от cy-r до cy. ticks - число делений шкалы (0 = без делений).
void gauge(Adafruit_GFX &d, int16_t cx, int16_t cy, int16_t r,
           float value, float minValue, float maxValue,
           uint8_t ticks = 5, uint16_t color = GTAG6_BLACK);

// ---------- Иконки ----------

// Батарейка, percent 0..100. (x,y) - левый верхний угол.
void batteryIcon(Adafruit_GFX &d, int16_t x, int16_t y, float percent,
                 int16_t w = 24, int16_t h = 12, uint16_t color = GTAG6_BLACK);

// Значок Wi-Fi, level 0..3. size - высота значка в пикселях (от 8).
void wifiIcon(Adafruit_GFX &d, int16_t x, int16_t y, uint8_t level,
              int16_t size = 12, uint16_t color = GTAG6_BLACK);

// Иконка погоды в квадрате size x size. (x,y) - левый верхний угол. size от 16.
void weatherIcon(Adafruit_GFX &d, int16_t x, int16_t y, int16_t size,
                 Weather type, uint16_t color = GTAG6_BLACK);

// ---------- Крупные цифры (семисегментный шрифт) ----------

// Поддерживаются: 0-9 . : - пробел, '*' (знак градуса), буквы C F H E P L A U.
// h - высота в пикселях. thickness 0 = автоматически. Возвращает ширину надписи.
int16_t sevenSegment(Adafruit_GFX &d, int16_t x, int16_t y, int16_t h, const char *text,
                     uint8_t thickness = 0, uint16_t color = GTAG6_BLACK);

// Ширина надписи без рисования (для выравнивания)
int16_t sevenSegmentWidth(int16_t h, const char *text, uint8_t thickness = 0);

// ---------- Текст ----------

// Ширина строки в пикселях при текущем шрифте
int16_t textWidth(Adafruit_GFX &d, const char *text, uint8_t size = 1);

// Текст в прямоугольнике (центрируется по вертикали, лишнее обрезается)
void textBox(Adafruit_GFX &d, const char *text, int16_t x, int16_t y, int16_t w, int16_t h,
             GTag6Align align = GTAG6_ALIGN_CENTER, uint8_t size = 1,
             uint16_t color = GTAG6_BLACK);

// Текст с переносом по словам в колонке шириной w. Поддерживает '\n'.
// lineH = 0 - межстрочный интервал по умолчанию (для встроенного шрифта).
// Возвращает Y под последней строкой.
int16_t wrappedText(Adafruit_GFX &d, const char *text, int16_t x, int16_t y, int16_t w,
                    uint8_t size = 1, uint16_t color = GTAG6_BLACK, int16_t lineH = 0);

}  // namespace GTag6UI


// ============================================================
//  GTag6Chart - график по последним N значениям (датчики, история)
//
//  Новые значения добавляются через push(), старые уходят влево.
//  Память: до 256 значений float внутри объекта.
// ============================================================

#define GTAG6_CHART_MAX_POINTS 256

enum GTag6ChartStyle : uint8_t {
  GTAG6_CHART_LINE = 0,     // линия
  GTAG6_CHART_BARS,         // столбики
  GTAG6_CHART_AREA          // залитая область под линией
};

class GTag6Chart {
public:
  // (x,y,w,h) - область графика вместе с рамкой и подписями.
  // points - сколько значений хранить (0 = w, максимум 256).
  GTag6Chart(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t points = 0);

  void push(float value);                    // добавить новое значение
  void clear();
  uint16_t count() const { return _count; }
  uint16_t capacity() const { return _cap; }
  float get(uint16_t i) const;               // 0 = самое старое значение
  float last() const;                        // последнее добавленное

  void setAutoRange(bool on = true);         // масштаб по min/max данных (по умолчанию)
  void setRange(float lo, float hi);         // фиксированный диапазон по Y
  void setGrid(bool on, uint8_t divisions = 4);   // пунктирная сетка
  void setLabels(bool on, uint8_t decimals = 0);  // подписи min/max слева
  void setStyle(GTag6ChartStyle style);

  void draw(Adafruit_GFX &d, uint16_t color = GTAG6_BLACK);

private:
  int16_t _x, _y, _w, _h;
  uint16_t _cap, _count = 0, _head = 0;
  bool _auto = true, _grid = false, _labels = false;
  uint8_t _div = 4, _dec = 0;
  float _lo = 0, _hi = 1;
  GTag6ChartStyle _style = GTAG6_CHART_LINE;
  float _data[GTAG6_CHART_MAX_POINTS];
};
