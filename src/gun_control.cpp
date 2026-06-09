#include "gun_control.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static int g_sock = -1;

#define SA_PC 0xF0
#define PGN_BUS_MANAGER 0x00FF1E
#define J1939_PRIORITY 3

// ... (структура BusManagerFrame та j1939_build_id залишаються)

GunControl::GunControl() {}
GunControl::~GunControl() { stop_all(); }

bool GunControl::init() {
    // Автоматичний підйом CAN
    system("sudo slcand -o -c -s6 -S6 -t hw -F -b115200 /dev/ttyACM0 can0 2>/dev/null || true");
    sleep(1);
    system("sudo ip link set can0 up type can bitrate 250000 2>/dev/null || true");

    // ... (твій код ініціалізації сокета)
    // (залиш той самий код can_init, який працював)
}

void GunControl::run() {
    std::cout << "\n=== POK Гармата - Головне меню ===\n";
    std::cout << "1 - Керування гарматою (WASD + +/-)\n";
    std::cout << "R - Перемкнути режим (Raspberry / POK пульт)\n";
    std::cout << "Q - Вихід\n\n";

    uint8_t pump = 0;
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty()) continue;
        char c = input[0];

        if (c == 'q' || c == 'Q') break;
        if (c == 'r' || c == 'R') {
            remote_override = !remote_override;
            std::cout << "→ Режим: " << (remote_override ? "RASPBERRY PI" : "POK PULT") << "\n";
            continue;
        }

        if (remote_override) {
            if (c == 'w' || c == 'W') move_vert(1);
            else if (c == 's' || c == 'S') move_vert(-1);
            else if (c == 'a' || c == 'A') move_horiz(-1);
            else if (c == 'd' || c == 'D') move_horiz(1);
            else if (c == '+' || c == '=') {
                pump = (pump >= 90) ? 100 : pump + 10;
                pump_set(pump);
            }
            else if (c == '-' || c == '_') {
                pump = (pump <= 10) ? 0 : pump - 10;
                pump_set(pump);
            }
            else if (c == ' ') stop_all();
        }
    }
    stop_all();
}