// coding: utf-8
/* Copyright (c) 2018, Hannes Ellinger
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_HPP
#   error "Don't include this file directly, use 'ding.hpp' instead!"
#endif

#include "ding.hpp"


template <typename NRF24>
uint16_t ding::Ding<NRF24>::network_id;

template <typename NRF24>
uint8_t ding::Ding<NRF24>::device_id;

template <typename NRF24>
void ding::Ding<NRF24>::initializeRadio(uint8_t channel)
{
	NRF24::resetCe();
	NRF24::initialize(0);

	NRF24::flushRxFifo();
	NRF24::flushTxFifo();

	NRF24::clearInterrupt(NRF24::InterruptFlag::ALL);

	NRF24::writeRegister(Register::SETUP_RETR, 0xff);

	NRF24::writeRegister(Register::RF_CH, channel);

	// NRF24::writeRegister(Register::SETUP_AW, 0x01);

	// NRF24::writeRegister(0x11); // RX Pipe
	NRF24::writeRegister(Register::DYNPD, 0x03);   // enable dynamic payload
	NRF24::writeRegister(Register::FEATURE, 0x04); // enable dynamic payload on pipe 0 and 1
}

template <typename NRF24>
void ding::Ding<NRF24>::setAddress(uint16_t network, uint8_t device)
{
	network_id = network;
	device_id  = device;

	NRF24::setRxAddress(Pipe::PIPE_0, ((uint32_t)network_id << 8) | 0xff);
	NRF24::setRxAddress(Pipe::PIPE_1, ((uint32_t)network_id << 8) | device_id);

	NRF24::writeRegister(Register::EN_AA, 0x03);
	NRF24::writeRegister(Register::EN_RX_ADDR, 0x03);
}


template <typename NRF24>
void ding::Ding<NRF24>::startTX()
{
	NRF24::resetCe();

	// Power up
	NRF24::clearBits(Register::CONFIG, Config::PRIM_RX);
	NRF24::setBits(Register::CONFIG, Config::PWR_UP);
	modm::delayMicroseconds(1500);
}

template <typename NRF24>
void ding::Ding<NRF24>::startRX()
{
	NRF24::resetCe();

	// Power up
	NRF24::setBits(Register::CONFIG, Config::PRIM_RX);
	NRF24::setBits(Register::CONFIG, Config::PWR_UP);
	modm::delayMicroseconds(1500);

	NRF24::setCe();
}

template <typename NRF24>
void ding::Ding<NRF24>::send(const ding::Message& message)
{
	NRF24::setTxAddress(((uint32_t)network_id << 8) | message.destination);
	NRF24::setRxAddress(Pipe::PIPE_0, ((uint32_t)network_id << 8) | message.destination);

	NRF24::writeTxPayload(message.getPacket().ptr(), message.payload_bytes+3);

	// pulse Ce
	NRF24::setCe();
	modm::delayMicroseconds(15);
	NRF24::resetCe();
}

template <typename NRF24>
ding::Message ding::Ding<NRF24>::receive()
{
	ding::Message message;

	modm::Vector<uint8_t, 32> packet;

	NRF24::resetCe();

	size_t packet_bytes = NRF24::readRxPayloadWidth();
	if (packet_bytes > 3 && packet_bytes <= 32) {
		NRF24::readRxPayload(packet.ptr());
		message = ding::Message(packet_bytes-3, packet);
	} else {
		NRF24::flushRxFifo();
		message = ding::Message();
	}

	NRF24::setCe();

	if (message.destination != device_id)
		message = ding::Message();

	return message;
}
