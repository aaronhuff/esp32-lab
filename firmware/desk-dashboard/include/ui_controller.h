#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "bambu_types.h"

enum class DashboardPage {
    BAMBU_PRINTER = 0,
    STOCKS,
    LAB_HEALTH,
    PAGE_COUNT
};

struct StockItem {
    const char* symbol;
    float price;
    float changePercent;
};

struct LabNodeStatus {
    const char* name;
    bool online;
    int pingMs;
};

class UIController {
public:
    UIController(TFT_eSPI& tft);
    void begin();
    void render(DashboardPage page, const BambuState& bambu, const StockItem* stocks, size_t stockCount, const LabNodeStatus* nodes, size_t nodeCount);
    void showBootSplash(const char* statusMsg);

private:
    void renderHeader(const char* title);
    void renderBambuPage(const BambuState& bambu);
    void renderStocksPage(const StockItem* stocks, size_t count);
    void renderLabHealthPage(const LabNodeStatus* nodes, size_t count);
    void renderFooter(DashboardPage currentPage);

    TFT_eSPI& tft;
    uint16_t screenWidth = 320;
    uint16_t screenHeight = 240;
};
