#ifndef WHISPERER_DEVICES_ITE_829X_HEADER
#define WHISPERER_DEVICES_ITE_829X_HEADER

#include <whisperer/command.h>
#include <whisperer/devices/ite.h>

/* ITE Device(829x) */
#define WHISPERER_DEVICES_ITE_829X_PID 0x8910

int whisperer_devices_ite_829x_probe(struct whisperer_commands *commands);
int whisperer_devices_ite_829x_close(void *context);

#endif /* WHISPERER_DEVICES_ITE_829X_HEADER */
