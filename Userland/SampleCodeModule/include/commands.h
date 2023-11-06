#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

void cmd_init();
int cmd_execute(char* buf, uint32_t len);

#endif
