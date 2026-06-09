#include "gun_control.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <linux/joystick.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static int g_can_sock = -1;
static int g_joy_fd = -1;

#define SA_PC 0xF0
#define PGN_BUS_MANAGER 0x00FF1E
#define J1939_PRIORITY 3

typedef struct {
    uint8_t free0 = 0;
    uint8_t free1 = 0;
    uint8_t emergency_stop = 0;
    uint8_t nozzle_speed = 0;
    uint8_t buttons = 0;
    uint8_t movement = 0;
    uint8_t ar_button = 0;
    uint8_t setting = 0;
} BusManagerFrame;

uint32_t j1939_build_id(uint8_t prio, uint32_t pgn, uint8_t sa) {
    return ((uint32_t)(prio & 0x7) << 26) | ((pgn & 0x1FFFF) << 8) | sa | CAN_EFF_FLAG;
}

GunControl::GunControl() {}
GunControl::~GunControl() { stop_all(); }

bool GunControl::init() {
    if (!init_can()) return false;
    init_joystick();
    return true;
}

bool GunControl::init_can() {
    system("sudo ip link set can0 down 2>/dev/null || true");
    system("sudo slcand -o -c -s6 -S6 -t hw -F -b115200 /dev/ttyACM0 can0 2>/dev/null || true");
    sleep(1);
    system("sudo ip link set can0 up type can bitrate 250000 2>/dev/null || true");
    sleep(1);

    struct sockaddr_can addr;
    struct ifreq ifr;

    g_can_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (g_can_sock < 0) { perror("CAN socket"); return false; }

    strncpy(ifr.ifr_name, "can0", IFNAMSIZ-1);
    if (ioctl(g_can_sock, SIOCGIFINDEX, &ifr) < 0) { perror("CAN ioctl"); return false; }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(g_can_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("CAN bind"); return false;
    }

    std::cout << "[CAN] Підключено успішно\n";
    return true;
}

bool GunControl::init_joystick() {
    g_joy_fd = open("/dev/input/js0", O_RDONLY | O_NONBLOCK);
    if (g_joy_fd >= 0) {
        std::cout << "[JOYSTICK] ThrustMaster TCA Sidestick Airbus підключено!\n";
        return true;
    }
    return false;
}

int bm_send(BusManagerFrame *fr) {
    if (g_can_sock < 0) return -1;
    struct can_frame frame = {0};
    frame.can_id = j1939_build_id(J1939_PRIORITY, PGN_BUS_MANAGER, SA_PC);
    frame.can_dlc = 8;

    uint8_t *d = frame.data;
    d[0] = fr->free0; d[1] = fr->free1; d[2] = fr->emergency_stop;
    d[3] = fr->nozzle_speed; d[4] = fr->buttons; d[5] = fr->movement;
    d[6] = fr->ar_button; d[7] = fr->setting;

    write(g_can_sock, &frame, sizeof(frame));
    return 0;
}

void GunControl::pump_set(uint8_t percent) {
    BusManagerFrame fr = {0};
    fr.nozzle_speed = percent;
    bm_send(&fr);
    std::cout << "[НАСОС] " << (int)percent << "%\n";
}

void GunControl::move_horiz(int value) {
    BusManagerFrame fr = {0};
    if (value > 20) fr.movement = 0x01;
    else if (value < -20) fr.movement = 0x04;
    bm_send(&fr);
}

void GunControl::move_vert(int value) {
    BusManagerFrame fr = {0};
    if (value > 20) fr.movement = 0x10;   // вниз
    else if (value < -20) fr.movement = 0x40; // вгору
    bm_send(&fr);
}

void GunControl::stop_all() {
    BusManagerFrame fr = {0};
    bm_send(&fr);
    std::cout << "[STOP ALL]\n";
}

void GunControl::run() {
    std::cout << "\n=== POK Гармата - Керування активне ===\n";
    std::cout << "Джойстик працює в реальному часі (без Enter)\n";
    std::cout << "Q - вихід\n\n";

    while (true) {
        // === Джойстик (реалтайм) ===
        if (g_joy_fd >= 0) {
            js_event event;
            while (read(g_joy_fd, &event, sizeof(event)) > 0) {
                if (event.type & JS_EVENT_AXIS) {
                    int value = (event.value * 100) / 32767;   // -100 .. 100

                    if (event.number == 0) move_horiz(value);      // Горизонталь
                    if (event.number == 1) move_vert(value);       // Вертикаль
                }
                if (event.type & JS_EVENT_BUTTON && event.value == 1) {
                    if (event.number == 0) stop_all();   // Триґер = STOP
                }
            }
        }

        // === Клавіатура (неблокуюча) ===
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        struct timeval tv = {0, 10000};   // 10ms

        if (select(1, &fds, NULL, NULL, &tv) > 0) {
            char c;
            if (read(0, &c, 1) > 0) {
                if (c == 'q' || c == 'Q') break;
                if (c == ' ') stop_all();
            }
        }

        usleep(10000); // 10ms цикл
    }

    stop_all();
}