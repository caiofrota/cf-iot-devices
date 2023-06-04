/**
 * CFDisplayHelper.h
 *
 * A library for Arduino that helps to print display for CF IoT devices.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Sep, 2021
 */

#include <CFIconSet.h>           // CF Icon Set for display.
#include <CFDisplayHelper.h>  // CF IoT display helper.

/**
 * Constructor.
 *
 * @param width Display width.
 * @param height Display height.
 * @param addr Display address.
 */
CFDisplayHelper::CFDisplayHelper(int width, int height, int addr) : _display(width, height, &Wire, -1),
                                                                          _width(width),
                                                                          _height(height),
                                                                          _address(addr),
                                                                          _showLogo(true),
                                                                          _logoTime(3000) {
}

/**
 * Initialize.
 */
void CFDisplayHelper::begin() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally.
  if (!_display.begin(SSD1306_SWITCHCAPVCC, _address)) {
    Logger::fatal("SSD1306 allocation failed");
    for (;;)
      ;  // Don't proceed, loop forever.
  }

  // Display logo.
  if (_showLogo) {
    if (_width == 128 && _height == 64) {
      _display.clearDisplay();
      _display.drawBitmap(0, 0, CFIconSet::CFLOGO_128X64, 128, 64, 1);
      _display.display();
      delay(_logoTime);
    }
  }

  // Clear display.
  _display.clearDisplay();
  _display.cp437(true);
  _display.setTextSize(1);
  _display.setTextColor(WHITE);
}

/**
 * Render display.
 */
void CFDisplayHelper::display() {
  _display.display();
}

/**
 * Clear display.
 */
void CFDisplayHelper::clearDisplay() {
  _display.clearDisplay();
}

/**
 * Set cursor position.
 *
 * @param col Column.
 * @param lin Line.
 */
void CFDisplayHelper::setCursor(int col, int lin) {
  _display.setCursor(col, lin);
}

/**
 * Print what should be rendered.
 *
 * @param text Text.
 */
void CFDisplayHelper::print(String text) {
  _display.print(text);
}

/**
 * Draw bitmap.
 *
 * @param x column.
 * @param y line.
 * @param bmap Bitmap.
 * @param w Width.
 * @param y Height.
 * @param color Color.
 */
void CFDisplayHelper::drawBitmap(int x, int y, const unsigned char bmap[], int w, int h, int color) {
  _display.drawBitmap(x, y, bmap, w, h, color);
}