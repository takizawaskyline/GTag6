#include "GTag6Widgets.h"
#include <math.h>

#define GTAG6_TEXT_PAD 2   // горизонтальный отступ текста в textBox

static inline int16_t iround(float v) { return (int16_t)lroundf(v); }
static const float DEG2RAD = 0.017453293f;

namespace GTag6UI {

// ------------------------------------------------------------
//  Линии и дуги
// ------------------------------------------------------------

void thickLine(Adafruit_GFX &d, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
               uint8_t thickness, uint16_t color)
{
  if (thickness <= 1)
  {
    d.drawLine(x0, y0, x1, y1, color);
    return;
  }

  int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
  int16_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);
  int16_t half = thickness / 2;

  for (int16_t i = -half; i < (int16_t)thickness - half; i++)
  {
    if (dx >= dy)
      d.drawLine(x0, y0 + i, x1, y1 + i, color);
    else
      d.drawLine(x0 + i, y0, x1 + i, y1, color);
  }
}

void dashedLine(Adafruit_GFX &d, int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                uint8_t dash, uint8_t gap, uint16_t color)
{
  float dx = x1 - x0;
  float dy = y1 - y0;
  float len = sqrtf(dx * dx + dy * dy);

  if (len < 1.0f)
  {
    d.drawPixel(x0, y0, color);
    return;
  }

  uint8_t period = dash + gap;
  if (period == 0) period = 1;

  int16_t n = (int16_t)len;
  for (int16_t i = 0; i <= n; i++)
  {
    if ((i % period) < dash)
    {
      float t = i / len;
      d.drawPixel(x0 + iround(dx * t), y0 + iround(dy * t), color);
    }
  }
}

void arc(Adafruit_GFX &d, int16_t cx, int16_t cy, int16_t r, uint8_t thickness,
         float startDeg, float endDeg, uint16_t color)
{
  if (r <= 0) return;
  if (thickness < 1) thickness = 1;

  if (endDeg < startDeg)
  {
    float t = startDeg;
    startDeg = endDeg;
    endDeg = t;
  }

  int16_t rin = r - (int16_t)thickness + 1;
  if (rin < 0) rin = 0;

  for (int16_t rr = rin; rr <= r; rr++)
  {
    if (rr == 0)
    {
      d.drawPixel(cx, cy, color);
      continue;
    }

    float step = 28.6479f / rr;   // шаг примерно 0.5 пикселя

    for (float a = startDeg; a <= endDeg; a += step)
    {
      float rad = a * DEG2RAD;
      d.drawPixel(cx + iround(rr * sinf(rad)), cy - iround(rr * cosf(rad)), color);
    }

    float rad = endDeg * DEG2RAD;
    d.drawPixel(cx + iround(rr * sinf(rad)), cy - iround(rr * cosf(rad)), color);
  }
}

// ------------------------------------------------------------
//  Индикаторы
// ------------------------------------------------------------

void progressBar(Adafruit_GFX &d, int16_t x, int16_t y, int16_t w, int16_t h,
                 float percent, uint16_t color)
{
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  d.drawRect(x, y, w, h, color);

  int16_t gap = (h >= 6) ? 2 : 1;
  int16_t iw = w - 2 * gap;
  int16_t ih = h - 2 * gap;
  if (iw <= 0 || ih <= 0) return;

  int16_t fillW = iround(iw * percent / 100.0f);
  if (fillW > 0)
    d.fillRect(x + gap, y + gap, fillW, ih, color);
}

void gauge(Adafruit_GFX &d, int16_t cx, int16_t cy, int16_t r,
           float value, float minValue, float maxValue,
           uint8_t ticks, uint16_t color)
{
  arc(d, cx, cy, r, 2, -90, 90, color);

  for (uint8_t i = 0; ticks > 0 && i <= ticks; i++)
  {
    float a = (-90.0f + 180.0f * i / ticks) * DEG2RAD;
    int16_t x0 = cx + iround((r - 5) * sinf(a));
    int16_t y0 = cy - iround((r - 5) * cosf(a));
    int16_t x1 = cx + iround(r * sinf(a));
    int16_t y1 = cy - iround(r * cosf(a));
    d.drawLine(x0, y0, x1, y1, color);
  }

  float span = maxValue - minValue;
  float f = (span != 0) ? (value - minValue) / span : 0;
  if (f < 0) f = 0;
  if (f > 1) f = 1;

  float a = (-90.0f + 180.0f * f) * DEG2RAD;
  int16_t nl = (r > 12) ? (r - 8) : (r / 2);

  thickLine(d, cx, cy, cx + iround(nl * sinf(a)), cy - iround(nl * cosf(a)), 2, color);
  d.fillCircle(cx, cy, 3, color);
}

