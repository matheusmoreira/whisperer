#include <stdlib.h>

#include <hidapi/hidapi.h>

#include <whisperer/devices/ite/829x.h>

#define VID WHISPERER_DEVICES_ITE_VID
#define PID WHISPERER_DEVICES_ITE_829X_PID

struct ite_829x {
	hid_device *keyboard;
	unsigned char current_brightness;
	unsigned char current_speed;
};

/* Clevo Control Center
 * Brightness and effect speed
 * Wireshark Leftover Capture Data
 *
 * 	          	cc090r0s00007f
 *
 * 	brightness	r			[0x0, 0xA]
 * 	speed     	s			[0x0, 0x2]
 *
 * Brightness seems to be scale from 1 to 10 with 0 being off.
 * Values above 0x0A seem to have the same effect as 0x0A.
 *
 * Speed controls how fast the keyboard's effects animate.
 * Seems to be a scale from 0 to 2.
 */
static
unsigned int
set_brightness_and_speed_common(struct ite_829x *ite_829x)
{
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	if (ite_829x->current_brightness > 0x0A)
		ite_829x->current_brightness = 0x0A;

	if (ite_829x->current_speed > 0x02)
		ite_829x->current_speed = 0x02;

	const unsigned char report[] = {
		0xCC, 0x09,
		ite_829x->current_brightness, ite_829x->current_speed,
		0x00, 0x00,
		0x7F
	};

	if (hid_send_feature_report(ite_829x->keyboard, report, sizeof(report)) == -1)
		return 1;

	return 0;
}

static
unsigned int
set_brightness_and_speed(const size_t count, const char **arguments, void *context)
{
	struct ite_829x *ite_829x = context;
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	if (count < 2)
		return 2;

	ite_829x->current_brightness = atoi(arguments[0]);
	ite_829x->current_speed      = atoi(arguments[1]);

	return set_brightness_and_speed_common(ite_829x);
}

/* Clevo Control Center
 * Brightness
 * Wireshark Leftover Capture Data
 *
 * 	Off	cc09000000007f
 * 	  1	cc09020200007f
 * 	  2	cc09040200007f
 * 	  3	cc09060200007f
 * 	  4	cc090a0200007f
 *
 * Other brightness values are also valid: 1, 3, 5, 7, 8, 9.
 * Values above 0x0A seem to have the same effect as 0x0A.
 * Seems to be a scale from 1 to 10 with 0 being off.
 */
static
unsigned int
set_brightness(const size_t count, const char **arguments, void *context)
{
	struct ite_829x *ite_829x = context;
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	if (count < 1)
		return 2;

	ite_829x->current_brightness = atoi(*arguments);

	return set_brightness_and_speed_common(ite_829x);
}

/* Clevo Control Center
 * Speed
 * Wireshark Leftover Capture Data
 *
 * 	1	cc090a0000007f
 * 	2	cc090a0100007f
 * 	3	cc090a0200007f
 */
static
unsigned int
set_speed(const size_t count, const char **arguments, void *context)
{
	struct ite_829x *ite_829x = context;
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	if (count < 1)
		return 2;

	ite_829x->current_speed = atoi(*arguments);

	return set_brightness_and_speed_common(ite_829x);
}

/* Clevo Control Center
 * Effects
 * Wireshark Leftover Capture Data
 *
 * 	0	Wave    	cc00040000007f
 * 	1	Breathe 	cc0a000000007f
 * 	2	Scan    	cc000a0000007f
 * 	3	Blink   	cc0b000000007f
 * 	4	Random  	cc000900000000
 * 	5	Ripple  	cc070000000000
 * 	6	Snake   	cc000b00000053
 *
 * There seems to be no pattern to it. Does the value of the last byte matter?
 * My keyboard apparently doesn't support the ripple effect,
 * even though it is present in the Clevo Control Center interface.
 */
