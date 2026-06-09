#pragma once
#include <cstdint>

class GunControl {
public:
    GunControl();
    ~GunControl();

    bool init();
    void run_keyboard_control();

    void pump_set(uint8_t percent);
    void move_horiz(int dir);   // -1 = left, 1 = right
    void move_vert(int dir);    // -1 = down, 1 = up
    void stop_all();

private:
    int sock = -1;
    bool remote_override = true;
};