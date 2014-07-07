#include "runloop.h"

#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct {
	int cycles_until_tick;
} timer_info_t;

struct runloop_state {
	asic_t *asic;
	clock_t last_end;
	int spare_cycles;
	timer_info_t timers_info[ASIC_TIMER_MAX];
};

typedef struct {
	int index;
	int after_cycle;
} timer_tick_t;

runloop_state_t *runloop_init(asic_t *asic) {
	runloop_state_t *state = malloc(sizeof(runloop_state_t));

	state->asic = asic;
	state->last_end = clock();

	int i;
	for (i = 0; i < ASIC_TIMER_MAX; i++) {
		timer_info_t *info = &state->timers_info[i];
		z80_timer_t *timer = &state->asic->timers[i];
		if (timer->frequency < 0) {
			info->cycles_until_tick = asic->clock_rate / timer->frequency;
		}
	}

	return state;
}

void runloop_tick(runloop_state_t *state) {
	clock_t now = clock();
	clock_t ticks_between = now - state->last_end;

	float seconds = ticks_between / CLOCKS_PER_SEC;
	int cycles = seconds * state->asic->clock_rate - state->spare_cycles;
	int total_cycles = 0;

	timer_tick_t ticks[ASIC_TIMER_MAX];
	int current_tick = 0;

	int cycles_until_next_tick = cycles;

	int i;
	for (i = 0; i < state->asic->max_timer; i++) {
		z80_timer_t *timer = &state->asic->timers[i];
		timer_info_t *info = &state->timers_info[i];
		if (info->cycles_until_tick < cycles) {
			ticks[current_tick].index = i;
			ticks[current_tick].after_cycle = info->cycles_until_tick;
			info->cycles_until_tick = state->asic->clock_rate / timer->frequency;
			if (current_tick == 0) {
				cycles_until_next_tick = info->cycles_until_tick;
			}
			current_tick++;
		}
	}

	int tick_i = 0;
	while (cycles > 0) {
		int ran = cycles_until_next_tick + cpu_execute(state->asic->cpu, cycles_until_next_tick);
		total_cycles += ran;
		cycles -= ran;

		if (i < current_tick) {
			cycles_until_next_tick = state->timers_info[ticks[++tick_i].index].cycles_until_tick - total_cycles;
		} else {
			break; // something went very, very, wrong
		}
	}

	state->last_end = now;
}