static
unsigned int
set_effects(const size_t count, const char **arguments, void *context)
{
	struct ite_829x *ite_829x = context;
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	if (count < 1)
		return 2;

	const unsigned char effect = atoi(*arguments);

	unsigned char effect1, effect2, last = 0x7F;

	switch (effect) {
	default:
		return -2;
	case 0:
		effect1 = 0x00;
		effect2 = 0x04;
		break;
	case 1:
		effect1 = 0x0A;
		effect2 = 0x00;
		break;
	case 2:
		effect1 = 0x00;
		effect2 = 0x0A;
		break;
	case 3:
		effect1 = 0x0B;
		effect2 = 0x00;
		break;
	case 4:
		effect1 = 0x00;
		effect2 = 0x09;
		last  = 0x00;
		break;
	case 5:
		effect1 = 0x07;
		effect2 = 0x00;
		last  = 0x00;
		break;
	case 6:
		effect1 = 0x00;
		effect2 = 0x0B;
		last  = 0x53;
		break;
	}

	const unsigned char report[] = {
		0xCC,
		effect1, effect2,
		0x00, 0x00, 0x00,
		last
	};

	if (hid_send_feature_report(ite_829x->keyboard, report, sizeof(report)) == -1)
		return 1;

	return 0;
}

/* Resets the keyboard back to a clean state.
 * Turns off all LEDs and clears their color configuration.
 * Stops any keyboard effects.
 *
 * Clevo Control Center sends this report when the user switches to normal mode
 * from effects mode. It follows up with over a hundred reports
 * that reconfigure the LED colors for each individual key.
 *
 * 	cc000c0000007f
 *
 */
static
unsigned int
reset(const size_t count, const char **arguments, void *context)
{
	struct ite_829x *ite_829x = context;
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	const unsigned char report[] = {
		0xCC,
		0x00, 0x0C,
		0x00, 0x00, 0x00,
		0x7F
	};

	if (hid_send_feature_report(ite_829x->keyboard, report, sizeof(report)) == -1)
		return 1;

	return 0;
}

/* Clevo Control Center
 * RGB color of LEDs
 * Wireshark Leftover Capture Data
 *
 * 	cc01000000007f
 * 	cc01llrrggbb7f
 *
 * ll = which LED to configure
 * rr = red
 * gg = green
 * bb = blue
 *
 * LED codes:
 *
 *   0 - 19 	Escape -> PageDown
 *  32 - 43 	Tilde -> Minus
 *  45 - 51 	Equals -> KeypadMinus
 *  64 - 83 	Tab1 -> KeypadPlus1
 *  96 - 108	CapsLock1 -> SingleQuote
 * 110 - 115	Enter1 -> KeypadPlus2
 * 128      	Shift1
 * 130 - 147	Shift2 -> KeypadEnter1
 * 160 - 165	Ctrl1 -> Spacebar1
 * 169 - 179	Spacebar2 -> KeypadEnter2
 *
 * 102 keys.
 * 114 LEDs.
 *
 * Sets the color of each individual LED.
 * Some keys are lit by more than one LED.
 * All of them can be controlled individually.
 * After resetting the keyboard, all keys must be reconfigured.
 */
static
unsigned int
set_led_color(const size_t count, const char **arguments, void *context)
{
	struct ite_829x *ite_829x = context;
	if (!ite_829x || !ite_829x->keyboard)
		return 3;

	if (count < 4)
		return 2;

	const unsigned char led = atoi(arguments[0]);
	const unsigned char r   = atoi(arguments[1]);
	const unsigned char g   = atoi(arguments[2]);
	const unsigned char b   = atoi(arguments[3]);

	const unsigned char report[] = {
		0xCC,
		0x01, led,
		r, g, b,
		0x7F
	};

	if (hid_send_feature_report(ite_829x->keyboard, report, sizeof(report)) == -1)
		return 1;

	return 0;
}

int
whisperer_devices_ite_829x_probe(struct whisperer_commands *commands)
{
	hid_device *keyboard = hid_open(VID, PID, NULL);
	if (keyboard == NULL) {
		fprintf(stderr, "Could not open keyboard [%04x:%04x]\n", VID, PID);
		return 2;
	}

	static struct ite_829x ite_829x = { 0, 0, 0 };
	static struct whisperer_command list[] = {
		{ "brightness+speed", set_brightness_and_speed },
		{ "brightness",       set_brightness,          },
		{ "speed",            set_speed,               },
		{ "effects",          set_effects,             },
		{ "reset",            reset,                   },
		{ "led",              set_led_color,           },
		{ 0 }
	};

	ite_829x.keyboard = keyboard;
	commands->context = &ite_829x;
	commands->list = list;

	return 0;
}

int
whisperer_devices_ite_829x_close(void *context)
{
	struct ite_829x *ite_829x = context;
	hid_close(ite_829x->keyboard);
	return 0;
}