// ------------------------------------------------------------
//  Иконки
// ------------------------------------------------------------

void batteryIcon(Adafruit_GFX &d, int16_t x, int16_t y, float percent,
                 int16_t w, int16_t h, uint16_t color)
{
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;

  int16_t nubW = w / 8;
  if (nubW < 2) nubW = 2;
  int16_t bw = w - nubW;

  d.drawRect(x, y, bw, h, color);
  d.fillRect(x + bw, y + h / 4, nubW, h - 2 * (h / 4), color);

  int16_t gap = (h >= 7) ? 2 : 1;
  int16_t iw = bw - 2 * gap;
  int16_t ih = h - 2 * gap;
  if (iw <= 0 || ih <= 0) return;

  int16_t fillW = iround(iw * percent / 100.0f);
  if (fillW > 0)
    d.fillRect(x + gap, y + gap, fillW, ih, color);
}

void wifiIcon(Adafruit_GFX &d, int16_t x, int16_t y, uint8_t level,
              int16_t size, uint16_t color)
{
  if (size < 8) size = 8;

  int16_t R3 = size - 2;
  int16_t cx = x + iround(0.71f * R3) + 1;
  int16_t cy = y + size - 2;
  uint8_t t = (size >= 16) ? (size / 8) : 1;

  d.fillCircle(cx, cy, (size >= 14) ? 2 : 1, color);

  if (level >= 1) arc(d, cx, cy, R3 / 3, t, -45, 45, color);
  if (level >= 2) arc(d, cx, cy, (2 * R3) / 3, t, -45, 45, color);
  if (level >= 3) arc(d, cx, cy, R3, t, -45, 45, color);
}

// Облако в рамке шириной w (высота ~0.63 w). grow - расширение (для белого ореола).
static void cloud(Adafruit_GFX &d, int16_t x, int16_t y, int16_t w, uint16_t color, int16_t grow)
{
  float fw = w;
  int16_t r1 = iround(0.18f * fw) + grow;
  int16_t r2 = iround(0.24f * fw) + grow;
  int16_t r3 = iround(0.17f * fw) + grow;

  d.fillCircle(x + iround(0.28f * fw), y + iround(0.45f * fw), r1, color);
  d.fillCircle(x + iround(0.50f * fw), y + iround(0.36f * fw), r2, color);
  d.fillCircle(x + iround(0.74f * fw), y + iround(0.46f * fw), r3, color);

  d.fillRect(x + iround(0.28f * fw),
             y + iround(0.45f * fw) - grow,
             iround(0.46f * fw),
             iround(0.18f * fw) + 2 * grow,
             color);
}

static void sun(Adafruit_GFX &d, int16_t cx, int16_t cy, int16_t r, uint16_t color)
{
  if (r < 2) r = 2;
  d.fillCircle(cx, cy, r, color);

  uint8_t t = (r >= 8) ? 2 : 1;
  for (uint8_t k = 0; k < 8; k++)
  {
    float a = k * 45.0f * DEG2RAD;
    int16_t x0 = cx + iround(1.45f * r * sinf(a));
    int16_t y0 = cy - iround(1.45f * r * cosf(a));
    int16_t x1 = cx + iround(2.0f * r * sinf(a));
    int16_t y1 = cy - iround(2.0f * r * cosf(a));
    thickLine(d, x0, y0, x1, y1, t, color);
  }
}

