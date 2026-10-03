// Пример: экран метеостанции на G-TAG 6
// Показывает иконку погоды, температуру крупными цифрами, влажность (стрелочный
// индикатор), график температуры, ветер (полоса) и текст с переносом по словам.
// Данные здесь тестовые - замените их показаниями своих датчиков.

#include <GTag6.h>
#include <math.h>

//            DIO CLK CS RESET DISCLK
GTag6 display(23, 18, 5, 16, 17);

GTag6Chart chart(4, 66, 150, 58);   // x, y, ширина, высота

float temp = 22.0f;
int hum = 56;
int wind = 12;

void drawDashboard()
{
  char buf[24];

  display.clearDisplay();
  display.setTextColor(GTAG6_BLACK);

  // --- Верхняя строка ---
  display.setTextSize(1);
  display.setCursor(4, 3);
  display.print("WEATHER STATION");
  GTag6UI::wifiIcon(display, 204, 1, 3, 11);
  GTag6UI::batteryIcon(display, 228, 2, 80, 24, 10);
  display.drawFastHLine(0, 14, 256, GTAG6_BLACK);

  // --- Иконка погоды и температура крупными цифрами ---
  GTag6UI::weatherIcon(display, 4, 20, 40, GTag6UI::WEATHER_RAIN);
  snprintf(buf, sizeof(buf), "%.1f*C", temp);       // '*' рисуется как знак градуса
  GTag6UI::sevenSegment(display, 52, 24, 34, buf);

  // --- Влажность: стрелочный индикатор ---
  GTag6UI::gauge(display, 212, 62, 38, hum, 0, 100, 10);
  snprintf(buf, sizeof(buf), "HUM %d%%", hum);
  GTag6UI::textBox(display, buf, 172, 68, 80, 10, GTAG6_ALIGN_CENTER, 1);

  // --- Ветер: полоса прогресса (0..30 м/с) ---
  snprintf(buf, sizeof(buf), "WIND %d M/S", wind);
  GTag6UI::textBox(display, buf, 172, 82, 80, 10, GTAG6_ALIGN_LEFT, 1);
  GTag6UI::progressBar(display, 172, 93, 80, 9, wind * 100.0f / 30.0f);

  // --- Текст с переносом ---
  GTag6UI::wrappedText(display, "Light rain, wind west", 172, 106, 84, 1);

  // --- График температуры ---
  chart.draw(display);

  display.display();
}

void setup()
{
  Serial.begin(115200);
  display.begin();

  chart.setGrid(true, 3);         // пунктирная сетка
  chart.setLabels(true, 0);       // подписи min/max слева, 0 знаков после запятой
  chart.setAutoRange(true);       // масштаб по данным
  // chart.setRange(-10, 40);     // или фиксированный диапазон
  // chart.setStyle(GTAG6_CHART_BARS);   // столбики; GTAG6_CHART_AREA - область

  for (int i = 0; i < 60; i++)    // тестовая история
    chart.push(22.0f + 4.0f * sinf(i / 8.0f));

  display.setAutoRefresh(5000);
  drawDashboard();
}

void loop()
{
  static uint32_t last = 0;

  display.tick();

  if (millis() - last >= 10000)
  {
    last = millis();

    float t = millis() / 60000.0f;
    temp = 22.0f + 4.0f * sinf(t);
    hum  = 50 + (int)(20.0f * sinf(t * 0.7f));
    wind = 8 + (int)(6.0f * sinf(t * 1.3f));

    chart.push(temp);
    drawDashboard();
  }
}
