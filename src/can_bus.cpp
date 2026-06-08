#include "can_bus.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static int g_sock = -1;

int can_open(const char* interface) {
    if (g_sock >= 0) return g_sock;

    struct sockaddr_can addr;
    struct ifreq ifr;

    g_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (g_sock < 0) {
        std::cerr << "CAN socket error: " << strerror(errno) << std::endl;
        return -1;
    }

    std::strcpy(ifr.ifr_name, interface);
    if (ioctl(g_sock, SIOCGIFINDEX, &ifr) < 0) {
        std::cerr << "CAN ioctl error: " << strerror(errno) << std::endl;
        close(g_sock);
        g_sock = -1;
        return -1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(g_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "CAN bind error: " << strerror(errno) << std::endl;
        close(g_sock);
        g_sock = -1;
        return -1;
    }

    std::cout << "✅ CAN interface " << interface << " opened successfully.\n";
    return g_sock;
}

void can_close() {
    if (g_sock >= 0) {
        close(g_sock);
        g_sock = -1;
    }
}

bool can_send_raw(uint32_t can_id, const uint8_t* data, uint8_t dlc) {
    if (g_sock < 0) return false;

    struct can_frame frame;
    frame.can_id = can_id;
    frame.can_dlc = dlc;
    std::memcpy(frame.data, data, dlc);

    if (write(g_sock, &frame, sizeof(frame)) != sizeof(frame)) {
        std::cerr << "CAN write error\n";
        return false;
    }
    return true;
}

int can_send_bus_manager(uint8_t byte3, uint8_t byte5, uint8_t byte6) {
    if (g_sock < 0) {
        if (can_open() < 0) return -1;
    }

    uint32_t can_id = 0x0CFF1E00 | SA_PC;  // PGN 0xFF1E + SA_PC

    uint8_t data[8] = {0};
    data[0] = 0xFF;      // стандартний префікс
    data[1] = 0x03;
    data[2] = 0x00;      // emergency stop (0 = normal)
    data[3] = byte3;     // насос 0-100%
    data[4] = 0x00;
    data[5] = byte5;     // рухи (VU, VD, HL, HR)
    data[6] = byte6;
    data[7] = 0x00;

    if (can_send_raw(can_id, data, 8)) {
        // std::cout << "CAN → Bus Manager sent\n";
        return 0;
    }
    return -1;
}

std::string can_id_to_string(uint32_t id) {
    char buf[20];
    snprintf(buf, sizeof(buf), "0x%08X", id);
    return buf;
}