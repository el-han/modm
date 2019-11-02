/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <cmath>
#include <inttypes.h>

#include <modm/debug/logger/logger.hpp>

#ifndef MODM_BME280_DATA_HPP
#	error  "Don't include this file directly, use 'bme280_data.hpp' instead!"
#endif

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

namespace modm
{
namespace bme280data
{

// Fixed point implementation

Data::Data() :
	calibratedPressure(0),
	t_fine(0)
{
}

void
Data::calculateCalibratedTemperature()
{
	int32_t var1;
	int32_t var2;
	int32_t temperature_min = -4000;
	int32_t temperature_max = 8500;

	int32_t adc = (int32_t)((((uint32_t)raw[3]) << 12) | (((uint16_t)raw[4]) << 4) | (raw[5] >> 4));

	MODM_LOG_DEBUG.printf("adc = 0x%05" PRIx32 "\r\n", adc);

	int32_t T1 = calibration.T1;
	int32_t T2 = calibration.T2;
	int32_t T3 = calibration.T3;

	var1 = (adc / 8) - (T1 * 2);
	var1 = (var1 * (T2)) / 2048;
	var2 = (adc / 16) - T1;
	var2 = (((var2 * var2) / 4096) * T3) / 16384;
	t_fine = var1 + var2;
	calibratedTemperature = (t_fine * 5 + 128) / 256;

	if (calibratedTemperature < temperature_min)
		calibratedTemperature = temperature_min;
	else if (calibratedTemperature > temperature_max)
	calibratedTemperature = temperature_max;

	meta |= TEMPERATURE_CALCULATED;
}

void
Data::calculateCalibratedPressure()
{
	int64_t var1;
	int64_t var2;
	int64_t var3;
	int64_t var4;
	int32_t pressure_min = 3000000;
	int32_t pressure_max = 11000000;

	// Before pressure can be calculated, temperature must be.
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}

	int32_t adc = (int32_t)((((uint32_t)raw[0]) << 12) | (((uint16_t)raw[1]) << 4) | (raw[2] >> 4));

	int64_t P1 = calibration.P1;
	int64_t P2 = calibration.P2;
	int64_t P3 = calibration.P3;
	int64_t P4 = calibration.P4;
	int64_t P5 = calibration.P5;
	int64_t P6 = calibration.P6;
	int64_t P7 = calibration.P7;
	int64_t P8 = calibration.P8;
	int64_t P9 = calibration.P9;

	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * P6;
	var2 = var2 + ((var1 * P5) * 131072);
	var2 = var2 + (P4 * 34359738368);
	var1 = ((var1 * var1 * P3) / 256) + ((var1 * P2 * 4096));
	var3 = ((int64_t)1) * 140737488355328;
	var1 = (var3 + var1) * P1 / 8589934592;

	/* To avoid divide by zero exception */
	if (var1 != 0) {
		var4 = 1048576 - adc;
		var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
		var1 = (P9 * (var4 / 8192) * (var4 / 8192)) / 33554432;
		var2 = (P8 * var4) / 524288;
		var4 = ((var4 + var1 + var2) / 256) + (P7 * 16);
		calibratedPressure = (uint32_t)(((var4 / 2) * 100) / 128);

		if (calibratedPressure < pressure_min)
			calibratedPressure = pressure_min;
		else if (calibratedPressure > pressure_max)
			calibratedPressure = pressure_max;
	} else {
		calibratedPressure = pressure_min;
	}

	meta |= PRESSURE_CALCULATED;
}

void
Data::calculateCalibratedHumidity()
{
	int32_t var1;
	int32_t var2;
	int32_t var3;
	int32_t var4;
	int32_t var5;
	int32_t humidity_max = 102400;

	// Before humidity can be calculated, temperature must be.
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}

	int32_t adc = (int32_t)((((uint16_t)raw[6]) << 8) | (raw[7] << 0));

	int32_t H1 = calibration.H1;
	int32_t H2 = calibration.H2;
	int32_t H3 = calibration.H3;
	int32_t H4 = calibration.H4;
	int32_t H5 = calibration.H5;
	int32_t H6 = calibration.H6;

	var1 = t_fine - ((int32_t)76800);
	var2 = adc * 16384;
	var3 = H4 * 1048576;
	var4 = H5 * var1;
	var5 = (((var2 - var3) - var4) + 16384) / 32768;
	var2 = (var1 * H6) / 1024;
	var3 = (var1 * H3) / 2048;
	var4 = ((var2 * (var3 + 32768)) / 1024) + 2097152;
	var2 = ((var4 * H2) + 8192) / 16384;
	var3 = var5 * var2;
	var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
	var5 = var3 - ((var4 * H1) / 16);
	var5 = (var5 < 0 ? 0 : var5);
	var5 = (var5 > 419430400 ? 419430400 : var5);
	calibratedHumidity = (uint32_t)(var5 / 4096);

	if (calibratedHumidity > humidity_max)
		calibratedHumidity = humidity_max;
}

int32_t
Data::getTemperature()
{
	if (not (meta & TEMPERATURE_CALCULATED)) {
		calculateCalibratedTemperature();
	}
	return calibratedTemperature;
}

int32_t
Data::getPressure()
{
	if (not (meta & PRESSURE_CALCULATED)) {
		calculateCalibratedPressure();
	}
	return calibratedPressure;
}

int32_t
Data::getHumidity()
{
	if (not (meta & HUMIDITY_CALCULATED)) {
		calculateCalibratedHumidity();
	}
	return calibratedHumidity;
}

} // bme280data namespace

} // modm namespace
