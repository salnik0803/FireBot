#include "platform_control.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define PCA9685_ADDR     0x40
#define PCA9685_MODE1    0x00
#define PCA9685_PRESCALE 0xFE

static int i2c_fd = -1;

PlatformControl::PlatformControl() {}

PlatformControl::~PlatformControl() {
    stop_all();
}

bool PlatformControl::init() {
    if (!pca9685_init()) {
        std::cerr << "PlatformControl: PCA9685 init failed!\n";
        return false;
    }
    std::cout << "PlatformControl: Ініціалізовано (PCA9685)\n";
    stop_all();
    return true;
}

void PlatformControl::stop_all() {
    for (int ch = 0; ch < 4; ++ch) {
        pca9685_set_pwm_us(ch, PWM_NEUTRAL_US);
    }
}

void PlatformControl::drive(int16_t left, int16_t right) {
    pca9685_set_pwm_percent(CH_DRIVE_LEFT, 50 + left/2);
    pca9685_set_pwm_percent(CH_DRIVE_RIGHT, 50 + right/2);
}

void PlatformControl::forward(uint8_t speed) {
    drive(speed, speed);
}

void PlatformControl::backward(uint8_t speed) {
    drive(-speed, -speed);
}

void PlatformControl::turn_left(uint8_t speed) {
    drive(-speed, speed);
}

void PlatformControl::turn_right(uint8_t speed) {
    drive(speed, -speed);
}

void PlatformControl::set_pump(uint8_t percent) {
    pca9685_set_pwm_percent(CH_PUMP, percent);
}

std::string PlatformControl::get_status() const {
    return "PlatformControl: Активний";
}

// ==================== НИЗЬКОРІВНЕВЕ КЕРУВАННЯ PCA9685 ====================

bool PlatformControl::pca9685_init() {
    const char* i2c_bus = "/dev/i2c-1";   // Для Raspberry Pi 4

    i2c_fd = open(i2c_bus, O_RDWR);
    if (i2c_fd < 0) {
        std::cerr << "Не вдалося відкрити I2C: " << i2c_bus << std::endl;
        return false;
    }

    if (ioctl(i2c_fd, I2C_SLAVE, PCA9685_ADDR) < 0) {
        std::cerr << "I2C ioctl error\n";
        return false;
    }

    // Включаємо режим
    uint8_t mode1 = 0x01 | 0x80;  // Auto-increment + Sleep off
    // Тут можна додати встановлення частоти 50Hz...

    std::cout << "PCA9685 ініціалізовано на " << i2c_bus << "\n";
    return true;
}

void PlatformControl::pca9685_set_pwm_us(uint8_t channel, uint16_t pulse_us) {
    // Простий stub — повна реалізація пізніше
    std::cout << "[PCA9685] Channel " << (int)channel 
              << " → " << pulse_us << " µs\n";
}

void PlatformControl::pca9685_set_pwm_percent(uint8_t channel, uint8_t percent) {
    uint16_t pulse = PWM_MIN_US + (PWM_MAX_US - PWM_MIN_US) * percent / 100;
    pca9685_set_pwm_us(channel, pulse);
}