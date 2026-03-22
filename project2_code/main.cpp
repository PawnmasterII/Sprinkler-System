#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include "GardenZone.h"
#include "TropicalZone.h"
#include "DesertZone.h"
#include "AquaticZone.h"
#include "CircularQueue.h"

static int dayCount = 0;
static const std::string SAVE_FILE = "garden_save.txt";

// ─── Helpers ─────────────────────────────────────────────────────────────────

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void printHeader() {
    std::cout << "\n========================================\n";
    std::cout << "  USF Botanical Gardens Irrigation System\n";
    std::cout << "  Day: " << dayCount << "\n";
    std::cout << "========================================\n";
}

void printMenu() {
    std::cout << "\n--- Grounds Manager Control Panel ---\n";
    std::cout << " 1. Advance System / Run Sprinkler\n";
    std::cout << " 2. Add a New Zone\n";
    std::cout << " 3. Remove Current Zone\n";
    std::cout << " 4. View System Status\n";
    std::cout << " 5. Trigger Florida Heatwave\n";
    std::cout << " 6. Manual Water a Zone\n";
    std::cout << " 7. Rename a Zone\n";
    std::cout << " 8. Move Zone to Front\n";
    std::cout << " 9. Save System to File\n";
    std::cout << "10. Load System from File\n";
    std::cout << "11. View Moisture History\n";
    std::cout << "12. Exit\n";
    std::cout << "Enter choice: ";
}

GardenZone* makeZone(const std::string& type, const std::string& name, int moisture) {
    if (type == "Tropical") return new TropicalZone(name, moisture);
    if (type == "Desert")   return new DesertZone(name, moisture);
    if (type == "Aquatic")  return new AquaticZone(name, moisture);
    return nullptr;
}

// ─── Feature: Advance System ─────────────────────────────────────────────────

void advanceSystem(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "[!] No zones in the system.\n"; return; }

    dayCount++;
    std::cout << "\n=== Day " << dayCount << " ===\n";

    // Irrigate front zone
    GardenZone* current = queue.peek();
    std::cout << "[~] Irrigating: " << current->getName()
              << " (" << current->getType() << ")\n";
    current->irrigate();
    std::cout << "    Moisture: " << current->getMoisture() << "/100\n";

    // Dry out all other zones
    queue.traverse([](GardenZone*& zone, int /*i*/, bool isFront) {
        if (!isFront) {
            zone->dryOut();
            std::cout << "[sun] " << zone->getName()
                      << " dried out -> " << zone->getMoisture() << "/100\n";
        }
    });

    // Record history snapshot for all zones
    queue.traverse([](GardenZone*& zone, int /*i*/, bool /*f*/) {
        zone->recordHistory();
    });

    // Rain event: 30% chance
    if ((std::rand() % 100) < 30) {
        std::cout << "\n[RAIN] It's raining! All zones gain 15 moisture.\n";
        queue.traverse([](GardenZone*& zone, int /*i*/, bool /*f*/) {
            zone->applyRain();
            std::cout << "  " << zone->getName() << " -> " << zone->getMoisture() << "/100\n";
        });
    }

    // Rotate sprinkler
    queue.rotate();
    std::cout << "[>] Next zone: " << queue.peek()->getName() << "\n";

    // Print any critical/flooded warnings
    bool warned = false;
    queue.traverse([&warned](GardenZone*& zone, int /*i*/, bool /*f*/) {
        std::string s = zone->getHealthStatus();
        if (s == "CRITICAL" || s == "FLOODED") {
            if (!warned) { std::cout << "\n[WARNINGS]\n"; warned = true; }
            std::cout << "  *** " << zone->getName() << " is " << s << "! ***\n";
        }
    });
}

// ─── Feature: Add Zone ───────────────────────────────────────────────────────

