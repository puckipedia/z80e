#ifndef ASIC_H
#define ASIC_H
#include "cpu.h"
#include "memory.h"
#include "ti.h"
#include <stdint.h>

typedef enum {
    BATTERIES_REMOVED,
    BATTERIES_LOW,
    BATTERIES_GOOD
} battery_state;

typedef struct asic asic_t;

typedef void (*timer_tick)(asic_t *, void *);

#define ASIC_TIMER_MAX 16

typedef struct cpu_timer z80_timer_t;

struct cpu_timer {
    int frequency;
    void *timer_data;
    timer_tick on_tick;
};

struct asic {
    z80cpu_t* cpu;
    ti_device_type device;
    ti_mmu_t* mmu;
    battery_state battery;
    int battery_remove_check;
    int clock_rate;
    z80_timer_t timers[ASIC_TIMER_MAX];
    int max_timer;
};

asic_t* asic_init(ti_device_type);
void asic_free(asic_t*);

#endif
