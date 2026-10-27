#include "stepper_motor.h"

#include <stddef.h>

#include "board_config.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Pin numbers come from board_config.h; the cast to the driver's enum stays
// here, because board_config.h must not include ESP-IDF headers. IN1..IN4 in
// the order the Arduino sketch passed them to Stepper().
static const gpio_num_t PINS[STEPPER_PHASES] = {
    (gpio_num_t)BOARD_STEPPER_IN1,
    (gpio_num_t)BOARD_STEPPER_IN2,
    (gpio_num_t)BOARD_STEPPER_IN3,
    (gpio_num_t)BOARD_STEPPER_IN4,
};

// Arduino `Stepper`'s four-pin full-step sequence, one nibble per phase with
// IN1 in the high bit: 1010, 0110, 0101, 1001.
//
// Both coils are energised in every phase -- that is where full-step gets its
// torque, and also why stopping has to release the pins rather than leave the
// last phase standing.
static const uint8_t PHASE[STEPPER_PHASES] = { 0xA, 0x6, 0x5, 0x9 };

static esp_timer_handle_t s_timer;
static SemaphoreHandle_t  s_lock;
// Read by the dead-man switch and the interlock from other tasks without the
// lock, so it is volatile: a single byte, and they only ever ask "is it on".
static volatile bool s_running;
static size_t        s_phase;               // index into PHASE[], kept across runs
static uint64_t      s_steps;

// The lock exists for one race. esp_timer_stop() does not wait for a callback
// that has already been dispatched, so without it a tick could write a phase
// *after* stop() had released the pins -- leaving two coils energised on a
// motor the operator had just switched off. tick() does its whole job inside
// the lock, so stop() either runs before it or after it, never through it.
#define LOCK()   xSemaphoreTakeRecursive(s_lock, portMAX_DELAY)
#define UNLOCK() xSemaphoreGiveRecursive(s_lock)

static void write_phase(uint8_t bits)
{
    for (size_t i = 0; i < STEPPER_PHASES; i++) {
        ESP_ERROR_CHECK(gpio_set_level(PINS[i], (bits >> (STEPPER_PHASES - 1 - i)) & 1u));
    }
}

// Runs in the esp_timer task, every STEPPER_STEP_INTERVAL_US while on. Four
// GPIO writes: short enough to belong in that task rather than a task of its
// own, which would only add a context switch per step.
static void tick(void *arg)
{
    (void)arg;
    LOCK();
    if (s_running) {
        s_phase = (s_phase + 1) % STEPPER_PHASES;
        write_phase(PHASE[s_phase]);
        s_steps++;
    }
    UNLOCK();
}

void stepper_motor_init(void)
{
    s_lock = xSemaphoreCreateRecursiveMutex();
    configASSERT(s_lock);

    const gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << BOARD_STEPPER_IN1) | (1ULL << BOARD_STEPPER_IN2) |
                        (1ULL << BOARD_STEPPER_IN3) | (1ULL << BOARD_STEPPER_IN4),
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
    write_phase(0);                         // coils released before anything can command them

    const esp_timer_create_args_t args = { .callback = tick, .name = "stepper" };
    ESP_ERROR_CHECK(esp_timer_create(&args, &s_timer));
}

bool stepper_motor_start(void)
{
    bool started = false;

    LOCK();
    if (!s_running) {
        s_running = true;
        s_steps   = 0;
        // Energise the phase it was left on, so the first tick is a real step
        // rather than a jump from released coils.
        write_phase(PHASE[s_phase]);
        ESP_ERROR_CHECK(esp_timer_start_periodic(s_timer, STEPPER_STEP_INTERVAL_US));
        started = true;
    }
    UNLOCK();
    return started;
}

bool stepper_motor_stop(void)
{
    bool stopped = false;

    LOCK();
    if (s_running) {
        s_running = false;
        esp_timer_stop(s_timer);            // harmless if it is not running
        write_phase(0);                     // release the coils; see the header
        stopped = true;
    }
    UNLOCK();
    return stopped;
}

bool stepper_motor_running(void) { return s_running; }

// Under the lock: a 64-bit counter is two loads on a 32-bit core, and a tick
// landing between them would report a step count that never existed.
uint64_t stepper_motor_steps(void)
{
    LOCK();
    const uint64_t steps = s_steps;
    UNLOCK();
    return steps;
}
