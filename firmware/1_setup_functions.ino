// MARK: - Setup Functions

void setupPWM() {
  pinMode(WAVEFORM_PIN, OUTPUT);
  // Set Pin 11 (WAVEFORM_PIN) PWM frequency to 31372.55 Hz, instead of default 490.20 Hz.
  TCCR2B = TCCR2B & B11111000 | B00000001;
}

void setupTimer() {
  ITimer1.init();
}

void setupSerial() {
  Serial.begin(115200);
  Serial.print("READY");
}

// MARK: - RGB LED

void setupLED() {
  if (RGB_LED_IS_USED) {
    pinMode(RED_LIGHT_PIN, OUTPUT);
    pinMode(GREEN_LIGHT_PIN, OUTPUT);
    pinMode(BLUE_LIGHT_PIN, OUTPUT);
  }
}

void setRGBColor(Color color) {
  if (RGB_LED_IS_USED) {
    analogWrite(RED_LIGHT_PIN, color.red);
    analogWrite(GREEN_LIGHT_PIN, color.green);
    analogWrite(BLUE_LIGHT_PIN, color.blue);
  }
}

// MARK: - Button

void setupButton() {
  if (BUTTON_IS_USED) {
    pinMode(BUTTON_PIN, BUTTON_PIN_MODE);
  }
}

void handleButton() {
  int reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    byte state = buttonState;
    buttonState = reading;
    if (reading != state && reading == HIGH) {
      buttonPressed();
    }
  }
  lastButtonState = reading;
}

void buttonPressed() {
  Serial.println("Button pressed");
  if (protocolInProgress) {
    cancelProtocol();
  } else {
    runProtocol();
  }
}
