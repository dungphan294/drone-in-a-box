// Dead-reckoned positioning: the stroke is split into LEVELS steps and timed.
//
// There is no position feedback on this rig, so level is inferred from how
// long the motor has run. That is only valid when every timed move runs at the
// same speed (STEP_SPEED_PCT) from a known datum, which homing establishes.
// Any manual jog must call positioner_cancel().
//
// Moves are non-blocking: an esp_timer one-shot stops the motor when the step
// is due, so the console stays responsive and nothing sleeps or spins.
#pragma once

#include <stdbool.h>

// MEASURE THIS on your actuator: press 'h', then time 't' with a stopwatch.
// The value below is a placeholder and every level depends on it.
#define TRAVEL_MS       10000               // full stroke, ms, at STEP_SPEED_PCT
#define LEVELS          5
#define STEP_SPEED_PCT  100                 // timed moves always run at this speed
#define STEP_MS         (TRAVEL_MS / LEVELS)
#define HOME_MS         ((TRAVEL_MS * 13) / 10)   // 30% overtravel into the end stop

#define LEVEL_UNKNOWN   (-1)

typedef enum {
    POS_OK,             // move started
    POS_UNKNOWN,        // level not established: home first
    POS_AT_END,         // already at the requested end of travel
    POS_BUSY,           // a move is still running; the level is not settled yet
} pos_result_t;

// Called from the esp_timer task when a timed move is due to finish. It does
// not finish the move itself: the callee takes whatever lock guards the motor
// and then calls positioner_complete(), so finishing a move can never
// interleave with a command arriving on another task.
typedef void (*positioner_due_cb_t)(void);

void positioner_init(positioner_due_cb_t on_due);

// Finish the move that is due: stop the motor and settle the level. Returns
// false, doing nothing, when no move is due -- the timer callback had already
// been dispatched when a cancel or a newer move overtook it, and
// esp_timer_stop() cannot recall it.
bool positioner_complete(void);

// 'h' = retracted end (level 0), 't' = extended end (level LEVELS).
// Returns false, changing nothing, when a move to that same end is already
// running: a repeated tap must not restart the 13 s timer each time.
bool positioner_go_end(char which);

// step is +1 or -1. On POS_OK, *from and *to carry the levels for reporting.
pos_result_t positioner_step(int step, int *from, int *to);

// Abort any move in flight and mark the level unknown. Call on every manual
// jog, because dead reckoning no longer holds once the motor is driven by hand.
void positioner_cancel(void);

int  positioner_level(void);
bool positioner_moving(void);