void weatherIcon(Adafruit_GFX &d, int16_t x, int16_t y, int16_t size,
                 Weather type, uint16_t color)
{
  float s = size;
  uint16_t bg = color ? 0 : 1;
  uint8_t t = (size >= 32) ? 2 : 1;

  switch (type)
  {
    case WEATHER_SUN:
      sun(d, x + size / 2, y + size / 2, iround(0.22f * s), color);
      break;

    case WEATHER_PARTLY:
      sun(d, x + iround(0.33f * s), y + iround(0.30f * s), iround(0.15f * s), color);
      cloud(d, x + iround(0.20f * s), y + iround(0.28f * s), iround(0.80f * s), bg, 2);
      cloud(d, x + iround(0.20f * s), y + iround(0.28f * s), iround(0.80f * s), color, 0);
      break;

    case WEATHER_CLOUD:
      cloud(d, x, y + iround(0.18f * s), size, color, 0);
      break;

    case WEATHER_RAIN:
      cloud(d, x, y, size, color, 0);
      for (uint8_t i = 0; i < 3; i++)
      {
        float dx = 0.30f + 0.20f * i;
        thickLine(d, x + iround(dx * s), y + iround(0.72f * s),
                     x + iround((dx - 0.06f) * s), y + iround(0.94f * s), t, color);
      }
      break;

    case WEATHER_SNOW:
    {
      cloud(d, x, y, size, color, 0);
      int16_t r = size / 16;
      if (r < 1) r = 1;
      d.fillCircle(x + iround(0.30f * s), y + iround(0.76f * s), r, color);
      d.fillCircle(x + iround(0.50f * s), y + iround(0.76f * s), r, color);
      d.fillCircle(x + iround(0.70f * s), y + iround(0.76f * s), r, color);
      d.fillCircle(x + iround(0.40f * s), y + iround(0.92f * s), r, color);
      d.fillCircle(x + iround(0.60f * s), y + iround(0.92f * s), r, color);
      break;
    }

    case WEATHER_STORM:
      cloud(d, x, y, size, color, 0);
      d.fillTriangle(x + iround(0.60f * s), y + iround(0.64f * s),
                     x + iround(0.40f * s), y + iround(0.84f * s),
                     x + iround(0.55f * s), y + iround(0.84f * s), color);
      d.fillTriangle(x + iround(0.45f * s), y + iround(1.00f * s),
                     x + iround(0.72f * s), y + iround(0.76f * s),
                     x + iround(0.52f * s), y + iround(0.76f * s), color);
      break;
  }
}

// ------------------------------------------------------------
//  Семисегментный шрифт
// ------------------------------------------------------------

