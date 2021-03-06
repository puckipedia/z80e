#ifndef DEBUGGER_HOOKS_H
#define DEBUGGER_HOOKS_H

#include <stdint.h>

typedef struct hook_info hook_info_t;

#include "asic.h"
#include "registers.h"

hook_info_t *create_hook_set(asic_t *asic);

// Memory hooks

uint8_t hook_on_memory_read(hook_info_t *, uint16_t address, uint8_t value);
uint8_t hook_on_memory_write(hook_info_t *, uint16_t address, uint8_t value);

typedef uint8_t (*hook_memory_callback)(void *data, uint16_t address, uint8_t value);

int hook_add_memory_read(hook_info_t *, uint16_t address_start, uint16_t address_end, void *data, hook_memory_callback);
int hook_add_memory_write(hook_info_t *, uint16_t address_start, uint16_t address_end, void *data, hook_memory_callback);

// Register hooks

uint16_t hook_on_register_read(hook_info_t *, registers flags, uint16_t value);
uint16_t hook_on_register_write(hook_info_t *, registers flags, uint16_t value);

typedef uint16_t (*hook_register_callback)(void *data, registers reg, uint16_t value);

int hook_add_register_read(hook_info_t *, registers flags, void *data, hook_register_callback);
int hook_add_register_write(hook_info_t *, registers flags, void *data, hook_register_callback);

// Execution hooks

void hook_on_before_execution(hook_info_t *, uint16_t address);
void hook_on_after_execution(hook_info_t *, uint16_t address);

typedef void (*hook_execution_callback)(void *data, uint16_t address);

int hook_add_before_execution(hook_info_t *, void *data, hook_execution_callback);
int hook_add_after_execution(hook_info_t *, void *data, hook_execution_callback);

#endif
