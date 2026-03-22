#ifndef TROPICALZONE_H
#define TROPICALZONE_H

#include "GardenZone.h"

class TropicalZone : public GardenZone {
public:
    TropicalZone(const std::string& name, int moisture)
        : GardenZone(name, moisture) {}

    void dryOut() override {
        moistureLevel -= 20;
        if (moistureLevel < 0) moistureLevel = 0;
    }

    void irrigate() override {
        moistureLevel += 40;
        if (moistureLevel > 100) moistureLevel = 100;
    }

    void printStatus() const override {
        std::string health = getHealthStatus();
        std::cout << "  Zone: " << zoneName
                  << " | Type: Tropical | Moisture: " << moistureLevel << "/100"
                  << " | Status: " << health << "\n";
    }

    std::string getType() const override { return "Tropical"; }
};

#endif
