#include <iostream>
#include "gun_control.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "   POK Fire Robot Control System v0.4\n";
    std::cout << "========================================\n\n";

    GunControl gun;

    if (!gun.init()) {
        std::cerr << "Не вдалося ініціалізувати систему!\n";
        return 1;
    }

    std::cout << "Підключено ThrustMaster TCA Sidestick Airbus\n";
    gun.run();

    std::cout << "Система завершена.\n";
    return 0;
}