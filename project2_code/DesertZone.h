#ifndef DESERTZONE_H
#define DESERTZONE_H

#include "GardenZone.h"

class DesertZone : public GardenZone {
public:
    DesertZone(const std::string& name, int moisture)
        : GardenZone(name, moisture) {}

    void dryOut() override {
        moistureLevel -= 5;
        if (moistureLevel < 0) moistureLevel = 0;
    }

    void irrigate() override {
        moistureLevel += 10;
        if (moistureLevel > 100) moistureLevel = 100;
    }

    void printStatus() const override {
        std::string health = getHealthStatus();
        std::cout << "  Zone: " << zoneName
                  << " | Type: Desert | Moisture: " << moistureLevel << "/100"
                  << " | Status: " << health << "\n";
    }

    std::string getType() const override { return "Desert"; }
};

#endif
