#pragma once
#include <string>
#include <cstdint>

// Канали PCA9685 для гусеничної платформи
#define CH_DRIVE_LEFT   0   // Ліва гусениця
#define CH_DRIVE_RIGHT  1   // Права гусениця
#define CH_PUMP         2   // Додатковий насос / клапан
#define CH_AUX1         3   // Резерв

#define PWM_MIN_US      1000
#define PWM_MAX_US      2000
#define PWM_NEUTRAL_US  1500

class PlatformControl {
public:
    PlatformControl();
    ~PlatformControl();

    bool init();                    // Ініціалізація PCA9685
    void stop_all();

    // Керування гусеницями
    void drive(int16_t left, int16_t right);   // -100 .. 100
    void turn_left(uint8_t speed = 70);
    void turn_right(uint8_t speed = 70);
    void forward(uint8_t speed = 80);
    void backward(uint8_t speed = 60);

    void set_pump(uint8_t percent);   // 0-100%

    std::string get_status() const;

private:
    bool pca9685_init();
    void pca9685_set_pwm_us(uint8_t channel, uint16_t pulse_us);
    void pca9685_set_pwm_percent(uint8_t channel, uint8_t percent);
};