void addZone(CircularQueue<GardenZone*>& queue) {
    std::string name;
    int typeChoice, moisture;

    std::cout << "\nEnter zone name: ";
    clearInput();
    std::getline(std::cin, name);

    std::cout << "Select type:\n  1. Tropical\n  2. Desert\n  3. Aquatic\nChoice: ";
    std::cin >> typeChoice;
    std::cout << "Enter starting moisture (0-100): ";
    std::cin >> moisture;
    moisture = std::max(0, std::min(100, moisture));

    GardenZone* z = nullptr;
    if      (typeChoice == 1) z = new TropicalZone(name, moisture);
    else if (typeChoice == 2) z = new DesertZone(name, moisture);
    else if (typeChoice == 3) z = new AquaticZone(name, moisture);
    else { std::cout << "[!] Invalid type.\n"; return; }

    queue.enqueue(z);
    std::cout << "[+] Added: " << name << " (" << z->getType() << ")\n";
}

// ─── Feature: Remove Current Zone ────────────────────────────────────────────

void removeCurrentZone(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "[!] No zones to remove.\n"; return; }
    GardenZone* removed = queue.dequeue();
    std::cout << "[-] Removed: " << removed->getName()
              << " (" << removed->getType() << ")\n";
    delete removed;
}

// ─── Feature: View Status ────────────────────────────────────────────────────

void viewStatus(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "\n[!] No zones in the system.\n"; return; }
    std::cout << "\n--- System Status | Day " << dayCount
              << " | " << queue.getSize() << " zones ---\n";
    queue.traverse([](GardenZone*& zone, int idx, bool isFront) {
        std::cout << (isFront ? " [NEXT] " : "  [" + std::to_string(idx) + "]   ");
        zone->printStatus();
    });
    std::cout << "-----------------------------------\n";
}

// ─── Feature: Heatwave ───────────────────────────────────────────────────────

void triggerHeatwave(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "[!] No zones in the system.\n"; return; }
    std::cout << "\n[!!!] FLORIDA HEATWAVE! All zones lose 30 moisture!\n";
    queue.traverse([](GardenZone*& zone, int /*i*/, bool /*f*/) {
        zone->applyHeatwave();
        std::cout << "  " << zone->getName() << " -> " << zone->getMoisture() << "/100"
                  << " [" << zone->getHealthStatus() << "]\n";
    });
}

// ─── Feature: Manual Water ───────────────────────────────────────────────────

void manualWater(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "[!] No zones in the system.\n"; return; }

    viewStatus(queue);
    std::cout << "Enter zone index to water: ";
    int idx; std::cin >> idx;
    if (idx < 0 || idx >= queue.getSize()) { std::cout << "[!] Invalid index.\n"; return; }

    std::cout << "Enter moisture amount to add: ";
    int amount; std::cin >> amount;
    if (amount <= 0) { std::cout << "[!] Amount must be positive.\n"; return; }

    GardenZone* zone = queue.getAt(idx);
    zone->manualWater(amount);
    std::cout << "[+] " << zone->getName() << " moisture -> " << zone->getMoisture() << "/100\n";
}

// ─── Feature: Rename Zone ────────────────────────────────────────────────────

void renameZone(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "[!] No zones in the system.\n"; return; }

    viewStatus(queue);
    std::cout << "Enter zone index to rename: ";
    int idx; std::cin >> idx;
    if (idx < 0 || idx >= queue.getSize()) { std::cout << "[!] Invalid index.\n"; return; }

    std::string newName;
    std::cout << "Enter new name: ";
    clearInput();
    std::getline(std::cin, newName);

    GardenZone* zone = queue.getAt(idx);
    std::string old = zone->getName();
    zone->setName(newName);
    std::cout << "[*] Renamed \"" << old << "\" -> \"" << newName << "\"\n";
}

// ─── Feature: Move Zone to Front ─────────────────────────────────────────────

void moveToFront(CircularQueue<GardenZone*>& queue) {
    if (queue.getSize() < 2) { std::cout << "[!] Need at least 2 zones.\n"; return; }

    viewStatus(queue);
    std::cout << "Enter zone index to move to front: ";
    int idx; std::cin >> idx;
    if (idx <= 0 || idx >= queue.getSize()) {
        std::cout << "[!] Invalid index (must be 1 or higher).\n"; return;
    }

    std::string name = queue.getAt(idx)->getName();
    queue.moveToFront(idx);
    std::cout << "[>] \"" << name << "\" is now at the front.\n";
}

