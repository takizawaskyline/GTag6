#include "GTag6.h"

#define GTAG6_LEDC_CHANNEL 0   // используется только в ESP32 core 2.x
#define GTAG6_CELL_PAD     2   // горизонтальный отступ текста в ячейке

GTag6::GTag6(int8_t dio, int8_t clk, int8_t cs, int8_t reset, int8_t dispClk)
  : Adafruit_GFX(GTAG6_WIDTH, GTAG6_HEIGHT),
    _dio(dio), _clk(clk), _cs(cs), _reset(reset), _dispClk(dispClk)
{
  memset(_fb, 0, sizeof(_fb));
}

// ------------------------------------------------------------
//  Публичные методы
// ------------------------------------------------------------

void GTag6::begin(uint32_t displayClkHz)
{
  pinMode(_dio, OUTPUT);
  pinMode(_clk, OUTPUT);
  pinMode(_cs, OUTPUT);
  pinMode(_reset, OUTPUT);

  digitalWrite(_dio, LOW);
  digitalWrite(_clk, LOW);
  digitalWrite(_cs, HIGH);
  digitalWrite(_reset, HIGH);

  startDisplayClock(displayClkHz);

  lcdInit();
  display();
  _lastRefresh = millis();
}

void GTag6::display()
{
  digitalWrite(_cs, LOW);

  send9Raw(false, 0x2A);
  send9Raw(true, 0x00);
  send9Raw(true, 0x00);
  send9Raw(false, 0x2C);

  for (uint16_t i = 0; i < GTAG6_BUF_SIZE; i++)
    sendPixelByte(_inverted ? (uint8_t)~_fb[i] : _fb[i]);

  digitalWrite(_cs, HIGH);
}

void GTag6::refresh()
{
  lcdInit();
  display();
  _lastRefresh = millis();
}

void GTag6::clearDisplay()
{
  fillScreen(GTAG6_WHITE);
}

void GTag6::setAutoRefresh(uint32_t ms)
{
  _autoMs = ms;
  _lastRefresh = millis();
}

void GTag6::tick()
{
  if (_autoMs && (millis() - _lastRefresh >= _autoMs))
    refresh();
}

void GTag6::displayOn()  { send9(false, 0x29); }
void GTag6::displayOff() { send9(false, 0x28); }

// ------------------------------------------------------------
//  Инверсия
// ------------------------------------------------------------

void GTag6::invertDisplay(bool inv)
{
  _inverted = inv;
}

void GTag6::invertBuffer()
{
  for (uint16_t i = 0; i < GTAG6_BUF_SIZE; i++)
    _fb[i] = ~_fb[i];
}

void GTag6::invertRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
  int16_t x0 = (x < 0) ? 0 : x;
  int16_t y0 = (y < 0) ? 0 : y;
  int16_t x1 = (x + w > _width)  ? _width  : x + w;
  int16_t y1 = (y + h > _height) ? _height : y + h;

  for (int16_t yy = y0; yy < y1; yy++)
  {
    for (int16_t xx = x0; xx < x1; xx++)
    {
      uint16_t idx;
      uint8_t mask;
      if (locate(xx, yy, idx, mask))
        _fb[idx] ^= mask;
    }
  }
}

// ------------------------------------------------------------
//  Adafruit_GFX
// ------------------------------------------------------------

// Координаты с учётом поворота -> индекс байта и маска бита в буфере
bool GTag6::locate(int16_t x, int16_t y, uint16_t &index, uint8_t &mask)
{
  if (x < 0 || x >= _width || y < 0 || y >= _height)
    return false;

  int16_t t;
  switch (getRotation())
  {
    case 1: t = x; x = GTAG6_WIDTH - 1 - y; y = t; break;
    case 2: x = GTAG6_WIDTH - 1 - x; y = GTAG6_HEIGHT - 1 - y; break;
    case 3: t = x; x = y; y = GTAG6_HEIGHT - 1 - t; break;
    default: break;
  }

  index = ((uint16_t)y * (GTAG6_WIDTH / 8)) + (x >> 3);
  mask = 0x80 >> (x & 7);
  return true;
}

void GTag6::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  uint16_t index;
  uint8_t mask;

  if (!locate(x, y, index, mask))
    return;

  if (color)
    _fb[index] |= mask;
  else
    _fb[index] &= ~mask;
}

