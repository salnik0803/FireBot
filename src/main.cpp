#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static int g_sock = -1;
static bool remote_override = true; // true = Raspberry Pi має пріоритет

/* ====================== НАЛАШТУВАННЯ ====================== */
#define SA_PC 0xF0
#define PGN_BUS_MANAGER 0x00FF1E
#define J1939_PRIORITY 3

// Насос
#define PUMP_OFF 0
#define PUMP_LOW 40
#define PUMP_MED 70
#define PUMP_FULL 100

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

/* ====================== CAN ====================== */
uint32_t j1939_build_id(uint8_t prio, uint32_t pgn, uint8_t sa) {
    return ((uint32_t)(prio & 0x7) << 26) |
           ((pgn & 0x1FFFF) << 8) |
           sa | CAN_EFF_FLAG;
}

int can_init(const char *iface = "can0") {
    struct sockaddr_can addr;
    struct ifreq ifr;

    g_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (g_sock < 0) { perror("socket"); return -1; }

    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);
    if (ioctl(g_sock, SIOCGIFINDEX, &ifr) < 0) { perror("ioctl"); return -1; }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(g_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); return -1;
    }
    printf("[CAN] Підключено до %s\n", iface);
    return 0;
}

int bm_send(BusManagerFrame *fr) {
    struct can_frame frame = {0};
    frame.can_id = j1939_build_id(J1939_PRIORITY, PGN_BUS_MANAGER, SA_PC);
    frame.can_dlc = 8;

    uint8_t *d = frame.data;
    d[0] = fr->free0; d[1] = fr->free1;
    d[2] = fr->emergency_stop;
    d[3] = fr->nozzle_speed;
    d[4] = fr->buttons;
    d[5] = fr->movement;
    d[6] = fr->ar_button;
    d[7] = fr->setting;

    if (write(g_sock, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("write");
        return -1;
    }
    return 0;
}

/* ====================== КОМАНДИ ====================== */
void pump_set(uint8_t percent) {
    BusManagerFrame fr = {0};
    fr.nozzle_speed = percent;
    bm_send(&fr);
    printf("[НАСОС] %d%%\n", percent);
}

void move_horiz(int dir) {   // -1 left, 1 right, 0 stop
    BusManagerFrame fr = {0};
    if (dir == 1)  fr.movement = 0x01;
    if (dir == -1) fr.movement = 0x04;
    bm_send(&fr);
    printf("[ГОРИЗОНТАЛЬ] %s\n", dir == 1 ? "ПРАВО" : "ЛІВО");
}

void move_vert(int dir) {    // -1 down, 1 up, 0 stop
    BusManagerFrame fr = {0};
    if (dir == 1)  fr.movement = 0x40;
    if (dir == -1) fr.movement = 0x10;
    bm_send(&fr);
    printf("[ВЕРТИКАЛЬ] %s\n", dir == 1 ? "ВГОРУ" : "ВНИЗ");
}

void stop_all() {
    BusManagerFrame fr = {0};
    bm_send(&fr);
    printf("[STOP ALL]\n");
}

int main() {
    if (can_init("can0") < 0) return 1;

    printf("\n=== POK Gun Control v2 (з підтримкою пульта) ===\n");
    printf("Режим: %s\n", remote_override ? "REMOTE OVERRIDE (Raspberry Pi)" : "LOCAL PULT");
    printf("W/S - Вертикаль | A/D - Горизонталь\n");
    printf("+/- - Насос | Пробіл - Стоп | R - Перемкнути режим | Q - Вихід\n\n");

    uint8_t pump = 0;

    while (1) {
        char c = getchar();
        if (c == '\n' || c == '\r') continue;

        if (c == 'q' || c == 'Q') break;
        if (c == 'r' || c == 'R') {
            remote_override = !remote_override;
            printf("→ Режим: %s\n", remote_override ? "REMOTE OVERRIDE" : "LOCAL PULT");
            continue;
        }
        if (c == ' ') {
            stop_all();
            pump = 0;
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
        } else {
            printf("(Локальний пульт активний - команди з Raspberry Pi вимкнено)\n");
        }
    }

    stop_all();
    close(g_sock);
    printf("Завершено.\n");
    return 0;
}
