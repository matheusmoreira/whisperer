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

#ifndef WHISPERER_COMMAND_HEADER
#define WHISPERER_COMMAND_HEADER

#include <stdio.h>

typedef unsigned int (*whisperer_command_function)(size_t count, const char **arguments, void *context);

struct whisperer_command {
	const char *name;
	whisperer_command_function function;
};

struct whisperer_commands {
	void *context;
	struct whisperer_command *list;
};

int whisperer_command_vector(struct whisperer_commands *commands, const char **arguments);
int whisperer_command_line(struct whisperer_commands *commands, const char *line);
int whisperer_command_file(struct whisperer_commands *commands, FILE *input);

#endif /* WHISPERER_COMMAND_HEADER */
