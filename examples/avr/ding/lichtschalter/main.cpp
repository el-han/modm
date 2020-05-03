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
#include <modm/board.hpp>
#include <modm/architecture/interface/interrupt.hpp>

#include <avr/sleep.h>
#include <avr/wdt.h>

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

using namespace modm::platform;

constexpr uint16_t network = 0xA1A2;
constexpr uint8_t  device  = 0xA0;
constexpr uint8_t  channel = 96;


// Raduino
typedef GpioB1 Ce;
typedef GpioB2 Csn;
typedef GpioD2 InterruptPin;

typedef GpioB5 Sck;
typedef GpioB3 Mosi;
typedef GpioB4 Miso;

typedef GpioC5 S1;
typedef GpioC4 S2;
typedef GpioC0 S3;
typedef GpioC1 S4;
typedef GpioC2 S5;
typedef GpioC3 S6;

typedef modm::Nrf24Phy<SpiMaster, Csn, Ce> Radio;

typedef ding::Ding<Radio> Schalter;

volatile bool sent, lost = false;

void
handleInterrupt()
{
	uint8_t status = Radio::readStatus();

	if (status & (uint8_t)Radio::Status::MAX_RT) {
		Radio::flushTxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::MAX_RT);
		lost = true;
	}

	if (status & (uint8_t)Radio::Status::TX_DS) {
		// Radio::flushTxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::TX_DS);
		sent = true;
	}

	if (status & (uint8_t)Radio::Status::RX_DR) {
		// Radio::flushRxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::RX_DR);
	}
}

void handleButton()
{
	// debounce
	disableInterrupts();

	if (!S1::read())
		Schalter::send(ding::SceneMessage(device, 1));
	if (!S2::read())
		Schalter::send(ding::SceneMessage(device, 2));
	if (!S3::read())
		Schalter::send(ding::SceneMessage(device, 3));
	if (!S4::read())
		Schalter::send(ding::SceneMessage(device, 4));
	if (!S5::read())
		Schalter::send(ding::SceneMessage(device, 5));
	if (!S6::read())
		Schalter::send(ding::SceneMessage(device, 6));

	modm::delayMilliseconds(100);
	enableInterrupts();
}

void sleepMode()
{
	// Set full power-down sleep mode and go to sleep.
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_mode();

	// Chip is now asleep!

	// Once awakened by the watchdog execution resumes here.  Start by disabling
	// sleep.
	sleep_disable(); // TODO brauche ich das? Doku sagt, sleep_mode() macht das schon.
}

int
main()
{
	systemClock::enable();

	enableInterrupts();

	Adc::initialize<SystemClock, 115_kHz>();
	Adc::setReference(Adc::Reference::InternalVcc);

	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	// Enable SPI
	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();
	SpiMaster::initialize<Board::SystemClock, 500000, 10_pct>();

	Schalter::initializeRadio(channel);

	InterruptPin::setInput(InterruptPin::InputType::PullUp);
	InterruptPin::enableExternalInterrupt();
	InterruptPin::setInputTrigger(InterruptPin::InputTrigger::LowLevel);

	S1::setInput(Gpio::InputType::PullUp);
	S2::setInput(Gpio::InputType::PullUp);
	S3::setInput(Gpio::InputType::PullUp);
	S4::setInput(Gpio::InputType::PullUp);
	S5::setInput(Gpio::InputType::PullUp);
	S6::setInput(Gpio::InputType::PullUp);

	S1::enablePcInterrupt();
	S2::enablePcInterrupt();
	S3::enablePcInterrupt();
	S4::enablePcInterrupt();
	S5::enablePcInterrupt();
	S6::enablePcInterrupt();

	Schalter::setAddress(network, device);

	Schalter::startTX();

	while (true)
	{
		uint16_t adcvalue = Adc::readChannel(14);
		uint16_t voltage = adcvalue != 0 ? (1100L * 1023) / adcvalue : 0;

		bool success = true; // only one transmission

		do {

			Schalter::send(ding::BatteryMessage(device, voltage));

			while (!sent && !lost)
				;

			if (sent)
				success = true;

			sent = false;
			lost = false;
		} while (!success);

		modm::delayMilliseconds(100);
		sleepMode();
	}
}

MODM_ISR(INT0)
{
	handleInterrupt();
}

MODM_ISR(PCINT1)
{
	S1::acknowledgePcInterruptFlag();
	handleButton();
}
