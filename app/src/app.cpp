#include "app/app.hpp"

#include <string>

#include "app/driver_stm32_fdcan.hpp"
#include "fdcan.h"
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
gn10_can::CANFrame canframe;
uint8_t solenoid[4] = {0};
uint16_t duty1 = 0, duty2 = 0, duty3 = 0, duty4 = 0;
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
}

/**
 * @brief Run one control cycle and update status heartbeat LED.
 */
void loop()
{
    if (driver.receive(canframe)) {
        solenoid[0] = canframe.data[0];
        solenoid[1] = canframe.data[1];
        solenoid[2] = canframe.data[2];
        solenoid[3] = canframe.data[3];
        }
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    HAL_GPIO_TogglePin(IN1_GPIO_Port, solenoid[0]);
    HAL_GPIO_TogglePin(IN2_GPIO_Port, solenoid[1]);
    HAL_GPIO_TogglePin(IN3_GPIO_Port, solenoid[2]);
    HAL_GPIO_TogglePin(IN4_GPIO_Port, solenoid[3]);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 40);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 40);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 40);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 40);
    // update_heartbeat_led();
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    HAL_GPIO_TogglePin(IN1_GPIO_Port, IN1_Pin);
    HAL_GPIO_TogglePin(IN2_GPIO_Port, IN2_Pin);
    HAL_GPIO_TogglePin(IN3_GPIO_Port, IN3_Pin);
    HAL_GPIO_TogglePin(IN4_GPIO_Port, IN4_Pin);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 10);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 10);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 10);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 10);
    // update_heartbeat_led();
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    HAL_GPIO_TogglePin(IN1_GPIO_Port, IN1_Pin);
    HAL_GPIO_TogglePin(IN2_GPIO_Port, IN2_Pin);
    HAL_GPIO_TogglePin(IN3_GPIO_Port, IN3_Pin);
    HAL_GPIO_TogglePin(IN4_GPIO_Port, IN4_Pin);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, 0);
    // update_heartbeat_led();
    HAL_Delay(1000);
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