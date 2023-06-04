/**
 * CFDisplayHelper.h
 *
 * A library for Arduino that helps to print display for CF IoT devices.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Sep, 2021
 */

#ifndef CFDisplayHelper_h
#define CFDisplayHelper_h

#include <Adafruit_GFX.h>      // Adafruit GFX.
#include <Adafruit_SSD1306.h>  // Adafruit display.
#include <Arduino.h>           // Arduino library.
#include <Logger.h>            // Logger.
#include <Wire.h>              // Wire.

class CFDisplayHelper {
 private:
  // Display attributes.
  Adafruit_SSD1306 _display;  // Display object.
  int _width;                 // Display width.
  int _height;                // Display height.
  int _address;               // Display address.
  bool _showLogo;             // Flag that indicates if it's to show the logo.
  unsigned long _logoTime;    // Time that will show the logo.

 public:
  CFDisplayHelper(int width, int height, int addr);          // Constructor.
  void begin();                                              // Initialize.
  void display();                                            // Render display.
  void clearDisplay();                                       // Clear display.
  void setCursor(int col, int lin);                          // Set cursor position.
  void print(String text);                                   // Print what should be rendered.
  void drawBitmap(int x, int y, const unsigned char bmap[],  // Draw bitmap.
                  int w, int h, int color);
};

#endif