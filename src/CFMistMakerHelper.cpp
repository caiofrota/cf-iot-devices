/**
 * CFMistMakerHelper.h
 *
 * A library for Arduino that helps to integrate with mist maker modules.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Sep, 2021
 */

// Libraries.
#include <CFMistMakerHelper.h>  // CF Mist Maker.

/**
 * Constructor.
 */
CFMistMakerHelper::CFMistMakerHelper(int pinButton) : _button(pinButton, HIGH),
                                                      _pinStatus(-1),
                                                      _changeStatus(false),
                                                      _lastStatus(0) {
}

/**
 * Constructor with status pin.
 */
CFMistMakerHelper::CFMistMakerHelper(int pinButton, int pinStatus) : _button(pinButton, HIGH),
                                                                     _pinStatus(pinStatus),
                                                                     _changeStatus(false),
                                                                     _lastStatus(0) {
}

/**
 * Initialize.
 */
void CFMistMakerHelper::begin() {
  // Initialize button.
  _button.begin();

  // Initialize status pin, if it's defined.
  if (_pinStatus >= 0) pinMode(_pinStatus, INPUT);
}

/**
 * Loop.
 */
void CFMistMakerHelper::loop() {
  _button.loop();  // Do button loop.

  // Check if status has changed.
  int status = _readStatus();
  if (_changeStatus) {
    if ((millis() - _lastChange) > 1000) {
      Serial.println(_changeStatus);
      _button.push();
      _lastChange = millis();
      Serial.println("Changing");
    } else if (_lastStatus == status) {
      _lastStatus = _readStatus();
      _changeStatus = false;
    }
  } else if (_lastStatus != status && _onStatusChangeCallback) {
    _lastStatus = status;
    _onStatusChangeCallback(_lastStatus);
  }
}

/**
 * Read the status if status pin is available.
 *
 * @return 1 if it's on.
 */
int CFMistMakerHelper::_readStatus() {
  if (_pinStatus >= 0) {
    for (int i = 0; i < 50; i++) {
      if (digitalRead(_pinStatus) > 0) return 1;
      delay(10);
    }
  }
  return 0;
}

/**
 * Get the last status read.
 *
 * @return 1 if it's on.
 */
int CFMistMakerHelper::getStatus() {
  return _lastStatus;
}

/**
 * Turn the mist maker on.
 */
void CFMistMakerHelper::turnOn() {
  _lastStatus = true;
  _changeStatus = true;
}

/**
 * Turn the mist maker off.
 */
void CFMistMakerHelper::turnOff() {
  _lastStatus = false;
  _changeStatus = true;
}

/**
 * Define on status change callback.
 *
 * @param callback Callback.
 */
void CFMistMakerHelper::setOnStatusChangeCallback(VoidCallback callback) {
  _onStatusChangeCallback = callback;
}
