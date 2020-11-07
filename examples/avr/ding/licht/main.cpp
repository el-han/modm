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
constexpr uint8_t  device  = 0xb5;
constexpr uint8_t  channel = 96;

/* Kochanie */
typedef GpioPwmD6 Red;
typedef GpioPwmD5 Green;
typedef GpioPwmB1 Blue;
typedef GpioPwmB2 White;

typedef D8 Ce;
typedef D7 Csn;
typedef D3 InterruptPin;

/* First Revision */
// typedef GpioPwmD5 Red;
// typedef GpioPwmB1 Green;
// typedef GpioPwmD6 Blue;
// typedef GpioPwmD3 White;

// typedef D8 Ce;
// typedef D10 Csn;
// typedef D2 InterruptPin;

typedef D13 Sck;
typedef D11 Mosi;
typedef D12 Miso;

typedef BitBangSpiMaster<Sck, Mosi, Miso> SpiRadio;

typedef modm::Nrf24Phy<SpiRadio, Csn, Ce> Radio;

typedef ding::Licht<Radio, Red, Green, Blue, White> RGBWLicht;

volatile bool received = false;

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
		RGBWLicht::receive();
		Radio::clearInterrupt(Radio::InterruptFlag::RX_DR);
		received = true;
	}
}

int
main()
{
	RGBWLicht::begin(ding::LichtData(255,100,0,0));

	enableInterrupts();

	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	// Enable SPI
	SpiRadio::connect<Sck::BitBang, Mosi::BitBang, Miso::BitBang>();
	SpiRadio::initialize<modm::platform::SystemClock, 500000, 10_pct>();

	RGBWLicht::initializeRadio(channel);

	InterruptPin::setInput(Gpio::InputType::PullUp);
	InterruptPin::enableExternalInterrupt();
	InterruptPin::setInputTrigger(InterruptPin::InputTrigger::LowLevel);

	RGBWLicht::setAddress(network, device);

	RGBWLicht::startRX();

	while (true)
	{
		if (received) {
			received = false;
			RGBWLicht::fade();
		}
	}
}

MODM_ISR(INT1)
{
	handleInterrupt();
}