bool GTag6::getPixel(int16_t x, int16_t y)
{
  uint16_t index;
  uint8_t mask;

  if (!locate(x, y, index, mask))
    return false;

  return (_fb[index] & mask) != 0;
}

void GTag6::fillScreen(uint16_t color)
{
  memset(_fb, color ? 0xFF : 0x00, sizeof(_fb));
}

// ------------------------------------------------------------
//  Таблица (быстрая сетка)
// ------------------------------------------------------------

void GTag6::drawTable(int16_t x, int16_t y, uint8_t rows, uint8_t cols,
                      int16_t cellW, int16_t cellH, uint8_t lineWidth)
{
  GTag6Table t(*this, x, y, rows, cols, cellW, cellH);
  t.setLineWidth(lineWidth);
  t.draw();
}

// ------------------------------------------------------------
//  Низкий уровень
// ------------------------------------------------------------

void GTag6::startDisplayClock(uint32_t hz)
{
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  ledcAttach(_dispClk, hz, 8);
  ledcWrite(_dispClk, 128);
#else
  ledcSetup(GTAG6_LEDC_CHANNEL, hz, 8);
  ledcAttachPin(_dispClk, GTAG6_LEDC_CHANNEL);
  ledcWrite(GTAG6_LEDC_CHANNEL, 128);
#endif
}

inline void GTag6::pulseClock()
{
  digitalWrite(_clk, HIGH);
  delayMicroseconds(1);
  digitalWrite(_clk, LOW);
  delayMicroseconds(1);
}

// 9 бит: бит D/C + 8 бит данных, MSB first, без управления CS
void GTag6::send9Raw(bool data, uint8_t value)
{
  digitalWrite(_dio, data ? HIGH : LOW);
  pulseClock();

  for (int i = 7; i >= 0; i--)
  {
    digitalWrite(_dio, (value >> i) & 1);
    pulseClock();
  }
}

void GTag6::send9(bool data, uint8_t value)
{
  digitalWrite(_cs, LOW);
  send9Raw(data, value);
  digitalWrite(_cs, HIGH);
  delayMicroseconds(2);
}

// Байт изображения: бит данных = 1, затем 8 бит LSB first, инвертированные
void GTag6::sendPixelByte(uint8_t value)
{
  digitalWrite(_dio, HIGH);
  pulseClock();

  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(_dio, (value & 0x01) ? LOW : HIGH);
    pulseClock();
    value >>= 1;
  }
}

void GTag6::lcdInit()
{
  digitalWrite(_cs, HIGH);

  // RESET
  digitalWrite(_reset, LOW);
  delayMicroseconds(100);
  digitalWrite(_reset, HIGH);
  delay(50);

  // Sleep Out
  send9(false, 0x11);
  delay(50);

  // Адрес + первоначальное заполнение белым
  digitalWrite(_cs, LOW);
  send9Raw(false, 0x2A);
  send9Raw(true, 0x00);
  send9Raw(true, 0x00);
  send9Raw(false, 0x2C);

  for (uint16_t i = 0; i < GTAG6_BUF_SIZE; i++)
    send9Raw(true, 0xFF);

  digitalWrite(_cs, HIGH);
  delay(10);

  // 0x4C
  digitalWrite(_cs, LOW);
  send9Raw(false, 0x4C);
  send9Raw(true, 0x0C);
  send9Raw(true, 0x00);
  send9Raw(true, 0x00);
  digitalWrite(_cs, HIGH);
  delay(4);

  // 0x4D
  digitalWrite(_cs, LOW);
  send9Raw(false, 0x4D);
  send9Raw(true, 0xFF);
  send9Raw(true, 0x00);
  send9Raw(true, 0x7F);
  digitalWrite(_cs, HIGH);
  delay(1);

  // 0x4E
  digitalWrite(_cs, LOW);
  send9Raw(false, 0x4E);
  send9Raw(true, 0x60);
  digitalWrite(_cs, HIGH);
  delay(10);

  // Display ON
  send9(false, 0x29);
  delay(10);
}


// ============================================================
//  GTag6Table
// ============================================================