// ─── Feature: Save to File ───────────────────────────────────────────────────

void saveSystem(CircularQueue<GardenZone*>& queue) {
    std::ofstream file(SAVE_FILE);
    if (!file) { std::cout << "[!] Could not open save file.\n"; return; }

    file << dayCount << "\n";
    file << queue.getSize() << "\n";
    queue.traverse([&file](GardenZone*& zone, int /*i*/, bool /*f*/) {
        file << zone->getName() << "\n"
             << zone->getType() << "\n"
             << zone->getMoisture() << "\n";
    });
    file.close();
    std::cout << "[SAVE] System saved to " << SAVE_FILE << "\n";
}

// ─── Feature: Load from File ─────────────────────────────────────────────────

void loadSystem(CircularQueue<GardenZone*>& queue) {
    std::ifstream file(SAVE_FILE);
    if (!file) { std::cout << "[!] Save file not found.\n"; return; }

    // Clear current queue
    while (!queue.isEmpty()) { delete queue.dequeue(); }

    int savedDay, count;
    file >> savedDay >> count;
    file.ignore();
    dayCount = savedDay;

    for (int i = 0; i < count; i++) {
        std::string name, type;
        int moisture;
        std::getline(file, name);
        std::getline(file, type);
        file >> moisture;
        file.ignore();

        GardenZone* z = makeZone(type, name, moisture);
        if (z) queue.enqueue(z);
    }
    file.close();
    std::cout << "[LOAD] Loaded " << count << " zones from " << SAVE_FILE
              << " (Day " << dayCount << ")\n";
}

// ─── Feature: View History ───────────────────────────────────────────────────

void viewHistory(CircularQueue<GardenZone*>& queue) {
    if (queue.isEmpty()) { std::cout << "[!] No zones in the system.\n"; return; }

    std::cout << "\n--- Moisture History (last 10 days) ---\n";
    queue.traverse([](GardenZone*& zone, int /*i*/, bool /*f*/) {
        const auto& hist = zone->getHistory();
        std::cout << "  " << zone->getName() << " (" << zone->getType() << "): ";
        if (hist.empty()) {
            std::cout << "no data yet\n";
            return;
        }
        for (int v : hist) std::cout << v << " ";
        // Simple trend arrow
        if (hist.size() >= 2) {
            int diff = hist.back() - hist[hist.size() - 2];
            if      (diff > 0)  std::cout << " [up]";
            else if (diff < 0)  std::cout << " [down]";
            else                std::cout << " [stable]";
        }
        std::cout << "\n";
    });
    std::cout << "---------------------------------------\n";
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    CircularQueue<GardenZone*> queue;
    queue.enqueue(new TropicalZone("Butterfly Garden", 60));
    queue.enqueue(new TropicalZone("Orchid Conservatory", 70));
    queue.enqueue(new DesertZone("Succulent Path", 30));
    queue.enqueue(new DesertZone("Cactus Courtyard", 20));
    queue.enqueue(new TropicalZone("Fern Grotto", 55));
    queue.enqueue(new AquaticZone("Lily Pad Pond", 85));

    printHeader();
    std::cout << "System initialized with 6 default zones.\n";

    int choice = 0;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) { clearInput(); continue; }

        switch (choice) {
            case  1: advanceSystem(queue);   break;
            case  2: addZone(queue);         break;
            case  3: removeCurrentZone(queue); break;
            case  4: viewStatus(queue);      break;
            case  5: triggerHeatwave(queue); break;
            case  6: manualWater(queue);     break;
            case  7: renameZone(queue);      break;
            case  8: moveToFront(queue);     break;
            case  9: saveSystem(queue);      break;
            case 10: loadSystem(queue);      break;
            case 11: viewHistory(queue);     break;
            case 12:
                std::cout << "\n[*] Shutting down irrigation system. Goodbye!\n";
                while (!queue.isEmpty()) { delete queue.dequeue(); }
                return 0;
            default:
                std::cout << "[!] Invalid choice.\n";
        }
    }
}
