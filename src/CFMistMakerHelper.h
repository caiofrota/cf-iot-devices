/**
 * CFMistMakerHelper.h
 *
 * A library for Arduino that helps to integrate with mist maker modules.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Sep, 2021
 */

#ifndef CFMistMakerHelper_h
#define CFMistMakerHelper_h

#include <Arduino.h>          // Arduino library.
#include <CFVirtualButton.h>  // CF Virtual Button.

class CFMistMakerHelper {
 private:
  // Aliases.
  using VoidCallback = void (*)(int status);  // Alias for callback.

  // Control attributes.
  CFVirtualButton _button;  // Virtual Button.
  int _pinStatus;           // Status pin.
  int _lastChange;          // Last time status was changed.
  bool _changeStatus;       // Last status.
  int _lastStatus;          // Last status.

  // Methods.
  int _readStatus();  // Read the status.

  // Available callbacks.
  VoidCallback _onStatusChangeCallback;  // On status change callback.

 public:
  // Methods.
  CFMistMakerHelper(int pinButton);                    // Constructor.
  CFMistMakerHelper(int pinButton, int pinStatus);     // Constructor with status pin.
  void begin();                                        // Initialize.
  void loop();                                         // Loop.
  int getStatus();                                     // Get the status.
  void toggle();                                       // Toggle mist maker status.
  void turnOn();                                       // Turn the mist maker on.
  void turnOff();                                      // Turn the mist maker off.
  void setOnStatusChangeCallback(const VoidCallback);  // Define on status change callback.
};

#endif