// Биты сегментов: a=0x01 b=0x02 c=0x04 d=0x08 e=0x10 f=0x20 g=0x40
static const uint8_t SEG_DIGITS[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static uint8_t segMask(char c)
{
  if (c >= '0' && c <= '9') return SEG_DIGITS[c - '0'];
  switch (c)
  {
    case '-': return 0x40;
    case 'C': case 'c': return 0x39;
    case 'F': case 'f': return 0x71;
    case 'H': case 'h': return 0x76;
    case 'E': case 'e': return 0x79;
    case 'P': case 'p': return 0x73;
    case 'L': case 'l': return 0x38;
    case 'A': case 'a': return 0x77;
    case 'U': case 'u': return 0x3E;
    default: return 0;
  }
}

// Рисует (если d != nullptr) один символ и возвращает его ширину
static int16_t segGlyph(Adafruit_GFX *d, int16_t x, int16_t y, int16_t h, uint8_t t,
                        char c, uint16_t color)
{
  int16_t w = h / 2;

  if (c == '.')
  {
    if (d) d->fillRect(x, y + h - t, t, t, color);
    return t;
  }
  if (c == ':')
  {
    if (d)
    {
      d->fillRect(x, y + h / 4, t, t, color);
      d->fillRect(x, y + (h * 3) / 4 - t, t, t, color);
    }
    return t;
  }
  if (c == '*')
  {
    int16_t s = 3 * t;
    if (d)
      for (uint8_t i = 0; i < t; i++)
        d->drawRect(x + i, y + i, s - 2 * i, s - 2 * i, color);
    return s;
  }

  uint8_t m = segMask(c);
  if (m == 0) return w / 2;   // пробел и неизвестные символы

  if (d)
  {
    int16_t gY  = y + h / 2 - t / 2;       // верх средней перемычки
    int16_t upY = y + t;
    int16_t upL = gY - upY;                // длина верхних вертикалей
    int16_t loY = gY + t;
    int16_t loL = (y + h - t) - loY;       // длина нижних вертикалей
    int16_t hx = x + t, hw = w - 2 * t;

    if (m & 0x01) d->fillRect(hx, y, hw, t, color);               // a
    if (m & 0x40) d->fillRect(hx, gY, hw, t, color);              // g
    if (m & 0x08) d->fillRect(hx, y + h - t, hw, t, color);       // d
    if (m & 0x20) d->fillRect(x, upY, t, upL, color);             // f
    if (m & 0x02) d->fillRect(x + w - t, upY, t, upL, color);     // b
    if (m & 0x10) d->fillRect(x, loY, t, loL, color);             // e
    if (m & 0x04) d->fillRect(x + w - t, loY, t, loL, color);     // c
  }
  return w;
}

static uint8_t segThickness(int16_t h, uint8_t thickness)
{
  if (thickness) return thickness;
  int16_t t = h / 10;
  return (uint8_t)(t < 1 ? 1 : t);
}

static int16_t segText(Adafruit_GFX *d, int16_t x, int16_t y, int16_t h, const char *text,
                       uint8_t thickness, uint16_t color)
{
  if (!text || h < 5) return 0;

  uint8_t t = segThickness(h, thickness);
  int16_t sp = (t < 2) ? 2 : t;
  int16_t cx = x;

  for (const char *p = text; *p; p++)
  {
    cx += segGlyph(d, cx, y, h, t, *p, color) + sp;
  }

  return (cx > x) ? (cx - x - sp) : 0;
}

int16_t sevenSegment(Adafruit_GFX &d, int16_t x, int16_t y, int16_t h, const char *text,
                     uint8_t thickness, uint16_t color)
{
  return segText(&d, x, y, h, text, thickness, color);
}

int16_t sevenSegmentWidth(int16_t h, const char *text, uint8_t thickness)
{
  return segText(nullptr, 0, 0, h, text, thickness, GTAG6_BLACK);
}

// ------------------------------------------------------------
//  Текст
// ------------------------------------------------------------

int16_t textWidth(Adafruit_GFX &d, const char *text, uint8_t size)
{
  if (!text) return 0;
  int16_t x1, y1;
  uint16_t w, h;
  d.setTextSize(size);
  d.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (int16_t)w;
}

void textBox(Adafruit_GFX &d, const char *text, int16_t x, int16_t y, int16_t w, int16_t h,
             GTag6Align align, uint8_t size, uint16_t color)
{
  if (!text) return;

  char buf[64];
  strncpy(buf, text, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = 0;

  d.setTextSize(size);
  d.setTextColor(color);

  int16_t x1, y1;
  uint16_t tw, th;
  d.getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);

  size_t len = strlen(buf);
  while (len > 0 && (int16_t)tw > w - 2 * GTAG6_TEXT_PAD)
  {
    buf[--len] = 0;
    d.getTextBounds(buf, 0, 0, &x1, &y1, &tw, &th);
  }

  int16_t px;
  switch (align)
  {
    case GTAG6_ALIGN_LEFT:  px = x + GTAG6_TEXT_PAD; break;
    case GTAG6_ALIGN_RIGHT: px = x + w - GTAG6_TEXT_PAD - (int16_t)tw; break;
    default:                px = x + (w - (int16_t)tw) / 2; break;
  }
  int16_t py = y + (h - (int16_t)th) / 2;

  d.setCursor(px - x1, py - y1);
  d.print(buf);
}

static void emitLine(Adafruit_GFX &d, char *line, size_t &len, int16_t x, int16_t &cy, int16_t lineH)
{
  d.setCursor(x, cy);
  d.print(line);
  cy += lineH;
  len = 0;
  line[0] = 0;
}

int16_t wrappedText(Adafruit_GFX &d, const char *text, int16_t x, int16_t y, int16_t w,
                    uint8_t size, uint16_t color, int16_t lineH)
{
  if (!text) return y;
  if (lineH <= 0) lineH = 8 * size + 2;

  d.setTextSize(size);
  d.setTextColor(color);

  char line[96];
  char word[48];
  char cand[160];
  size_t len = 0;
  line[0] = 0;
  int16_t cy = y;
  const char *p = text;

  while (*p)
  {
    if (*p == '\n')
    {
      emitLine(d, line, len, x, cy, lineH);
      p++;
      continue;
    }
    if (*p == ' ')
    {
      p++;
      continue;
    }

    size_t wl = 0;
    while (*p && *p != ' ' && *p != '\n' && wl < sizeof(word) - 1)
      word[wl++] = *p++;
    word[wl] = 0;

    if (len > 0)
      snprintf(cand, sizeof(cand), "%s %s", line, word);
    else
      snprintf(cand, sizeof(cand), "%s", word);

    int16_t x1, y1;
    uint16_t tw, th;
    d.getTextBounds(cand, 0, 0, &x1, &y1, &tw, &th);

    if ((int16_t)tw > w && len > 0)
    {
      emitLine(d, line, len, x, cy, lineH);
      strncpy(line, word, sizeof(line) - 1);
    }
    else
    {
      strncpy(line, cand, sizeof(line) - 1);
    }
    line[sizeof(line) - 1] = 0;
    len = strlen(line);
  }

  if (len > 0)
    emitLine(d, line, len, x, cy, lineH);

  return cy;
}

}  // namespace GTag6UI


// ============================================================
//  GTag6Chart
// ============================================================

GTag6Chart::GTag6Chart(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t points)
  : _x(x), _y(y), _w(w), _h(h)
{
  uint16_t cap = points ? points : (uint16_t)(w > 2 ? w : 2);
  if (cap > GTAG6_CHART_MAX_POINTS) cap = GTAG6_CHART_MAX_POINTS;
  if (cap < 2) cap = 2;
  _cap = cap;
}

