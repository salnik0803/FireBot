#pragma once
#include <cstdint>
#include <string>

class GunControl {
public:
    GunControl();
    ~GunControl();

    bool init();
    void run();                    // Основний цикл (клавіатура + джойстик)

    void pump_set(uint8_t percent);
    void move_horiz(int value);    // -100..100
    void move_vert(int value);     // -100..100
    void stop_all();

private:
    int can_sock = -1;
    int joy_fd = -1;
    bool remote_override = true;

    bool init_can();
    bool init_joystick();
    void process_joystick();       // обробка джойстика
    void process_keyboard();
};