/**
 * CFVirtualButton.h
 *
 * A library for Arduino that manage a virtual button using an output pin.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2022
 */

#ifndef CFVirtualButton_h
#define CFVirtualButton_h

#include <Arduino.h>  // Arduino library.

class CFVirtualButton {
 private:
  // Virtual Button attributes.
  int _pinButton;      // Button pin.
  int _defaultStatus;  // Default status.
  int _status;         // Current status.
  int _lastChange;     // Last time status was changed.

  // Methods.
  void _setStatus(int status);  // Define a new status.

 public:
  CFVirtualButton(int _pinButton);                     // Constructor.
  CFVirtualButton(int _pinButton, int defaultStatus);  // Constructor with default status.
  void begin();                                        // Initialize.
  void loop();                                         // Loop.
  void push();                                         // Push button.
};

#endif
