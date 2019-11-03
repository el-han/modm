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
#include <modm/communication/ding/humi.hpp>
#include <modm/board.hpp>
#include <modm/architecture/interface/interrupt.hpp>

#include <avr/sleep.h>
#include <avr/wdt.h>

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DISABLED

using namespace modm::platform;

constexpr uint16_t network = 0xA1A2;
constexpr uint8_t  device  = 0xC4;
constexpr uint8_t  channel = 96;


// Raduino
typedef D7 Ce;
typedef D8 Csn;
typedef D3 InterruptPin;

typedef D13 Sck;
typedef D11 Mosi;
typedef D12 Miso;

typedef D2 ButtonInterruptPin;

typedef A2 S1;
typedef A1 S2;
typedef D9 S3;
typedef D6 S4;
typedef D5 S5;
typedef D4 S6;

typedef BitBangSpiMaster<Sck, Mosi, Miso> SpiRadio;
typedef modm::Nrf24Phy<SpiRadio, Csn, Ce> Radio;

typedef ding::Humi<Radio, I2cMaster> Humi;
typedef ding::Ding<Radio> Schalter;

volatile bool sent, lost = false;
volatile bool wdt = true;

void
handleInterrupt()
{
	uint8_t status = Radio::readStatus();
	MODM_LOG_DEBUG << "Interrutp!\r\n";

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
		Schalter::send(ding::Message(0x00, device, 1));
	if (!S2::read())
		Schalter::send(ding::Message(0x00, device, 2));
	if (!S3::read())
		Schalter::send(ding::Message(0x00, device, 3));
	if (!S4::read())
		Schalter::send(ding::Message(0x00, device, 4));
	if (!S5::read())
		Schalter::send(ding::Message(0x00, device, 5));
	if (!S6::read())
		Schalter::send(ding::Message(0x00, device, 6));

	modm::delayMilliseconds(100);
	enableInterrupts();
}

void enableWdtInterrupt()
{
	// Pick the closest appropriate watchdog timer value.
	int sleepWDTO = WDTO_8S;

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
	systemClock::enable();

	Uart0::connect<D1::Txd, D0::Rxd>();
	Uart0::initialize<Board::SystemClock, 9600>();
	enableInterrupts();

	MODM_LOG_DEBUG << "Humi Test\r\n";

	Adc::initialize<SystemClock, 115_kHz>();
	Adc::setReference(Adc::Reference::InternalVcc);

	I2cMaster::connect<A4::Sda, A5::Scl>();
	I2cMaster::initialize<Board::SystemClock, 100000>();

	modm::bme280::Data bmeData;
	modm::Bme280<I2cMaster> bme(bmeData, 0x76);

	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	// Enable SPI
	SpiRadio::connect<D13::BitBang, D11::BitBang, D12::BitBang>();
	SpiRadio::initialize<Board::SystemClock, 500000, 10_pct>();

	Humi::initializeRadio(channel);

	MODM_LOG_DEBUG << "go\r\n";

	InterruptPin::setInput(InterruptPin::InputType::PullUp);
	InterruptPin::enableExternalInterrupt();
	InterruptPin::setInputTrigger(InterruptPin::InputTrigger::LowLevel);

	S1::setInput(Gpio::InputType::PullUp);
	S2::setInput(Gpio::InputType::PullUp);
	S3::setInput(Gpio::InputType::PullUp);
	S4::setInput(Gpio::InputType::PullUp);
	S5::setInput(Gpio::InputType::PullUp);
	S6::setInput(Gpio::InputType::PullUp);

	ButtonInterruptPin::setInput(Gpio::InputType::PullUp);
	ButtonInterruptPin::enableExternalInterrupt();
	ButtonInterruptPin::setInputTrigger(Gpio::InputTrigger::LowLevel);

	Humi::setAddress(network, device);

	Humi::startTX();

	Humi::begin(bme);

	while (true)
	{
		if (wdt == true) {

			wdt = false;

			Humi::measure(bme);

			bool success = true; // only one transmission

			do {
				MODM_LOG_DEBUG << "send... ";

				Humi::send();

				while (!sent && !lost)
					;

				if (sent)
					success = true;

				sent = false;
				lost = false;
			} while (!success);

			uint16_t adcvalue = Adc::readChannel(14);
			uint16_t voltage = adcvalue != 0 ? (1100L * 1023) / adcvalue : 0;

			success = true; // only one transmission

			do {
				MODM_LOG_DEBUG << "send... ";

				Schalter::send(ding::Message(device, voltage));

				while (!sent && !lost)
					;

				if (sent)
					success = true;

				sent = false;
				lost = false;
			} while (!success);
			MODM_LOG_DEBUG << "sent\r\n";

			modm::delayMilliseconds(100);
			enableWdtInterrupt();
		}

		sleepMode();
	}
}

MODM_ISR(INT1)
{
	handleInterrupt();
}

MODM_ISR(INT0)
{
	handleButton();
}

MODM_ISR(WDT)
{
	wdt = true;
}
