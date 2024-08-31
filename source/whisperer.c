/* SPDX-License-Identifier: AGPL-3.0-or-later */

/* Copyright © 2019-2024 Matheus Afonso Martins Moreira
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy
 * of the GNU Affero General Public License
 * along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 *
 */

#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <hidapi/hidapi.h>

#include <whisperer/cmd.h>
#include <whisperer/device.h>
#include <whisperer/devices/ite/829x.h>

int whisperer_probe(struct whisperer_commands *commands)
{
	int code;

	code = whisperer_devices_ite_829x_probe(commands);
	return code;
}

int whisperer_close(void *context)
{
	whisperer_devices_ite_829x_close(context);

	return 0;
}

int whisperer_process(struct whisperer_commands *commands, const char **arguments, FILE *input)
{
	int code = whisperer_command_vector(commands, arguments);
	if (code != 0)
		return code;

	code = whisperer_command_file(commands, input);

	return code;
}

int main(int count, const char **arguments)
{
	struct whisperer_commands commands;
	int code;

	if (hid_init() == -1) {
		fputs("Error during hidapi-libusb initialization\n", stderr);
		return 1;
	}

	code = whisperer_probe(&commands);
	if (code != 0) {
		return code;
	}

	code = whisperer_process(&commands, arguments + 1, stdin);
	if (code != 0) {
		return code;
	}

	code = whisperer_close(commands.context);
	if (code != 0) {
		return code;
	}

	if (hid_exit() == -1) {
		fputs("Error during hidapi-libusb finalization\n", stderr);
		if (code == 0)
			return 4;
	}

	return code;
}