void GTag6Chart::push(float value)
{
  _data[_head] = value;
  _head = (_head + 1) % _cap;
  if (_count < _cap) _count++;
}

void GTag6Chart::clear()
{
  _count = 0;
  _head = 0;
}

float GTag6Chart::get(uint16_t i) const
{
  if (_count == 0) return 0;
  if (i >= _count) i = _count - 1;
  return _data[((uint32_t)_head + _cap - _count + i) % _cap];
}

float GTag6Chart::last() const
{
  return _count ? get(_count - 1) : 0;
}

void GTag6Chart::setAutoRange(bool on) { _auto = on; }

void GTag6Chart::setRange(float lo, float hi)
{
  _auto = false;
  _lo = lo;
  _hi = hi;
}

void GTag6Chart::setGrid(bool on, uint8_t divisions)
{
  _grid = on;
  _div = divisions < 2 ? 2 : divisions;
}

void GTag6Chart::setLabels(bool on, uint8_t decimals)
{
  _labels = on;
  _dec = decimals;
}

void GTag6Chart::setStyle(GTag6ChartStyle style) { _style = style; }

void GTag6Chart::draw(Adafruit_GFX &d, uint16_t color)
{
  // Диапазон по Y
  float lo = _lo, hi = _hi;
  if (_auto)
  {
    if (_count == 0)
    {
      lo = 0;
      hi = 1;
    }
    else
    {
      lo = hi = get(0);
      for (uint16_t i = 1; i < _count; i++)
      {
        float v = get(i);
        if (v < lo) lo = v;
        if (v > hi) hi = v;
      }
    }
  }
  if (hi - lo < 1e-6f) hi = lo + 1.0f;

  // Подписи
  int16_t margin = 0;
  char sHi[16], sLo[16];
  if (_labels)
  {
    snprintf(sHi, sizeof(sHi), "%.*f", _dec, hi);
    snprintf(sLo, sizeof(sLo), "%.*f", _dec, lo);

    d.setTextSize(1);
    d.setTextColor(color);

    int16_t x1, y1;
    uint16_t wHi, wLo, th;
    d.getTextBounds(sHi, 0, 0, &x1, &y1, &wHi, &th);
    d.getTextBounds(sLo, 0, 0, &x1, &y1, &wLo, &th);

    margin = (int16_t)(wHi > wLo ? wHi : wLo) + 3;

    d.setCursor(_x, _y);
    d.print(sHi);
    d.setCursor(_x, _y + _h - 8);
    d.print(sLo);
  }

  int16_t px = _x + margin;
  int16_t pw = _w - margin;
  int16_t ph = _h;
  if (pw < 8 || ph < 8) return;

  d.drawRect(px, _y, pw, ph, color);

  // Сетка
  if (_grid)
  {
    for (uint8_t k = 1; k < _div; k++)
    {
      int16_t yy = _y + (int16_t)((int32_t)k * ph / _div);
      for (int16_t xx = px + 2; xx < px + pw - 2; xx += 3)
        d.drawPixel(xx, yy, color);
    }
  }

  if (_count < 1) return;

  int16_t iw = pw - 3;                                  // X: px+1 .. px+pw-2
  float step = (_cap > 1) ? (float)iw / (_cap - 1) : 0;
  float span = hi - lo;
  int16_t bottom = _y + ph - 2;                         // нижняя строка внутри рамки

  int16_t prevX = 0, prevY = 0;

  for (uint16_t i = 0; i < _count; i++)
  {
    float f = (get(i) - lo) / span;
    if (f < 0) f = 0;
    if (f > 1) f = 1;

    int16_t xx = px + 1 + iw - iround((_count - 1 - i) * step);
    int16_t yy = bottom - iround(f * (ph - 3));

    switch (_style)
    {
      case GTAG6_CHART_LINE:
        if (i > 0) d.drawLine(prevX, prevY, xx, yy, color);
        else d.drawPixel(xx, yy, color);
        break;

      case GTAG6_CHART_AREA:
        d.drawFastVLine(xx, yy, bottom - yy + 1, color);
        break;

      case GTAG6_CHART_BARS:
      {
        int16_t bw = (int16_t)step;
        if (bw > 2) bw -= 1;
        if (bw < 1) bw = 1;
        int16_t bx = xx - bw + 1;
        if (bx < px + 1) bx = px + 1;
        d.fillRect(bx, yy, xx - bx + 1, bottom - yy + 1, color);
        break;
      }
    }

    prevX = xx;
    prevY = yy;
  }
}
