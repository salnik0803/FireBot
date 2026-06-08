#include "gun_control.h"
#include "can_bus.h"
#include <iostream>
#include <sstream>

GunControl::GunControl() {}

GunControl::~GunControl() {
    stop_all();
}

bool GunControl::init() {
    sock = can_open(CAN_INTERFACE);
    if (sock < 0) {
        std::cerr << "GunControl: Не вдалося відкрити CAN!\n";
        return false;
    }
    std::cout << "GunControl: Ініціалізовано успішно\n";
    return true;
}

void GunControl::stop_all() {
    send_bus_manager_frame(0, SPEED_STOP, 0);
}

void GunControl::move_up(uint8_t speed) {
    uint8_t mv = (speed << 6);           // VU
    send_bus_manager_frame(0, mv);
}

void GunControl::move_down(uint8_t speed) {
    uint8_t mv = (speed << 4);           // VD
    send_bus_manager_frame(0, mv);
}

void GunControl::move_left(uint8_t speed) {
    uint8_t mv = (speed << 2);           // HL
    send_bus_manager_frame(0, mv);
}

void GunControl::move_right(uint8_t speed) {
    uint8_t mv = speed;                  // HR
    send_bus_manager_frame(0, mv);
}

void GunControl::pump_set(uint8_t percent) {
    send_bus_manager_frame(percent, 0);
}

void GunControl::nozzle_set(uint8_t mode) {
    // TODO: реалізувати через відповідні біти
    std::cout << "Nozzle mode: " << (int)mode << "\n";
}

void GunControl::move_horiz(int8_t direction) {
    if (direction > 0) move_right(SPEED_FULL);
    else if (direction < 0) move_left(SPEED_FULL);
    else stop_all();
}

void GunControl::move_vert(int8_t direction) {
    if (direction > 0) move_up(SPEED_FULL);
    else if (direction < 0) move_down(SPEED_FULL);
    else stop_all();
}

void GunControl::emergency_stop() {
    // Байт 2 = Emergency Stop
    send_bus_manager_frame(0, 0, 0x01);
    std::cout << "🚨 EMERGENCY STOP ACTIVATED!\n";
}

std::string GunControl::get_status() const {
    std::ostringstream oss;
    oss << "GunControl: Активний | CAN sock: " << sock;
    return oss.str();
}

// Приватний метод відправки фрейму
bool GunControl::send_bus_manager_frame(uint8_t byte3, uint8_t byte5, uint8_t byte6) {
    // Реалізація буде в can_bus.cpp
    return can_send_bus_manager(byte3, byte5, byte6);
}