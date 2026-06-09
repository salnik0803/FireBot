#pragma once
#include <cstdint>

class GunControl {
public:
    GunControl();
    ~GunControl();

    bool init();
    void run();                     // ← Головне меню

    void pump_set(uint8_t percent);
    void move_horiz(int dir);
    void move_vert(int dir);
    void stop_all();

private:
    int sock = -1;
    bool remote_override = true;
};