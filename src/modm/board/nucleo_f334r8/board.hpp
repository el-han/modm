/*
 * Copyright (c) 2020, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_F334R8_HPP
#define MODM_STM32_NUCLEO_F334R8_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_f334r8
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_f334r8
/// @{
using namespace modm::literals;

/// STM32F334R8 running at 64MHz generated from the internal 8MHz clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 64_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc1   = Apb2;
	static constexpr uint32_t Adc2   = Apb2;

	static constexpr uint32_t Can    = Apb1;

	static constexpr uint32_t Spi1   = Apb2;

	// USART1 is on APB2 bus, but default clock is PCLK1
	static constexpr uint32_t Usart1 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;

	// I2C1 clock source is HSI per default
	static constexpr uint32_t I2c1   = 8_MHz;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock();	// 8MHz
		// 8MHz / 2 * 16 = 64MHz
		const Rcc::PllFactors pllFactors{
			.pllMul = 16,
			.pllPrediv = 2, // fixed (/2) for Hsi
		};
		Rcc::enablePll(Rcc::PllSource::HsiDiv2, pllFactors);
		// set flash latency for 64MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 36MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Footprint
#include "nucleo64_arduino.hpp"

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_f334r8
/// @{
using Rx = GpioInputA15;
using Tx = GpioOutputA2;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_f334r8
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();
}
/// @}

}

#endif	// MODM_STM32_NUCLEO_F334R8_HPP
