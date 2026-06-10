#include "app/app.hpp"

#include <string>

#include "app/driver_stm32_fdcan.hpp"
#include "fdcan.h"
#include "gn10_can/core/can_bus.hpp"
#include "gn10_can/devices/motor_driver_client.hpp"
#include "gn10_can/devices/motor_driver_types.hpp"
#include "gn10_can/devices/solenoid_driver_server.hpp"
#include "tim.h"
#include "usart.h"
namespace {

constexpr uint32_t k_heartbeat_toggle_interval_ms = 500;

uint32_t heartbeat_last_toggle_time_ms = 0;

/**
 * @brief Toggle heartbeat LED at a fixed interval.
 */
void update_heartbeat_led()
{
    const uint32_t now_ms = HAL_GetTick();
    if ((now_ms - heartbeat_last_toggle_time_ms) >= k_heartbeat_toggle_interval_ms) {
        heartbeat_last_toggle_time_ms = now_ms;
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
}
gn10_can::drivers::DriverSTM32FDCAN driver(&hfdcan1);
gn10_can::CANBus canbus(driver);
gn10_can::devices::SolenoidDriverServer solenoid_driver(canbus, 0);
uint8_t solenoid[8] = {0};
std::array<bool, 8> solenoid_targets{};
}  // namespace

/**
 * @brief Initialize CAN and mainboard application state.
 */
void setup()
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    driver.init();
    solenoid_driver.get_new_init();
}

/**
 * @brief Run one control cycle and update status heartbeat LED.
 */
void loop()
{
    if (solenoid_driver.get_new_target(solenoid_targets)) {
        for (size_t i = 0; i < 8; i++) {
            solenoid[i] = solenoid_targets[i];
        }
    }
    update_heartbeat_led();
    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, (GPIO_PinState)solenoid[0]);
    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, (GPIO_PinState)solenoid[1]);
    HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, (GPIO_PinState)solenoid[2]);
    HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, (GPIO_PinState)solenoid[3]);
    gn10_can::CANFrame frame;
    if (driver.receive(frame)) {
        char tx_data[64];
        size_t len = sprintf(tx_data, "id:%lx, len:%d", frame.id, frame.dlc);
        HAL_UART_Transmit(&huart1, (uint8_t*)tx_data, len, 1000);
    }
}
extern "C" {
// C言語側の関数のオーバーライド
/**
 * @brief Receive callback for FDCAN FIFO0.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs) {}
}