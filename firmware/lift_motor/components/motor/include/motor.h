// The ZK-5AD / actuator driver layer: owns the two PWM pins and nothing else.
#pragma once

#include <stdbool.h>
#include <stdint.h>

void motor_init(void);

// direction: 'e' extend, 'r' retract, 's' stop (coast). speed_pct is clamped
// to the stall floor by the command layer's speed_clamp().
void motor_set(char direction, int speed_pct);

// Diagnostic: hold one input at full scale and the other low, with no PWM at
// all. 'A' = PIN_A/GPIO4, 'B' = PIN_B/GPIO5. Splits a PWM-path fault from a
// pin, wire or driver-module fault in one keypress.
void motor_hold_raw(char which);

// Slow decay brakes during the off-phase; fast decay coasts. See README.
void motor_set_slow_decay(bool slow);
bool motor_slow_decay(void);

char     motor_direction(void);
int      motor_speed_pct(void);
uint32_t motor_duty_a(void);
uint32_t motor_duty_b(void);
uint32_t motor_pwm_freq(void);
const char *motor_pin_name(char which);
