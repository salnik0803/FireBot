#include <iostream>
#include "gun_control.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "   POK Fire Robot Control System v0.3\n";
    std::cout << "========================================\n\n";

    GunControl gun;

    if (!gun.init()) {
        std::cerr << "Не вдалося ініціалізувати GunControl!\n";
        return 1;
    }

    std::cout << "✅ Система запущена успішно!\n\n";
    gun.run_keyboard_control();

    std::cout << "Система завершена.\n";
    return 0;
}