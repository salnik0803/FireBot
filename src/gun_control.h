#pragma once
#include <string>
#include <cstdint>

// ====================== НАЛАШТУВАННЯ ======================
#define CAN_INTERFACE "can0"

// PGN та адреси для BUS MANAGER (POK STACS)
#define SA_PC           0xF0
#define SA_BUS_MANAGER  0x1E
#define PGN_BUS_MANAGER 0x00FF1E
#define J1939_PRIORITY  3

// Швидкості руху
#define SPEED_STOP      0x00
#define SPEED_SLOW      0x01
#define SPEED_FULL      0x02

// Стани сопла / diffuser
#define NOZZLE_NONE     0x00
#define NOZZLE_FOG      0x01
#define NOZZLE_STRAIGHT 0x02

class GunControl {
public:
    GunControl();
    ~GunControl();

    bool init();
    void stop_all();

    // Рухи гармати
    void move_up(uint8_t speed = SPEED_FULL);
    void move_down(uint8_t speed = SPEED_SLOW);
    void move_left(uint8_t speed = SPEED_FULL);
    void move_right(uint8_t speed = SPEED_SLOW);

    // Насос / потік
    void pump_set(uint8_t percent);        // 0-100%
    void nozzle_set(uint8_t mode);

    // Комбіновані рухи
    void move_horiz(int8_t direction);     // -100 .. 100
    void move_vert(int8_t direction);

    void emergency_stop();

    std::string get_status() const;

private:
    int sock = -1;
    bool send_bus_manager_frame(uint8_t byte3, uint8_t byte5, uint8_t byte6 = 0);
};