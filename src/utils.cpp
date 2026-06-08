#include "utils.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

void print_menu() {
    std::cout << "\n=== Головне меню ===\n";
    std::cout << "1 - Керування гарматою\n";
    std::cout << "2 - Керування платформою\n";
    std::cout << "3 - Статус системи\n";
    std::cout << "q - Вихід\n";
}

std::string get_current_time() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    return ss.str();
}

void signal_handler(int sig) {
    std::cout << "\nОтримано сигнал " << sig << ". Завершення...\n";
    exit(0);
}