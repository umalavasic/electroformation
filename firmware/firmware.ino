// WAVEFORM
#define WAVEFORM_PIN              11           // Don't change unless you also adjust timers and PWM frequency accordingly
#define SIGNAL_VOLTAGE_AMPLITUDE  5            // [V]
#define PROTOCOL_MAX_STAGES       10           // Currently set to accept 10 stages in single protocol

// BUTTON
#define BUTTON_IS_USED            true
#define BUTTON_PIN                2
#define BUTTON_PIN_MODE           INPUT_PULLUP // change to INPUT when using external resistors

// RGB LED
#define RGB_LED_IS_USED           true
#define RED_LIGHT_PIN             3
#define GREEN_LIGHT_PIN           6
#define BLUE_LIGHT_PIN            5

// TIMER
#define USE_TIMER_1               true
#include <TimerInterrupt.h>
#include <ISR_Timer.h>

// EEPROM
#include <EEPROM.h>

// MARK: - Signal Generation

const byte sineWave[] = {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179, 176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 93, 90, 88, 85, 82, 79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 40, 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11, 10, 9, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9, 10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76, 79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124
  // https://www.daycounter.com/Calculators/Sine-Generator-Calculator.phtml
};
const unsigned waveformResolution = sizeof(sineWave) / sizeof(sineWave[0]); // = 256
byte waveform[waveformResolution];
unsigned waveformPosition;

// MARK: - Protocol

struct Protocol {
  unsigned long duration; // [s]
  float frequency;        // [Hz], 0.0 < float frequency
  float amplitude;        // [V],  0.0 < float amplitude < SIGNAL_VOLTAGE_AMPLITUDE
};
Protocol protocol[PROTOCOL_MAX_STAGES];
byte protocolStages;
bool protocolInProgress = false;

// MARK: - Serial Communication

const unsigned messageSize = 512; // Maximum amount of characters in one serial port message
char receivedMessage[messageSize];
bool receivedProtocol = false;

// MARK: - Button

byte buttonState = HIGH, lastButtonState = HIGH;
unsigned long lastDebounceTime, debounceDelay = 50; // debounce time; increase if the output flickers

// MARK: - RGB LED

struct Color {
  byte red, green, blue;
};

const struct Colors {
  Color red = {255, 0, 0};
  Color green = {0, 255, 0};
  Color blue = {0, 0, 255};
  Color cyan = {0, 255, 255};
  Color magenta = {255, 0, 255};
  Color yellow = {255, 75, 0};
  Color white = {255, 255, 255};
} colors;

// MARK: - Life Cycle

void setup() {
  setupPWM();
  setupTimer();
  setupSerial();
  setupButton();
  setupLED();

  loadProtocol();
  exportProtocol();

  setRGBColor(colors.cyan);
}

void loop() {
  if (!protocolInProgress) {
    receiveProtocol();
    if (receivedProtocol) {
      receivedProtocolMessage();
      receivedProtocol = false;
    }
  }

  if (BUTTON_IS_USED) {
    handleButton();
  }
}
