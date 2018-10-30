// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_HUMI_HPP
#define MODM_DING_HUMI_HPP

#include <stdint.h>
#include <modm/driver/pressure/bme280.hpp>

#include "ding.hpp"
#include "message.hpp"

namespace ding {

class HumiData {
public:
	int16_t  temperature;
	uint16_t humidity;
	uint16_t pressure;

	constexpr HumiData(int16_t t, uint16_t h, uint16_t p) : temperature(t), humidity(h), pressure(p) {};
	constexpr HumiData() : temperature(0), humidity(0), pressure(0) {};

	bool operator==(const HumiData& p) const
	{
		return (this->temperature==p.temperature) && (this->humidity==p.humidity) && (this->pressure==p.pressure);
	}
	bool operator!=(const HumiData& p) const
	{
		return (this->temperature!=p.temperature) || (this->humidity!=p.humidity) || (this->pressure!=p.pressure);
	}
};

class HumiMessage : public ding::Message {
public:

	HumiMessage(uint8_t destination_id, uint8_t source_id, HumiData data);
	HumiMessage(const ding::Message& message);

	HumiData getHumiData();
};

template <typename NRF24, typename I2cMaster>
class Humi : public Ding<NRF24>
{
public:
	static void begin(modm::Bme280<I2cMaster>& bme);
	static void measure(modm::Bme280<I2cMaster>& bme);
	static void send();

protected:
	static HumiData current_data;
};

} // namespace ding

#include "humi_impl.hpp"

#endif
