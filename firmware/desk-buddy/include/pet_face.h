#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

enum class PetMood {
    IDLE = 0,
    HAPPY,
    THINKING,
    SLEEPING,
    EXCITED,
    ALERT,
    DIZZY
};

class PetFace {
public:
    PetFace(Adafruit_SSD1306& display);
    void begin();
    void setMood(PetMood newMood, const char* speechBubble = nullptr, uint32_t durationMs = 0);
    PetMood getMood() const { return currentMood; }
    void update();

private:
    void renderEyes();
    void renderSpeech();
    void updateBlinking();
    void updateSaccades();

    Adafruit_SSD1306& display;
    PetMood currentMood = PetMood::IDLE;

    // Eye geometry
    int eyeWidth = 26;
    int eyeHeight = 32;
    int eyeSpacing = 16;
    int leftEyeX = 26;
    int rightEyeX = 74;
    int eyeY = 16;

    // Saccade glance offsets
    int lookOffsetX = 0;
    int lookOffsetY = 0;
    uint32_t lastLookChangeMs = 0;

    // Blinking state
    bool isBlinking = false;
    uint32_t blinkStartMs = 0;
    uint32_t nextBlinkMs = 0;
    int blinkProgress = 0; // 0 (open) to 100 (fully closed)

    // Speech bubble & timed mood
    String message = "";
    uint32_t moodExpiresMs = 0;

    // Sleep animation counter
    int sleepZOffset = 0;
    uint32_t lastSleepTickMs = 0;
};
