#include <iostream>
#include <string>
#include "gun_control.h"
#include "platform_control.h"
#include "can_bus.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "   POK Fire Robot Control System v0.2\n";
    std::cout << "========================================\n\n";

    GunControl gun;
    PlatformControl platform;

    if (!gun.init()) {
        std::cerr << "Помилка ініціалізації GunControl (CAN)\n";
    }

    if (!platform.init()) {
        std::cerr << "Помилка ініціалізації PlatformControl (PCA9685)\n";
    }

    std::cout << "✅ Система запущена успішно!\n\n";

    std::cout << "Команди:\n";
    std::cout << "  w/s - Гармата вгору/вниз\n";
    std::cout << "  a/d - Гармата вліво/вправо\n";
    std::cout << "  +/- - Насос (потік)\n";
    std::cout << "  f/b - Платформа вперед/назад\n";
    std::cout << "  l/r - Платформа поворот ліворуч/праворуч\n";
    std::cout << "  e   - Emergency Stop\n";
    std::cout << "  q   - Вихід\n\n";

    char c;
    int pump_level = 0;

    while (true) {
        std::cin >> c;
        std::cin.ignore();

        if (c == 'q' || c == 'Q') break;

        if (c == 'e' || c == 'E') {
            gun.emergency_stop();
            platform.stop_all();
        }
        else if (c == 'w' || c == 'W') gun.move_up();
        else if (c == 's' || c == 'S') gun.move_down();
        else if (c == 'a' || c == 'A') gun.move_left();
        else if (c == 'd' || c == 'D') gun.move_right();
        else if (c == '+') {
            pump_level = std::min(100, pump_level + 10);
            gun.pump_set(pump_level);
            std::cout << "Насос: " << pump_level << "%\n";
        }
        else if (c == '-') {
            pump_level = std::max(0, pump_level - 10);
            gun.pump_set(pump_level);
            std::cout << "Насос: " << pump_level << "%\n";
        }
        else if (c == 'f' || c == 'F') platform.forward(70);
        else if (c == 'b' || c == 'B') platform.backward(60);
        else if (c == 'l' || c == 'L') platform.turn_left(70);
        else if (c == 'r' || c == 'R') platform.turn_right(70);
    }

    gun.stop_all();
    platform.stop_all();
    std::cout << "\nСистема завершена. До побачення!\n";
    return 0;
}