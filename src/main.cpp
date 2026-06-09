#include <iostream>
#include "gun_control.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "   POK Fire Robot Control System v0.3\n";
    std::cout << "========================================\n\n";

    GunControl gun;

    if (!gun.init()) {
        std::cerr << "Не вдалося ініціалізувати систему!\n";
        return 1;
    }

    gun.run();        // ← викликаємо нову функцію run()

    std::cout << "Система завершена.\n";
    return 0;
}