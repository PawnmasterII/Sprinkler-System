#ifndef GARDENZONE_H
#define GARDENZONE_H

#include <string>
#include <iostream>
#include <vector>

class GardenZone {
protected:
    std::string zoneName;
    int moistureLevel; // 0 to 100
    std::vector<int> history; // moisture readings over time (last 10)

public:
    GardenZone(const std::string& name, int moisture)
        : zoneName(name), moistureLevel(moisture) {}

    virtual ~GardenZone() {}

    std::string getName() const { return zoneName; }
    int getMoisture() const { return moistureLevel; }
    void setName(const std::string& name) { zoneName = name; }

    void recordHistory() {
        history.push_back(moistureLevel);
        if ((int)history.size() > 10) history.erase(history.begin());
    }

    const std::vector<int>& getHistory() const { return history; }

    // Health status label
    std::string getHealthStatus() const {
        if (moistureLevel < 20)  return "CRITICAL";
        if (moistureLevel > 90)  return "FLOODED";
        if (moistureLevel < 40)  return "Low";
        return "OK";
    }

    virtual void dryOut() {
        moistureLevel -= 10;
        if (moistureLevel < 0) moistureLevel = 0;
    }

    virtual void irrigate() {
        moistureLevel += 20;
        if (moistureLevel > 100) moistureLevel = 100;
    }

    // Manual water: flat +amount regardless of type
    void manualWater(int amount) {
        moistureLevel += amount;
        if (moistureLevel > 100) moistureLevel = 100;
    }

    virtual void printStatus() const {
        std::string health = getHealthStatus();
        std::cout << "  Zone: " << zoneName
                  << " | Type: Base | Moisture: " << moistureLevel << "/100"
                  << " | Status: " << health << "\n";
    }

    virtual std::string getType() const { return "Base"; }

    // Flat -30 for heatwave events regardless of zone type
    void applyHeatwave() {
        moistureLevel -= 30;
        if (moistureLevel < 0) moistureLevel = 0;
    }

    // Flat +15 for rain events regardless of zone type
    void applyRain() {
        moistureLevel += 15;
        if (moistureLevel > 100) moistureLevel = 100;
    }
};

#endif
