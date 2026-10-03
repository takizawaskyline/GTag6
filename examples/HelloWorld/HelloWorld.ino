#include <GTag6.h>

//            DIO CLK CS RESET DISCLK
GTag6 display(23, 18, 5, 16, 17);

// Текст по центру экрана по горизонтали
void centerText(const char *s, int y, uint8_t size)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(size);
  display.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((display.width() - w) / 2 - x1, y);
  display.print(s);
}

void drawScreen()
{
  display.clearDisplay();
  display.setTextColor(GTAG6_BLACK);

  // Двойная рамка
  display.drawRect(2, 2, 252, 124, GTAG6_BLACK);
  display.drawRect(3, 3, 250, 122, GTAG6_BLACK);

  centerText("HELLO WORLD", 14, 3);
  display.fillRect(12, 43, 232, 2, GTAG6_BLACK);
  centerText("G TAG", 58, 4);

  // Сердце: два круга + треугольник
  display.fillCircle(122, 98, 6, GTAG6_BLACK);
  display.fillCircle(134, 98, 6, GTAG6_BLACK);
  display.fillTriangle(116, 101, 140, 101, 128, 118, GTAG6_BLACK);

  display.fillRect(18, 102, 16, 16, GTAG6_BLACK);
  display.fillRect(222, 102, 16, 16, GTAG6_BLACK);

  display.display();   // отправить буфер на экран
}

void setup()
{
  Serial.begin(115200);
  display.begin();

  // display.setRotation(2);          // поворот на 180°
  // display.invertDisplay(true);     // инверсия при отправке (буфер не меняется)

  display.setAutoRefresh(5000);       // полный refresh раз в 5 с
  drawScreen();
}

void loop()
{
  display.tick();                     // выполняет автообновление
}
