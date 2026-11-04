#include "pet_face.h"

PetFace::PetFace(Adafruit_SSD1306& disp) : display(disp) {}

void PetFace::begin() {
    display.clearDisplay();
    nextBlinkMs = millis() + random(1000, 4000);
    lastLookChangeMs = millis() + random(1500, 3000);
}

void PetFace::setMood(PetMood newMood, const char* speechBubble, uint32_t durationMs) {
    currentMood = newMood;
    message = (speechBubble != nullptr) ? String(speechBubble) : "";

    if (durationMs > 0) {
        moodExpiresMs = millis() + durationMs;
    } else {
        moodExpiresMs = 0;
    }
}

void PetFace::updateBlinking() {
    uint32_t now = millis();

    if (currentMood == PetMood::SLEEPING) {
        isBlinking = true;
        blinkProgress = 100;
        return;
    }

    if (!isBlinking && now >= nextBlinkMs) {
        isBlinking = true;
        blinkStartMs = now;
    }

    if (isBlinking) {
        uint32_t elapsed = now - blinkStartMs;
        const uint32_t blinkDuration = 180; // ms

        if (elapsed < blinkDuration / 2) {
            blinkProgress = (elapsed * 200) / blinkDuration;
        } else if (elapsed < blinkDuration) {
            blinkProgress = 100 - (((elapsed - blinkDuration / 2) * 200) / blinkDuration);
        } else {
            isBlinking = false;
            blinkProgress = 0;
            nextBlinkMs = now + random(2000, 6000);
        }
    }
}

void PetFace::updateSaccades() {
    uint32_t now = millis();

    if (currentMood != PetMood::IDLE) {
        lookOffsetX = 0;
        lookOffsetY = 0;
        return;
    }

    if (now >= lastLookChangeMs) {
        int r = random(0, 5);
        if (r == 0) { lookOffsetX = -6; lookOffsetY = 0; }  // Look left
        else if (r == 1) { lookOffsetX = 6; lookOffsetY = 0; } // Look right
        else if (r == 2) { lookOffsetX = 0; lookOffsetY = -4; } // Look up
        else { lookOffsetX = 0; lookOffsetY = 0; } // Center

        lastLookChangeMs = now + random(1500, 4500);
    }
}

void PetFace::renderSpeech() {
    if (message.length() == 0) return;

    // Draw little speech banner at the bottom
    display.fillRect(0, 52, 128, 12, SSD1306_BLACK);
    display.drawFastHLine(0, 51, 128, SSD1306_WHITE);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(4, 54);
    display.print(message);
}

