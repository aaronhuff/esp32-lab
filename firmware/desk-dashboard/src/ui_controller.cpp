#include "ui_controller.h"

// Color Palette (RGB565)
#define COLOR_BG          0x0841  // Deep dark navy / slate
#define COLOR_CARD        0x18C3  // Card background
#define COLOR_ACCENT      0x05BF  // Vibrant Cyan
#define COLOR_GREEN       0x07E0  // Bright Green
#define COLOR_RED         0xF800  // Red
#define COLOR_YELLOW      0xFFE0  // Yellow
#define COLOR_WHITE       0xFFFF  // White
#define COLOR_MUTED       0x7BEF  // Light gray

UIController::UIController(TFT_eSPI& tftRef) : tft(tftRef) {}

void UIController::begin() {
    tft.init();
    tft.setRotation(1); // Landscape (320x240)
    tft.fillScreen(COLOR_BG);
}

void UIController::showBootSplash(const char* statusMsg) {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("ESP32 DESK LAB", screenWidth / 2, 70, 4);

    tft.setTextColor(COLOR_WHITE, COLOR_BG);
    tft.drawString("Multi-Dashboard Initializing", screenWidth / 2, 110, 2);

    tft.setTextColor(COLOR_MUTED, COLOR_BG);
    tft.drawString(statusMsg, screenWidth / 2, 160, 2);

    // Subtle accent loading bar outline
    tft.drawRoundRect(40, 190, 240, 10, 5, COLOR_ACCENT);
    tft.fillRoundRect(42, 192, 120, 6, 3, COLOR_ACCENT);
}

void UIController::renderHeader(const char* title) {
    tft.fillRect(0, 0, screenWidth, 32, COLOR_CARD);
    tft.setTextColor(COLOR_ACCENT, COLOR_CARD);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(title, 10, 16, 4);

    // Current WiFi / System indicator
    tft.setTextDatum(MR_DATUM);
    tft.setTextColor(COLOR_GREEN, COLOR_CARD);
    tft.drawString("ONLINE", screenWidth - 10, 16, 2);
    tft.drawFastHLine(0, 32, screenWidth, COLOR_ACCENT);
}

void UIController::renderFooter(DashboardPage currentPage) {
    int y = screenHeight - 16;
    tft.fillRect(0, y - 4, screenWidth, 20, COLOR_BG);

    // Page indicator dots
    int dotSpacing = 16;
    int startX = (screenWidth - (static_cast<int>(DashboardPage::PAGE_COUNT) * dotSpacing)) / 2;

    for (int i = 0; i < static_cast<int>(DashboardPage::PAGE_COUNT); ++i) {
        int x = startX + i * dotSpacing;
        if (i == static_cast<int>(currentPage)) {
            tft.fillCircle(x, y + 4, 4, COLOR_ACCENT);
        } else {
            tft.drawCircle(x, y + 4, 3, COLOR_MUTED);
        }
    }
}

void UIController::renderBambuPage(const BambuState& bambu) {
    renderHeader("BAMBU LAB 3D");

    // Top status badge
    uint16_t stateColor = COLOR_MUTED;
    if (bambu.gcode_state == "RUNNING") stateColor = COLOR_GREEN;
    else if (bambu.gcode_state == "PAUSE") stateColor = COLOR_YELLOW;
    else if (bambu.gcode_state == "FINISH") stateColor = COLOR_ACCENT;

    tft.fillRoundRect(10, 42, 300, 26, 4, COLOR_CARD);
    tft.setTextColor(stateColor, COLOR_CARD);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(bambu.gcode_state, 20, 55, 4);

    tft.setTextDatum(MR_DATUM);
    tft.setTextColor(COLOR_WHITE, COLOR_CARD);
    tft.drawString(bambu.subtask_name.substring(0, 18), 300, 55, 2);

    // Progress Bar Card
    tft.fillRoundRect(10, 76, 300, 54, 4, COLOR_CARD);
    tft.setTextColor(COLOR_WHITE, COLOR_CARD);
    tft.setTextDatum(ML_DATUM);
    tft.drawString("Print Progress", 20, 92, 2);

    // Percentage
    char pctStr[16];
    snprintf(pctStr, sizeof(pctStr), "%d%%", bambu.mc_percent);
    tft.setTextDatum(MR_DATUM);
    tft.setTextColor(COLOR_ACCENT, COLOR_CARD);
    tft.drawString(pctStr, 300, 92, 4);

    // Bar itself
    int barW = 280;
    int fillW = (bambu.mc_percent * barW) / 100;
    tft.drawRoundRect(20, 110, barW, 12, 4, COLOR_MUTED);
    if (fillW > 0) {
        tft.fillRoundRect(22, 112, fillW - 4, 8, 2, COLOR_GREEN);
    }

    // Telemetry Cards: Nozzle / Bed / Time
    int cardW = 94;
    int cardH = 58;
    int cardY = 138;

    // Nozzle Card
    tft.fillRoundRect(10, cardY, cardW, cardH, 4, COLOR_CARD);
    tft.setTextColor(COLOR_MUTED, COLOR_CARD);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("NOZZLE", 10 + cardW / 2, cardY + 6, 2);
    char nozStr[16];
    snprintf(nozStr, sizeof(nozStr), "%.0f C", bambu.nozzle_temper);
    tft.setTextColor(COLOR_WHITE, COLOR_CARD);
    tft.drawString(nozStr, 10 + cardW / 2, cardY + 28, 4);

    // Bed Card
    tft.fillRoundRect(113, cardY, cardW, cardH, 4, COLOR_CARD);
    tft.setTextColor(COLOR_MUTED, COLOR_CARD);
    tft.drawString("BED", 113 + cardW / 2, cardY + 6, 2);
    char bedStr[16];
    snprintf(bedStr, sizeof(bedStr), "%.0f C", bambu.bed_temper);
    tft.setTextColor(COLOR_WHITE, COLOR_CARD);
    tft.drawString(bedStr, 113 + cardW / 2, cardY + 28, 4);

    // Time Remaining Card
    tft.fillRoundRect(216, cardY, cardW, cardH, 4, COLOR_CARD);
    tft.setTextColor(COLOR_MUTED, COLOR_CARD);
    tft.drawString("REMAINING", 216 + cardW / 2, cardY + 6, 2);
    int hrs = bambu.mc_remaining_time / 60;
    int mins = bambu.mc_remaining_time % 60;
    char timeStr[16];
    if (hrs > 0) {
        snprintf(timeStr, sizeof(timeStr), "%dh %dm", hrs, mins);
    } else {
        snprintf(timeStr, sizeof(timeStr), "%dm", mins);
    }
    tft.setTextColor(COLOR_ACCENT, COLOR_CARD);
    tft.drawString(timeStr, 216 + cardW / 2, cardY + 28, 4);
}

