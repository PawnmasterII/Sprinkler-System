#ifndef AQUATICZONE_H
#define AQUATICZONE_H

#include "GardenZone.h"

class AquaticZone : public GardenZone {
public:
    AquaticZone(const std::string& name, int moisture)
        : GardenZone(name, moisture) {}

    // Water features don't dry out naturally
    void dryOut() override {
        // No moisture loss — always submerged
    }

    // Overwatering causes overflow: resets to 80 if it exceeds 90
    void irrigate() override {
        moistureLevel += 20;
        if (moistureLevel > 90) {
            std::cout << "  [OVERFLOW] " << zoneName << " is overflowing! Draining to 80.\n";
            moistureLevel = 80;
        }
    }

    void printStatus() const override {
        std::string health = getHealthStatus();
        std::cout << "  Zone: " << zoneName
                  << " | Type: Aquatic | Moisture: " << moistureLevel << "/100"
                  << " | Status: " << health << "\n";
    }

    std::string getType() const override { return "Aquatic"; }
};

#endif
