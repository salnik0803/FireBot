#pragma once
#include <string>
#include <cstdint>

// Ініціалізація та базові функції CAN
int can_open(const char* interface = "can0");
void can_close();
int can_send_bus_manager(uint8_t byte3, uint8_t byte5, uint8_t byte6 = 0);

// Допоміжні функції
bool can_send_raw(uint32_t can_id, const uint8_t* data, uint8_t dlc);
std::string can_id_to_string(uint32_t id);