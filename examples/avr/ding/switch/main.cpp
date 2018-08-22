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
#include <modm/communication/ding/ding_message.hpp>
#include <modm/communication/ding/ding_licht.hpp>
#include <modm/board/board.hpp>
#include <modm/architecture/interface/interrupt.hpp>

#include <avr/sleep.h>
#include <avr/wdt.h>

using namespace modm::platform;

constexpr uint16_t network = 0xA1A2;
constexpr uint8_t  device  = 0xA3;
constexpr uint8_t  channel = 96;

constexpr ding::LichtData off   = ding::LichtData(0, 0, 0, 0);
constexpr ding::LichtData night = ding::LichtData(255, 31, 0, 0);
constexpr ding::LichtData day   = ding::LichtData(0, 0, 0, 255);

ding::LichtData message = off;

typedef D8 Ce;
typedef D10 Csn;
typedef D2 InterruptPin;
typedef D3 ButtonPin;

typedef modm::Nrf24Phy<SpiMaster, Csn, Ce> Radio;

typedef ding::Ding<Radio> Switch;

volatile bool button = false;
// volatile uint16_t downtime = 0;

void
handleInterrupt()
{
	uint8_t status = Radio::readStatus();

	if (status & (uint8_t)Radio::Status::MAX_RT) {
		Radio::flushTxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::MAX_RT);
	}

	if (status & (uint8_t)Radio::Status::TX_DS) {
		// Radio::flushTxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::TX_DS);

		if (message == off)
			message = night;
		else if (message == night)
			message = day;
		else
			message = off;

	}

	if (status & (uint8_t)Radio::Status::RX_DR) {
		// Radio::flushRxFifo();
		Radio::clearInterrupt(Radio::InterruptFlag::RX_DR);
	}
}

void handleButton()
{
	if (!ButtonPin::read()) {
// 		downtime = millis();
// 	} else {
// 		if ((millis() - downtime) > 1000) {
// 			state = 2;
// 		} else if ((millis() - downtime) > 400) {
// 			state = 0;
// 		} else {
// 			state = 1;
// 		}
		button = true;
	}
}

void enableWdtInterrupt()
{
	// Pick the closest appropriate watchdog timer value.
	int sleepWDTO = WDTO_1S;

	// Build watchdog prescaler register value before timing critical code.
	uint8_t wdps = ((sleepWDTO & 0x08 ? 1 : 0) << WDP3) |
	               ((sleepWDTO & 0x04 ? 1 : 0) << WDP2) |
	               ((sleepWDTO & 0x02 ? 1 : 0) << WDP1) |
	               ((sleepWDTO & 0x01 ? 1 : 0) << WDP0);

	// The next section is timing critical so interrupts are disabled.
	disableInterrupts();
	// First clear any previous watchdog reset.
	MCUSR &= ~(1<<WDRF);
	// Now change the watchdog prescaler and interrupt enable bit so the watchdog
	// reset only triggers the interrupt (and wakes from deep sleep) and not a
	// full device reset.  This is a timing critical section of code that must
	// happen in 4 cycles.
	WDTCSR |= (1<<WDCE) | (1<<WDE);  // Set WDCE and WDE to enable changes. TODO warum WDE?
	WDTCSR = wdps;                   // Set the prescaler bit values.
	WDTCSR |= (1<<WDIE);             // Enable only watchdog interrupts.
	// Critical section finished, re-enable interrupts.
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

void wdtSleep() {
	enableWdtInterrupt();
	sleepMode();
}

int
main()
{
	enableInterrupts();

	InterruptPin::enableExternalInterrupt();
	InterruptPin::setInputTrigger(InterruptPin::InputTrigger::LowLevel);

	ButtonPin::setInput(InterruptPin::InputType::PullUp);
	ButtonPin::enableExternalInterrupt();
	ButtonPin::setInputTrigger(InterruptPin::InputTrigger::BothEdges);

	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	// Enable SPI
	SpiMaster::connect<D13::Sck, D11::Mosi, D12::Miso>();
	SpiMaster::initialize<modm::platform::SystemClock, 500000, modm::Tolerance::TenPercent>();

	Switch::initializeRadio(channel);

	Switch::setAddress(network, device);

	Switch::startTX();


	while (1)
	{
		if (button) {

			Switch::send(ding::LichtMessage(0x00, 0x02, message));

			button = false;
	// 		downtime = 0;
		}

		modm::delayMilliseconds(100);
		sleepMode();

// 	if (downtime == 0)
// 		enterSleep();

	}
}

MODM_ISR(INT0)
{
	handleInterrupt();
}

MODM_ISR(INT1)
{
	handleButton();
}

MODM_ISR(WDT)
{

}
