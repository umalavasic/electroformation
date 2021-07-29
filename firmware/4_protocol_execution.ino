void runProtocol() {
  protocolInProgress = true;
  setRGBColor(colors.magenta);
  for (int i = 0; i < protocolStages; i++) {
    if (!protocolInProgress) {
      return; // Protocol has been canceled.
    }
    Protocol stage = protocol[i];
    runStage(stage);
  }
  protocolFinished(true);
}

void runStage(Protocol stage) {
  unsigned long duration = stage.duration;
  float frequency = stage.frequency;
  float amplitude = stage.amplitude;

  // 1. Populate 'waveform' array with sine wave values modulated with amplitude.
  float scaler = amplitude / SIGNAL_VOLTAGE_AMPLITUDE;
  for (int i = 0; i < waveformResolution; i++) {
    waveform[i] = byte(round(sineWave[i] * scaler));
  }

  // 2. Start the timer interrupt process.
  float timerFrequency = frequency * 2 * waveformResolution;
  ITimer1.attachInterrupt(timerFrequency, tick);

  // 3. Wait for the duration of the sequence, then stop the timer interrupt process.
  unsigned long startTime = millis(), currentTime = millis();
  while (currentTime - startTime < duration * 1000) {
    if (BUTTON_IS_USED) {
      handleButton();
    }
    currentTime = millis();
  }

  ITimer1.disableTimer();
}

void tick() {
  byte instantaneousValue = waveform[waveformPosition];
  analogWrite(WAVEFORM_PIN, instantaneousValue);
  waveformPosition = (waveformPosition + 1) % waveformResolution;
  // Serial.println(instantaneousValue); -- to check sinewave using serial plotter
}

void cancelProtocol() {
  ITimer1.disableTimer();
  waveformPosition = 0;
  protocolFinished(false);
}

void protocolFinished(bool finished) {
  protocolInProgress = false;
  if (finished) {
    setRGBColor(colors.green);
  }
  else {
    setRGBColor(colors.red);
  }
  analogWrite(WAVEFORM_PIN, 0);
}
