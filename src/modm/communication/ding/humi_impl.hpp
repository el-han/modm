// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_HUMI_HPP
#   error "Don't include this file directly, use 'ding_humi.hpp' instead!"
#endif

#include "humi.hpp"


template <typename NRF24, typename I2cMaster>
ding::HumiData ding::Humi<NRF24, I2cMaster>::current_data;

ding::HumiMessage::HumiMessage(uint8_t destination_id, uint8_t source_id, HumiData data)
{
	payload_bytes = sizeof(HumiData);
	destination   = destination_id;
	source        = source_id;
	type          = ding::MessageType::Humi;

	payload[0] = (uint8_t)((data.temperature >> 0) & 0x00ff);
	payload[1] = (uint8_t)((data.temperature >> 8) & 0x00ff);
	payload[2] = (uint8_t)((data.humidity >> 0) & 0x00ff);
	payload[3] = (uint8_t)((data.humidity >> 8) & 0x00ff);
	payload[4] = (uint8_t)((data.pressure >> 0) & 0x00ff);
	payload[5] = (uint8_t)((data.pressure >> 8) & 0x00ff);
}

ding::HumiMessage::HumiMessage(const ding::Message& message)
{
	payload_bytes = message.payload_bytes;
	destination   = message.destination;
	source        = message.source;
	type          = message.type;
	payload       = message.payload;
}

ding::HumiData ding::HumiMessage::getHumiData()
{
	int16_t  temperature = payload[0] | (((uint16_t)payload[1]) << 8);
	uint16_t humidity    = payload[2] | (((uint16_t)payload[3]) << 8);
	uint16_t pressure    = payload[4] | (((uint16_t)payload[5]) << 8);

	return ding::HumiData(temperature, humidity, pressure);
}

template <typename NRF24, typename I2cMaster>
void
ding::Humi<NRF24, I2cMaster>::begin(modm::Bme280<I2cMaster>& bme)
{
	RF_CALL_BLOCKING(bme.initialize(
		modm::Bme280<I2cMaster>::Mode::Forced,
		modm::Bme280<I2cMaster>::Oversampling::Sexdecuple,
		modm::Bme280<I2cMaster>::Oversampling::Sexdecuple,
		modm::Bme280<I2cMaster>::Oversampling::Sexdecuple
	));
}

template <typename NRF24, typename I2cMaster>
void ding::Humi<NRF24, I2cMaster>::measure(modm::Bme280<I2cMaster>& bme)
{
		RF_CALL_BLOCKING(bme.force());
		RF_CALL_BLOCKING(bme.readout());

		current_data.temperature = bme.getData().getTemperature();
		current_data.humidity    = bme.getData().getHumidity()/10;
		current_data.pressure    = bme.getData().getPressure()/10000;
}

template <typename NRF24, typename I2cMaster>
void ding::Humi<NRF24, I2cMaster>::send()
{
	ding::HumiMessage message = ding::HumiMessage(0x00, ding::Humi<NRF24, I2cMaster>::device_id, current_data);
	Ding<NRF24>::send(message);
}
