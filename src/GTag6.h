#pragma once
// ============================================================
//  GTag6 - драйвер дисплея электронного ценника G-TAG 6
//  Экран 256x128, 1 бит на пиксель, ESP32 (Arduino core 2.x/3.x)
//  Наследуется от Adafruit_GFX -> доступны все функции рисования.
// ============================================================

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "GTag6Widgets.h"   // цвета, GTag6Align, виджеты (GTag6UI::*) и GTag6Chart

#define GTAG6_WIDTH     256
#define GTAG6_HEIGHT    128
#define GTAG6_BUF_SIZE  4096          // 256 * 128 / 8

class GTag6 : public Adafruit_GFX {
public:
  // dio, clk, cs, reset - шина дисплея; dispClk - пин 29 кГц "DisplayCLK"
  GTag6(int8_t dio, int8_t clk, int8_t cs, int8_t reset, int8_t dispClk);

  // Настройка пинов, запуск тактирования, инициализация и вывод пустого кадра
  void begin(uint32_t displayClkHz = 29000);

  // --- Вывод ---
  void display();                     // отправить framebuffer на экран
  void refresh();                     // полный RESET + инициализация + отправка кадра
  void clearDisplay();                // залить белым (в буфере)

  // Автообновление: refresh() каждые ms миллисекунд при вызове tick() в loop().
  // 0 - выключено.
  void setAutoRefresh(uint32_t ms);
  void tick();

  void displayOn();                   // команда 0x29
  void displayOff();                  // команда 0x28 (стандартная пара к 0x29, не проверена на железе)

  // --- Инверсия цвета ---
  // invertDisplay(true/false) - флаг: кадр инвертируется при отправке, буфер не меняется
  // invertBuffer()            - реально инвертирует весь буфер (чёрное <-> белое)
  // invertRect(x,y,w,h)       - инвертирует прямоугольную область (с учётом поворота)
  void invertDisplay(bool inv) override;
  void invertBuffer();
  void invertRect(int16_t x, int16_t y, int16_t w, int16_t h);

  // --- Пиксели ---
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  bool getPixel(int16_t x, int16_t y);          // true = чёрный
  void fillScreen(uint16_t color) override;

  // --- Таблицы ---
  // Быстрая сетка rows x cols с одинаковыми ячейками. Для текста в ячейках
  // используйте класс GTag6Table (ниже).
  void drawTable(int16_t x, int16_t y, uint8_t rows, uint8_t cols,
                 int16_t cellW, int16_t cellH, uint8_t lineWidth = 1);

  // Прямой доступ к буферу (формат: 32 байта на строку, MSB = левый пиксель, 1 = чёрный)
  uint8_t *getBuffer() { return _fb; }

private:
  int8_t _dio, _clk, _cs, _reset, _dispClk;
  bool _inverted = false;
  uint32_t _autoMs = 0;
  uint32_t _lastRefresh = 0;
  uint8_t _fb[GTAG6_BUF_SIZE];

  bool locate(int16_t x, int16_t y, uint16_t &index, uint8_t &mask);
  void startDisplayClock(uint32_t hz);
  void lcdInit();
  inline void pulseClock();
  void send9Raw(bool data, uint8_t value);
  void send9(bool data, uint8_t value);
  void sendPixelByte(uint8_t value);
};


// ============================================================
//  GTag6Table - таблица с текстом в ячейках
//
//  Геометрия: каждая ячейка занимает colW x rowH, включая ЛЕВУЮ и ВЕРХНЮЮ
//  линию сетки. Общая ширина = сумма colW + толщина линии, высота аналогично
//  (см. width() / height()).
// ============================================================

#define GTAG6_TABLE_MAX_COLS 16

class GTag6Table {
public:
  // Одинаковые ячейки
  GTag6Table(GTag6 &d, int16_t x, int16_t y, uint8_t rows, uint8_t cols,
             int16_t cellW, int16_t cellH);

  // Разная ширина колонок: colW - массив из cols значений (копируется)
  GTag6Table(GTag6 &d, int16_t x, int16_t y, uint8_t rows,
             const int16_t *colW, uint8_t cols, int16_t cellH);

  void setLineWidth(uint8_t t);            // толщина линий сетки (по умолчанию 1)
  int16_t width() const;                   // полная ширина таблицы в пикселях
  int16_t height() const;                  // полная высота таблицы в пикселях

  void draw(uint16_t color = GTAG6_BLACK); // нарисовать сетку

  // Внутренняя область ячейки (без линий). false, если row/col вне таблицы
  bool cellRect(uint8_t row, uint8_t col,
                int16_t &x, int16_t &y, int16_t &w, int16_t &h) const;

  void fillCell(uint8_t row, uint8_t col, uint16_t color);
  void invertCell(uint8_t row, uint8_t col);

  // Текст в ячейке (текущий шрифт GFX). Текст, не влезающий по ширине, обрезается.
  // Для текста на чёрном фоне: fillCell(r,c,GTAG6_BLACK); setCell(..., GTAG6_WHITE).
  void setCell(uint8_t row, uint8_t col, const char *text,
               GTag6Align align = GTAG6_ALIGN_CENTER,
               uint8_t textSize = 1, uint16_t color = GTAG6_BLACK);
  void setCell(uint8_t row, uint8_t col, const String &text,
               GTag6Align align = GTAG6_ALIGN_CENTER,
               uint8_t textSize = 1, uint16_t color = GTAG6_BLACK);

private:
  GTag6 &_d;
  int16_t _x, _y;
  uint8_t _rows, _cols;
  int16_t _rowH;
  int16_t _colW[GTAG6_TABLE_MAX_COLS];
  uint8_t _t = 1;

  int16_t colX(uint8_t c) const;
  int16_t rowY(uint8_t r) const;
};
