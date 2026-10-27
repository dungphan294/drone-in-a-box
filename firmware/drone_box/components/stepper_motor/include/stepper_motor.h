// The stepper driver layer: owns four GPIOs and nothing else.
//
// Ported from ../../StepperMotorTest/StepperMotorTest.ino, which used
// Arduino's `Stepper` library at 200 steps/rev and 100 RPM. The phase
// sequence is that library's and the step interval is its setSpeed()
// arithmetic, so the motor turns as it did on the bench sketch. The pins and
// both numbers now come from board_config.h.
//
// The difference is that nothing here blocks. Arduino's `step()` busy-waits
// through `delayMicroseconds()` for the whole move, which would freeze the
// serial console and the HTTP server for as long as the motor ran -- and with
// the dead-man watching, a frozen HTTP server means a stopped motor. An
// esp_timer periodic advances one phase per tick instead, so "on" means
// "turning" rather than "the firmware is inside a loop".
//
// The stepper and the actuator must never run together -- one mechanism, one
// supply. That interlock is NOT here: it lives in `controller`, the one place
// every client's commands pass through. See controller.c.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "board_config.h"

#define STEPPER_STEPS_PER_REV  BOARD_STEPPER_STEPS_PER_REV
#define STEPPER_RPM            BOARD_STEPPER_RPM
#define STEPPER_PHASES         4

// Arduino's setSpeed() arithmetic, which is what made the sketch's motion what
// it was: 60 s / 200 steps / 100 rpm = 3000 us per step.
#define STEPPER_STEP_INTERVAL_US  (60UL * 1000000UL / STEPPER_STEPS_PER_REV / STEPPER_RPM)

void stepper_motor_init(void);

// Turn continuously until stopped. Returns false, changing nothing, when it is
// already running -- so a repeated tap on the web button is not an error.
bool stepper_motor_start(void);

// Stop and release the coils. Returns false when it was already idle.
//
// Releasing matters: full-step energises both coils in every phase, so a
// stopped-but-still-driven motor would sit at its rated current indefinitely,
// heating the windings and the driver for a holding torque this mechanism does
// not need. The cost is that the shaft is free to be turned by hand when off.
bool stepper_motor_stop(void);

bool     stepper_motor_running(void);

// Steps taken since the current run started, for the status page. Reset by
// stepper_motor_start(), not by stop, so the count survives being read after
// the motor has stopped.
uint64_t stepper_motor_steps(void);