void UIController::renderStocksPage(const StockItem* stocks, size_t count) {
    renderHeader("MARKETS & TICKERS");

    int startY = 44;
    int rowH = 40;

    for (size_t i = 0; i < count && i < 4; ++i) {
        int y = startY + (i * (rowH + 4));
        tft.fillRoundRect(10, y, 300, rowH, 4, COLOR_CARD);

        // Symbol
        tft.setTextColor(COLOR_WHITE, COLOR_CARD);
        tft.setTextDatum(ML_DATUM);
        tft.drawString(stocks[i].symbol, 20, y + rowH / 2, 4);

        // Price
        char priceStr[24];
        snprintf(priceStr, sizeof(priceStr), "$%.2f", stocks[i].price);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(priceStr, 175, y + rowH / 2, 4);

        // Percent change
        char chgStr[16];
        uint16_t chgColor = (stocks[i].changePercent >= 0) ? COLOR_GREEN : COLOR_RED;
        snprintf(chgStr, sizeof(chgStr), "%s%.2f%%", (stocks[i].changePercent >= 0 ? "+" : ""), stocks[i].changePercent);
        tft.setTextColor(chgColor, COLOR_CARD);
        tft.setTextDatum(MR_DATUM);
        tft.drawString(chgStr, 300, y + rowH / 2, 4);
    }
}

void UIController::renderLabHealthPage(const LabNodeStatus* nodes, size_t count) {
    renderHeader("LAB INFRASTRUCTURE");

    int startY = 44;
    int rowH = 40;

    for (size_t i = 0; i < count && i < 4; ++i) {
        int y = startY + (i * (rowH + 4));
        tft.fillRoundRect(10, y, 300, rowH, 4, COLOR_CARD);

        // Server / Node Name
        tft.setTextColor(COLOR_WHITE, COLOR_CARD);
        tft.setTextDatum(ML_DATUM);
        tft.drawString(nodes[i].name, 20, y + rowH / 2, 4);

        // Status badge
        uint16_t statusColor = nodes[i].online ? COLOR_GREEN : COLOR_RED;
        const char* statusTxt = nodes[i].online ? "ONLINE" : "OFFLINE";

        tft.setTextDatum(MR_DATUM);
        tft.setTextColor(statusColor, COLOR_CARD);

        char pingBuf[32];
        if (nodes[i].online) {
            snprintf(pingBuf, sizeof(pingBuf), "%s (%dms)", statusTxt, nodes[i].pingMs);
        } else {
            snprintf(pingBuf, sizeof(pingBuf), "%s", statusTxt);
        }
        tft.drawString(pingBuf, 300, y + rowH / 2, 4);
    }
}

void UIController::render(DashboardPage page, const BambuState& bambu, const StockItem* stocks, size_t stockCount, const LabNodeStatus* nodes, size_t nodeCount) {
    tft.fillScreen(COLOR_BG);

    switch (page) {
        case DashboardPage::BAMBU_PRINTER:
            renderBambuPage(bambu);
            break;
        case DashboardPage::STOCKS:
            renderStocksPage(stocks, stockCount);
            break;
        case DashboardPage::LAB_HEALTH:
            renderLabHealthPage(nodes, nodeCount);
            break;
        default:
            break;
    }

    renderFooter(page);
}
