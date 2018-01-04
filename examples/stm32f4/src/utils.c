/*
 * utils.c
 *
 *  Created on: 11 jul 2012
 *      Author: benjamin
 */
#include "utils.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Can be used with printf when double values are not
 * supported (because hardfloat is used)
 */
char* ftostr(float value, int places) {
	static char buffer[100];
	uint32_t whole;
	uint32_t fraction;
	char sign[2] = "";

	if (value < 0) {
		value = -value;
		sign[0] = '-';
		sign[1] = '\0';
	}

	whole = (uint32_t) value;
	fraction = (uint32_t) ((value - floorf(value)) * powf(10.0f, (float)places) + 0.5f);
	sprintf(buffer, "%s%lu.%*.*lu", sign, whole, places, places, fraction);

	return buffer;
}
