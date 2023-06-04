/**
 * CFVirtualButton.h
 *
 * A library for Arduino that manage a virtual button using an output pin.
 *
 * @author  Caio Frota <caiofrota@gmail.com>
 * @version 1.0
 * @since   Nov, 2022
 */

#include <CFVirtualButton.h>  // CF Virtual Button.

CFVirtualButton::CFVirtualButton(int pinButton) : _pinButton(pinButton),
                                                  _defaultStatus(LOW),
                                                  _status(LOW),
                                                  _lastChange(0) {
}

CFVirtualButton::CFVirtualButton(int pinButton, int defaultStatus) : _pinButton(pinButton),
                                                                     _defaultStatus(defaultStatus),
                                                                     _status(defaultStatus),
                                                                     _lastChange(0) {
}

void CFVirtualButton::begin() {
  pinMode(_pinButton, OUTPUT);
  digitalWrite(_pinButton, _defaultStatus);
}

void CFVirtualButton::loop() {
  if (_status != _defaultStatus) {
    _setStatus(_defaultStatus);
  }
}

void CFVirtualButton::push() {
  _setStatus((_defaultStatus == HIGH) ? LOW : HIGH);
}

void CFVirtualButton::_setStatus(int status) {
  if ((millis() - _lastChange) > 100) {
    digitalWrite(_pinButton, status);
    _status = status;
    _lastChange = millis();
  }
}