GTag6Table::GTag6Table(GTag6 &d, int16_t x, int16_t y, uint8_t rows, uint8_t cols,
                       int16_t cellW, int16_t cellH)
  : _d(d), _x(x), _y(y), _rows(rows),
    _cols(cols > GTAG6_TABLE_MAX_COLS ? GTAG6_TABLE_MAX_COLS : cols),
    _rowH(cellH)
{
  for (uint8_t i = 0; i < _cols; i++)
    _colW[i] = cellW;
}

GTag6Table::GTag6Table(GTag6 &d, int16_t x, int16_t y, uint8_t rows,
                       const int16_t *colW, uint8_t cols, int16_t cellH)
  : _d(d), _x(x), _y(y), _rows(rows),
    _cols(cols > GTAG6_TABLE_MAX_COLS ? GTAG6_TABLE_MAX_COLS : cols),
    _rowH(cellH)
{
  for (uint8_t i = 0; i < _cols; i++)
    _colW[i] = colW[i];
}

void GTag6Table::setLineWidth(uint8_t t)
{
  _t = (t == 0) ? 1 : t;
}

int16_t GTag6Table::colX(uint8_t c) const
{
  int16_t v = _x;
  for (uint8_t i = 0; i < c && i < _cols; i++)
    v += _colW[i];
  return v;
}

int16_t GTag6Table::rowY(uint8_t r) const
{
  return _y + (int16_t)r * _rowH;
}

int16_t GTag6Table::width() const
{
  return colX(_cols) - _x + _t;
}

int16_t GTag6Table::height() const
{
  return (int16_t)_rows * _rowH + _t;
}

void GTag6Table::draw(uint16_t color)
{
  int16_t W = width();
  int16_t H = height();

  for (uint8_t r = 0; r <= _rows; r++)
    _d.fillRect(_x, rowY(r), W, _t, color);

  for (uint8_t c = 0; c <= _cols; c++)
    _d.fillRect(colX(c), _y, _t, H, color);
}

bool GTag6Table::cellRect(uint8_t row, uint8_t col,
                          int16_t &x, int16_t &y, int16_t &w, int16_t &h) const
{
  if (row >= _rows || col >= _cols)
    return false;

  x = colX(col) + _t;
  y = rowY(row) + _t;
  w = _colW[col] - _t;
  h = _rowH - _t;
  return true;
}

void GTag6Table::fillCell(uint8_t row, uint8_t col, uint16_t color)
{
  int16_t x, y, w, h;
  if (cellRect(row, col, x, y, w, h))
    _d.fillRect(x, y, w, h, color);
}

void GTag6Table::invertCell(uint8_t row, uint8_t col)
{
  int16_t x, y, w, h;
  if (cellRect(row, col, x, y, w, h))
    _d.invertRect(x, y, w, h);
}

void GTag6Table::setCell(uint8_t row, uint8_t col, const char *text,
                         GTag6Align align, uint8_t textSize, uint16_t color)
{
  int16_t cx, cy, cw, ch;
  if (!text || !cellRect(row, col, cx, cy, cw, ch))
    return;

  char buf[64];
  strncpy(buf, text, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = 0;

  _d.setTextSize(textSize);
  _d.setTextColor(color);

  int16_t x1, y1;
  uint16_t w, h;
  _d.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  // Обрезаем текст, пока не влезет по ширине
  size_t len = strlen(buf);
  while (len > 0 && (int16_t)w > cw - 2 * GTAG6_CELL_PAD)
  {
    buf[--len] = 0;
    _d.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  }

  int16_t px;
  switch (align)
  {
    case GTAG6_ALIGN_LEFT:  px = cx + GTAG6_CELL_PAD; break;
    case GTAG6_ALIGN_RIGHT: px = cx + cw - GTAG6_CELL_PAD - (int16_t)w; break;
    default:                px = cx + (cw - (int16_t)w) / 2; break;
  }
  int16_t py = cy + (ch - (int16_t)h) / 2;

  _d.setCursor(px - x1, py - y1);
  _d.print(buf);
}

void GTag6Table::setCell(uint8_t row, uint8_t col, const String &text,
                         GTag6Align align, uint8_t textSize, uint16_t color)
{
  setCell(row, col, text.c_str(), align, textSize, color);
}
