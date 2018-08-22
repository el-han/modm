// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/driver/radio/nrf24/nrf24_phy.hpp>
#include <modm/communication/ding/ding.hpp>
#include <modm/communication/ding/message.hpp>
#include <modm/communication/ding/licht.hpp>
#include <modm/board/board.hpp>
#include <modm/driver/gpio/pwm.hpp>
#include <modm/architecture/interface/interrupt.hpp>

using namespace modm::platform;

constexpr uint16_t network = 0xA1A2;
constexpr uint8_t  device  = 0xb0;
constexpr uint8_t  channel = 96;

typedef GpioPwmD6 Red;
typedef GpioPwmD5 Green;
typedef GpioPwmB1 Blue;
typedef GpioPwmB2 White;

typedef D8 Ce;
typedef D7 Csn;
typedef D3 InterruptPin;

typedef modm::Nrf24Phy<SpiMaster, Csn, Ce> Radio;

typedef ding::Licht<Radio, Red, Green, Blue, White> RGBWLicht;

void
handleInterrupt()
{
	uint8_t status = Radio::readStatus();

	if (status & (uint8_t)Radio::Status::MAX_RT) {
		Radio::flushTxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::MAX_RT);
	}

	if (status & (uint8_t)Radio::Status::TX_DS) {
		Radio::clearInterrupt(Radio::InterruptFlag::TX_DS);
	}

	if (status & (uint8_t)Radio::Status::RX_DR) {
		Radio::clearInterrupt(Radio::InterruptFlag::RX_DR);
	}
}

int
main()
{
	RGBWLicht::begin();

	InterruptPin::setInput(Gpio::InputType::PullUp);
	InterruptPin::enableExternalInterrupt();
	InterruptPin::setInputTrigger(InterruptPin::InputTrigger::LowLevel);

	enableInterrupts();

	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	// Enable SPI
	SpiMaster::connect<D13::Sck, D11::Mosi, D12::Miso>();
	SpiMaster::initialize<modm::platform::SystemClock, 625000, modm::Tolerance::Exact>();

	RGBWLicht::initializeRadio(channel);

	RGBWLicht::setAddress(network, device);

	RGBWLicht::startRX();

	RGBWLicht::fade(0, 0, 0, 0);

	while (1)
	{
		while (RGBWLicht::available()) {
			RGBWLicht::receive();
		}
		RGBWLicht::fade();
	}
}

MODM_ISR(INT1)
{
	handleInterrupt();
}