void PetFace::renderEyes() {
    int ly = eyeY + lookOffsetY;
    int lx = leftEyeX + lookOffsetX;
    int rx = rightEyeX + lookOffsetX;

    switch (currentMood) {
        case PetMood::HAPPY:
            // Cute squinty inverted arcs (^ ^)
            for (int t = 0; t < 4; ++t) {
                display.drawCircle(leftEyeX + 13, eyeY + 22 + t, 12, SSD1306_WHITE);
                display.drawCircle(rightEyeX + 13, eyeY + 22 + t, 12, SSD1306_WHITE);
            }
            display.fillRect(leftEyeX, eyeY + 22, 28, 16, SSD1306_BLACK);
            display.fillRect(rightEyeX, eyeY + 22, 28, 16, SSD1306_BLACK);
            break;

        case PetMood::SLEEPING: {
            // Horizontal slit eyes (- -)
            display.fillRoundRect(leftEyeX, eyeY + 16, eyeWidth, 4, 2, SSD1306_WHITE);
            display.fillRoundRect(rightEyeX, eyeY + 16, eyeWidth, 4, 2, SSD1306_WHITE);

            // Floating "Z z z"
            uint32_t now = millis();
            if (now - lastSleepTickMs > 400) {
                lastSleepTickMs = now;
                sleepZOffset = (sleepZOffset + 1) % 16;
            }
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(106, 18 - (sleepZOffset / 2));
            display.print("z");
            display.setCursor(114, 10 - (sleepZOffset / 2));
            display.print("Z");
            break;
        }

        case PetMood::THINKING:
            // One eye squinting, one eye looking up
            display.fillRoundRect(leftEyeX, eyeY + 10, eyeWidth, 12, 4, SSD1306_WHITE); // Squint left
            display.fillRoundRect(rightEyeX, eyeY, eyeWidth, eyeHeight, 8, SSD1306_WHITE); // Wide right
            // Pupil in top corner
            display.fillCircle(rightEyeX + 18, eyeY + 8, 4, SSD1306_BLACK);
            break;

        case PetMood::EXCITED:
            // Heart or wide shining eyes
            display.fillRoundRect(leftEyeX, eyeY - 2, eyeWidth + 2, eyeHeight + 4, 8, SSD1306_WHITE);
            display.fillRoundRect(rightEyeX, eyeY - 2, eyeWidth + 2, eyeHeight + 4, 8, SSD1306_WHITE);
            // Cute double sparkle pupils
            display.fillCircle(leftEyeX + 8, eyeY + 6, 4, SSD1306_BLACK);
            display.fillCircle(leftEyeX + 18, eyeY + 18, 2, SSD1306_BLACK);
            display.fillCircle(rightEyeX + 8, eyeY + 6, 4, SSD1306_BLACK);
            display.fillCircle(rightEyeX + 18, eyeY + 18, 2, SSD1306_BLACK);
            break;

        case PetMood::DIZZY:
            // X X eyes
            display.drawLine(leftEyeX + 2, eyeY + 6, leftEyeX + eyeWidth - 2, eyeY + eyeHeight - 6, SSD1306_WHITE);
            display.drawLine(leftEyeX + 2, eyeY + eyeHeight - 6, leftEyeX + eyeWidth - 2, eyeY + 6, SSD1306_WHITE);
            display.drawLine(rightEyeX + 2, eyeY + 6, rightEyeX + eyeWidth - 2, eyeY + eyeHeight - 6, SSD1306_WHITE);
            display.drawLine(rightEyeX + 2, eyeY + eyeHeight - 6, rightEyeX + eyeWidth - 2, eyeY + 6, SSD1306_WHITE);
            break;

        case PetMood::ALERT:
            // Wide open round eyes with exclamation badge
            display.fillCircle(leftEyeX + 13, eyeY + 16, 14, SSD1306_WHITE);
            display.fillCircle(rightEyeX + 13, eyeY + 16, 14, SSD1306_WHITE);
            display.fillCircle(leftEyeX + 13, eyeY + 16, 4, SSD1306_BLACK);
            display.fillCircle(rightEyeX + 13, eyeY + 16, 4, SSD1306_BLACK);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(58, 2);
            display.print("!");
            break;

        case PetMood::IDLE:
        default: {
            // Normal rounded rectangular eyes with blinking
            int curH = eyeHeight;
            int curY = ly;

            if (blinkProgress > 0) {
                curH = ((100 - blinkProgress) * eyeHeight) / 100;
                if (curH < 2) curH = 2;
                curY = ly + (eyeHeight - curH) / 2;
            }

            display.fillRoundRect(lx, curY, eyeWidth, curH, 7, SSD1306_WHITE);
            display.fillRoundRect(rx, curY, eyeWidth, curH, 7, SSD1306_WHITE);

            // Pupil highlights (only visible if eyes are sufficiently open)
            if (curH > 14) {
                display.fillCircle(lx + 8, curY + 8, 3, SSD1306_BLACK);
                display.fillCircle(rx + 8, curY + 8, 3, SSD1306_BLACK);
            }
            break;
        }
    }
}

void PetFace::update() {
    uint32_t now = millis();

    // Check if temporary mood expired
    if (moodExpiresMs > 0 && now >= moodExpiresMs) {
        currentMood = PetMood::IDLE;
        message = "";
        moodExpiresMs = 0;
    }

    updateBlinking();
    updateSaccades();

    display.clearDisplay();
    renderEyes();
    renderSpeech();
    display.display();
}
