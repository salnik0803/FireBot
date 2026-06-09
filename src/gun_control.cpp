#include "gun_control.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>

static int g_can_sock = -1;
static int g_joy_fd = -1;

GunControl::GunControl() {}
GunControl::~GunControl() { stop_all(); }

bool GunControl::init() {
    init_can();
    init_joystick();
    return true;
}

// ... (твій init_can залишається без змін)

bool GunControl::init_joystick() {
    g_joy_fd = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
    if (g_joy_fd >= 0) {
        std::cout << "[JOYSTICK] Підключено!\n";
        return true;
    }
    return false;
}

void GunControl::run() {
    std::cout << "\n=== Просте тестування ===\n";
    std::cout << "Натискай клавіші або рухай стік джойстика\n";
    std::cout << "Q - вихід\n\n";

    while (true) {
        // Джойстик
        if (g_joy_fd >= 0) {
            js_event event;
            while (read(g_joy_fd, &event, sizeof(event)) > 0) {
                if (event.type & JS_EVENT_AXIS) {
                    int val = event.value * 100 / 32767;
                    std::cout << "Axis " << event.number << " = " << val << "\n";
                }
            }
        }

        // Клавіатура
        char c = 0;
        if (read(0, &c, 1) > 0) {
            if (c == 'q' || c == 'Q') break;
            std::cout << "Натиснуто: " << c << "\n";
        }

        usleep(50000);
    }
}