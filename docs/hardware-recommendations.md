# Hardware Recommendations & 3D Print Enclosures

This guide breaks down the recommended hardware options for both initial units: the **Desktop Lab & Bambu 3D Monitor** and the **DeskBuddy Cyber-Pet**.

---

## Unit 1: Desktop Lab & Bambu 3D Monitor (`desk-dashboard`)

An all-in-one display module is strongly recommended. They eliminate breadboard wiring, keep the footprint compact, and have abundant ready-to-print cases on MakerWorld and Printables.

| Hardware Option | Screen Size / Tech | Processor | Est. Cost | Best For | 3D Print Case Availability |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **"CYD" ESP32-2432S028R** *(Recommended)* | 2.8" TFT (320x240) + Touch | ESP32-D0WDQ6-V3 (Dual Core) | ~$12 - $15 | Highest value, all-in-one with RGB LED, speaker port, SD card, USB-C | Hundreds of designs on Bambu MakerWorld |
| **LilyGO T-Display-S3** | 1.9" IPS (170x320, 8-bit parallel) | ESP32-S3 (Dual Core, 8MB PSRAM, 16MB Flash) | ~$18 - $22 | Sleek, compact desk widget with vibrant IPS viewing angles | Excellent mini monitor & retro TV cases |
| **M5Stack CoreS3** | 2.0" IPS Capacitive Touch | ESP32-S3 + Camera, Mic, Speaker, Battery | ~$45 - $55 | Premium plug-and-play, no 3D printing needed unless adding a stand | Base includes magnet/DIN mount |
| **Waveshare ESP32-S3 3.5" or 4.3"** | 3.5" or 4.3" IPS Touch (480x320 or 800x480) | ESP32-S3 (16MB Flash, 8MB PSRAM) | ~$25 - $35 | Large desk screen with plenty of room for multi-charts | Desk stand STLs on Printables |

### Recommended Choice: The "CYD" (Cheap Yellow Display) or LilyGO T-Display-S3
- **Why CYD?** At ~$12-$15, it contains everything on one single PCB: ESP32, 2.8" display, resistive touch controller (XPT2046), micro-SD slot, addressable RGB LED, and audio amplifier.
- Search term: *"ESP32-2432S028R"* on Amazon, AliExpress, or eBay. (Ensure you pick the USB-C version).

---

## Unit 2: DeskBuddy Cyber-Pet / Agent Status (`desk-buddy`)

The goal of this unit is personality, expressive eyes/emojis, and fun interaction for kids or your desk.

| Hardware Option | Display Style | Board | Est. Cost | Experience |
| :--- | :--- | :--- | :--- | :--- |
| **Dual Round Displays (GC9A01)** *(Most Expressive!)* | 2x 1.28" Round LCDs (240x240 each) | ESP32 or ESP32-S3 | ~$15 - $20 | Animated round eyes that look around, blink, and squint like Cozmo/Vector robots! |
| **Single Round LCD (GC9A01 / Waveshare ESP32-S3-Round)** | 1x 1.28" Round LCD (240x240) | ESP32-S3 integrated | ~$15 - $18 | Compact desktop "orb" or pocket monster companion. |
| **0.96" or 1.3" I2C Monochrome OLED (SSD1306 / SH1106)** | Retro Blue/White OLED | Generic ESP32 / ESP32-C3 | ~$6 - $10 | Super cheap, retro cyberpunk aesthetic, very quick to wire up. |
| **Reusing CYD or LilyGO T-Display-S3** | Color TFT / IPS | ESP32 / ESP32-S3 | ~$12 - $18 | Full color expressions, animated emojis, and text speech bubbles! |

### Recommended Choice:
- If ordering hardware: **Waveshare ESP32-S3 Round LCD (1.28")** or an **ESP32 + dual GC9A01 round screens** creates an irresistible robotic eyeball look that kids adore.
- Alternatively, you can run the DeskBuddy firmware right on a CYD or 0.96" OLED!

---

## 3D Printing Recommendations (Bambu Lab)

Print these using your Bambu Lab printer (PLA or PETG):
1. **For CYD (ESP32-2432S028R)**:
   - *MakerWorld search*: "CYD case", "Cheap Yellow Display desktop stand", or "CYD Retro TV".
   - Popular design: [CYD Desktop Stand with tilt](https://makerworld.com/search/models?keyword=CYD+case)
2. **For LilyGO T-Display-S3**:
   - *MakerWorld search*: "T-Display-S3 case" or "T-Display Retro Monitor".
3. **For DeskBuddy Cyber-Pet**:
   - *Robot / Pet Enclosures*: Search MakerWorld for "Desktop Robot Eyes", "BMO case", or "Mini Robot companion".
