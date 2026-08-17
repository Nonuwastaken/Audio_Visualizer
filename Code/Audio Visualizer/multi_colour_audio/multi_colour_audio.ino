#include <driver/i2s.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- RGB MOSFET PINS ---
#define RED_PIN   1
#define GREEN_PIN 3
#define BLUE_PIN  0

// --- I2S MICROPHONE PINS ---
#define I2S_WS    5
#define I2S_SCK   4
#define I2S_SD    6
#define I2S_PORT  I2S_NUM_0

// --- OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SDA_PIN 21
#define SCL_PIN 20

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool oledFound = false;
uint8_t waveBuffer[SCREEN_WIDTH] = {0};

// ==========================================
// --- TWEAK THESE VARIABLES TO FINE-TUNE ---
// ==========================================

// 1. VOLUME THRESHOLDS (Scaled down numbers)
#define NOISE_FLOOR     5000   // Ignore sounds quieter than this
#define MAX_LOUDNESS    100000 // Hits max brightness at this volume

// 2. FADING SPEEDS (0.01 to 1.0)
#define FADE_UP_SPEED   1.0  // Higher = Snappier reaction to loud beats
#define FADE_DOWN_SPEED 0.1  // Lower = Slower, smoother fade out between beats

// ==========================================

float smoothBrightness = 0;

void setupI2S() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };
  const i2s_pin_config_t pin_config = {
    .mck_io_num = I2S_PIN_NO_CHANGE, 
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_start(I2S_PORT);
}

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  analogWrite(RED_PIN, 0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 0);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    oledFound = false;
  } else {
    oledFound = true;
    display.clearDisplay();
    display.display();
  }

  setupI2S();
}

#define OLED_REFRESH_EVERY 3
int loopCount = 0;

void loop() {
  size_t bytesIn = 0;
  int32_t samples[64];

  esp_err_t result = i2s_read(I2S_PORT, &samples, sizeof(samples), &bytesIn, portMAX_DELAY);

  if (result == ESP_OK && bytesIn > 0) {
    int samplesRead = bytesIn / sizeof(int32_t);
    int32_t maxAmplitude = 0;

    for (int i = 0; i < samplesRead; i++) {
      int32_t raw = samples[i];
      
      // Scale down those massive 32-bit values by shifting bits
      raw >>= 14; 
      
      if (abs(raw) > maxAmplitude) {
        maxAmplitude = abs(raw);
      }
    }

    // Calculate the target brightness based on your thresholds
    int targetBrightness = 0;
    if (maxAmplitude > NOISE_FLOOR) {
      targetBrightness = map(maxAmplitude, NOISE_FLOOR, MAX_LOUDNESS, 0, 255);
      targetBrightness = constrain(targetBrightness, 0, 255);
    }

    // Apply the gradual fading logic
    if (targetBrightness > smoothBrightness) {
      smoothBrightness += (targetBrightness - smoothBrightness) * FADE_UP_SPEED;
    } else {
      smoothBrightness += (targetBrightness - smoothBrightness) * FADE_DOWN_SPEED;
    }
    
    // Prevent floating point errors from dropping it below zero
    if (smoothBrightness < 1.0) smoothBrightness = 0;
    
    int brightness = (int)smoothBrightness;

    // --- TWEAKABLE COLOR SETTINGS ---
    // 1. Slow down the overall cycle (higher number = slower cycle)
    float timeShift = millis() / 5000.0; 
    
    // 2. Control how long it "holds" a color. 
    // 1.0 = Constant smooth shifting (no hold).
    // 2.0 = Lingers heavily on pure colors, with fast fades between them.
    float holdFactor = 1.5; 
    // --------------------------------

    // Step A: Generate the base smooth waves (0.0 to 1.0)
    float rRaw = (sin(timeShift) + 1.0) / 2.0; 
    float gRaw = (sin(timeShift + 2.094) + 1.0) / 2.0; 
    float bRaw = (sin(timeShift + 4.188) + 1.0) / 2.0; 

    // Step B: Over-amplify and clamp to flatten the peaks (creates the "linger" effect)
    float rMultiplier = constrain((rRaw - 0.5) * holdFactor + 0.5, 0.0, 1.0);
    float gMultiplier = constrain((gRaw - 0.5) * holdFactor + 0.5, 0.0, 1.0);
    float bMultiplier = constrain((bRaw - 0.5) * holdFactor + 0.5, 0.0, 1.0);

    // Step C: Apply the audio brightness to the flattened waves
    analogWrite(RED_PIN, (int)(brightness * rMultiplier));
    analogWrite(GREEN_PIN, (int)(brightness * gMultiplier));
    analogWrite(BLUE_PIN, (int)(brightness * bMultiplier));

    // Update the OLED display
    if (oledFound) {
      loopCount++;
      if (loopCount >= OLED_REFRESH_EVERY) {
        loopCount = 0;
        memmove(waveBuffer, waveBuffer + 1, SCREEN_WIDTH - 1);
        waveBuffer[SCREEN_WIDTH - 1] = map(brightness, 0, 255, 0, SCREEN_HEIGHT - 1);

        display.clearDisplay();
        for (int x = 0; x < SCREEN_WIDTH; x++) {
          int barHeight = waveBuffer[x];
          int y = (SCREEN_HEIGHT - barHeight) / 2;
          display.drawFastVLine(x, y, barHeight, SSD1306_WHITE);
        }
        display.display();
      }
    }
  }
}