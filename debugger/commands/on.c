#include "commands.h"
#include "debugger.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

enum {
	REGISTER,
	MEMORY,
	EXECUTION,
};

enum {
	READ,
	WRITE
};
int register_from_string(char *string) {
	#define REGISTER(num, len, print) \
		if (strncasecmp(string, print, len) == 0) {\
			return num; \
		}
	REGISTER(A, 1, "A");
	REGISTER(B, 1, "B");
	REGISTER(C, 1, "C");
	REGISTER(D, 1, "D");
	REGISTER(E, 1, "E");
	REGISTER(F, 1, "F");
	REGISTER(H, 1, "H");
	REGISTER(L, 1, "L");
	REGISTER(AF, 2, "AF");
	REGISTER(_AF, 3, "AF'");
	REGISTER(BC, 2, "BC");
	REGISTER(_BC, 3, "BC'");
	REGISTER(DE, 2, "DE");
	REGISTER(_DE, 3, "DE'");
	REGISTER(HL, 2, "HL");
	REGISTER(_HL, 3, "HL'");
	REGISTER(PC, 2, "PC");
	REGISTER(SP, 2, "SP");
	REGISTER(I, 1, "I");
	REGISTER(R, 1, "R");
	REGISTER(IXH, 3, "IXH");
	REGISTER(IXL, 3, "IXL");
	REGISTER(IX, 2, "IX");
	REGISTER(IYH, 3, "IYH");
	REGISTER(IYL, 3, "IYL");
	REGISTER(IY, 2, "IY");

	return 0;
}

typedef struct {
	int look_for;
	struct debugger_state deb_sta;
	debugger_command_t *command;
	int argc;
	char **argv;
} on_state_t;

int command_on(struct debugger_state *state, int argc, char **argv) {
	if (argc < 5) {
		printf("%s `type` `read|write` `value` `command` [command args]\n"
			" Run a command on a specific case\n"
			" The type can be memory / register / execute\n"
			" The value can be ALL, a register, or an expression\n",
			argv[0]);
	}

	int thing = 0;

	if (strncasecmp(argv[2], "read", 4) == 0) {
		thing = READ;
	}
	
	if (strncasecmp(argv[2], "write", 4) == 0) {
		thing = WRITE;
	}

	char **new_argv = malloc(sizeof(char *) * argc - 3);
	int new_argc = argc - 4;
	on_state_t *sta = malloc(sizeof(on_state_t));
	find_best_command(argv[4], &sta->command);
	sta->deb_sta = state->create_new_state(state, sta->command->state, argv[4]);
	sta->argc = new_argc;
	sta->argv = new_argv;
	int i = 0;
	for (i = 0; i < new_argc; i++) {
		int size = strlen(argv[4 + i]) + 1;
		new_argv[i] = malloc(size);
		memcpy(new_argv[i], argv[4 + i], size);
	}

	if (strncasecmp(argv[1], "register", 8) == 0) {
		sta->look_for = register_from_string(argv[3]);
	} else if (strncasecmp(argv[1], "memory", 6) == 0) {
		sta->look_for = parse_expression(state, argv[3]);
	}

	return 0;
}

struct break_data {
	uint16_t address;
	asic_t *asic;
};

void break_callback(struct break_data *data, uint16_t address) {
	if(data->address != address) {
		return;
	}

	data->asic->state->stopped = 1;
}

int command_break(struct debugger_state *state, int argc, char **argv) {
	if (argc != 2) {
		state->print(state, "%s `address` - break at address\n", argv[0]);
		return 0;
	}

	uint16_t address = parse_expression(state, argv[1]);

	struct break_data *data = malloc(sizeof(struct break_data));
	data->address = address;
	data->asic = state->asic;

	hook_add_before_execution(state->asic->hook, data, (hook_execution_callback)break_callback);
	return 0;
}
