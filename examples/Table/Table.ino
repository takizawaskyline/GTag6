#include <GTag6.h>

//            DIO CLK CS RESET DISCLK
GTag6 display(23, 18, 5, 16, 17);

// Ширины колонок: 96 + 78 + 78 = 252, плюс линия 1 px = 253 px
const int16_t COL_W[] = { 96, 78, 78 };

// 4 строки по 30 px: 4*30 + 1 = 121 px по высоте
GTag6Table table(display, 0, 3, 4, COL_W, 3, 30);

void drawScreen()
{
  display.clearDisplay();

  table.draw();

  // Шапка: чёрный фон, белый текст
  for (uint8_t c = 0; c < 3; c++)
    table.fillCell(0, c, GTAG6_BLACK);

  table.setCell(0, 0, "Item",  GTAG6_ALIGN_LEFT,   2, GTAG6_WHITE);
  table.setCell(0, 1, "Qty",   GTAG6_ALIGN_CENTER, 2, GTAG6_WHITE);
  table.setCell(0, 2, "Price", GTAG6_ALIGN_RIGHT,  2, GTAG6_WHITE);

  table.setCell(1, 0, "Milk",  GTAG6_ALIGN_LEFT,   2);
  table.setCell(1, 1, "2",     GTAG6_ALIGN_CENTER, 2);
  table.setCell(1, 2, "1.20",  GTAG6_ALIGN_RIGHT,  2);

  table.setCell(2, 0, "Bread", GTAG6_ALIGN_LEFT,   2);
  table.setCell(2, 1, "1",     GTAG6_ALIGN_CENTER, 2);
  table.setCell(2, 2, "0.99",  GTAG6_ALIGN_RIGHT,  2);

  table.setCell(3, 0, "Eggs",  GTAG6_ALIGN_LEFT,   2);
  table.setCell(3, 1, "12",    GTAG6_ALIGN_CENTER, 2);
  table.setCell(3, 2, "3.40",  GTAG6_ALIGN_RIGHT,  2);

  // Выделить ячейку инверсией цвета
  table.invertCell(3, 2);

  // Другие варианты инверсии:
  // display.invertRect(10, 10, 50, 20);   // любая область
  // display.invertBuffer();               // весь экран

  display.display();
}

void setup()
{
  display.begin();
  display.setAutoRefresh(5000);
  drawScreen();
}

void loop()
{
  display.tick();
}
