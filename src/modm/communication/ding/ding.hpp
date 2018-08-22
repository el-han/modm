// coding: utf-8
/* Copyright (c) 2018, Hannes Ellinger
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_HPP
#define MODM_DING_HPP

#include <stdint.h>

#include <modm/driver/radio/nrf24/nrf24_phy.hpp>
#include <modm/driver/radio/nrf24/nrf24_definitions.hpp>
#include <modm/math/geometry/vector.hpp>
#include "message.hpp"

namespace ding
{

template <typename NRF24>
class Ding
{

typedef typename NRF24::NrfRegister Register;
typedef typename NRF24::NrfRegister_t Register_t;

typedef typename NRF24::Config Config;
typedef typename NRF24::Status Status;
typedef typename NRF24::Pipe Pipe;
typedef typename NRF24::FifoStatus FifoStatus;

public:

	static void initializeRadio(uint8_t channel);
	static void setAddress(uint16_t network, uint8_t device);

	static void startRX();
	static void startTX();

	static bool
	available() {
		return !(NRF24::readRegister(Register::FIFO_STATUS) & (uint8_t)FifoStatus::RX_EMPTY);
	}

	static void send(const ding::Message& message);
	static ding::Message receive();

protected:
	static uint16_t network_id;
	static uint8_t  device_id;
};

} // namespace ding

#include "ding_impl.hpp"

#endif
