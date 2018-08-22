// coding: utf-8
/* Copyright (c) 2018, Hannes Ellinger
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ATMEGA_GPIO_PWM_HPP
#define MODM_ATMEGA_GPIO_PWM_HPP

#include <modm/platform.hpp>

/**
 * @ingroup 	platform
 * @defgroup	atmega328p
 */

/**
 * @ingroup 	atmega328p
 * @defgroup	atmega328p_gpio GPIO
 */


namespace modm
{

namespace platform
{

/// Output class for Pin B1
/// @ingroup	atmega328p_gpio
struct GpioPwmB1 : public GpioOutputB1
{
protected:
	modm_always_inline static void enablePwm() {
		// Enable counter, reset pin on match
		TCCR1A |=   (1 << 7) | (1 << 0);
		TCCR1A &= ~((1 << 6) | (1 << 1));

		// Fast PWM, prescaler 1
		TCCR1B = 0x09;
	}

	modm_always_inline static void disablePwm() {
		// OC1A disconnected
		TCCR1A &= ~((1 << 7) | (1 << 6));
	}
public:
	static constexpr Port port = Port::B;		///< the port of this GPIO
	static constexpr uint8_t pin = 1;			///< the pin  of this GPIO
	static constexpr uint8_t mask = (1 << pin);	///< the mask of this GPIO

	modm_always_inline static void setOutput(uint8_t dutycycle) {
		setOutput();
		set(dutycycle);
	}
	modm_always_inline static void setOutput() {
		disablePwm();
		DDRB |= mask;
	}
	modm_always_inline static void set() {
		disablePwm();
		PORTB |= mask;
	}
	modm_always_inline static void set(uint8_t dutycycle) {
		if (dutycycle == 255) {
			set();
		} else if (dutycycle == 0) {
			reset();
		} else {
			enablePwm();
			OCR1A = dutycycle - 1;
		}
	}
	modm_always_inline static void reset() {
		disablePwm();
		PORTB &= ~mask;
	}
	modm_always_inline static void toggle() {
		disablePwm();
		PINB = mask;
	}
	modm_always_inline static bool isSet() {
		return (PORTB & mask);
	}
	modm_always_inline static void
	disconnect() {
		disablePwm();
		DDRB &= ~mask;   // manual call to setInput()
		PORTB &= ~mask;
	}
};

/// Output class for Pin B2
/// @ingroup	atmega328p_gpio
struct GpioPwmB2 : public GpioOutputB2
{
protected:
	modm_always_inline static void enablePwm() {
		// Enable counter, reset pin on match
		TCCR1A |=   (1 << 5) | (1 << 0);
		TCCR1A &= ~((1 << 4) | (1 << 1));

		// Fast PWM, prescaler 1
		TCCR1B = 0x09;
	}

	modm_always_inline static void disablePwm() {
		// OC1A disconnected
		TCCR1A &= ~((1 << 5) | (1 << 4));
	}
public:
	static constexpr Port port = Port::B;		///< the port of this GPIO
	static constexpr uint8_t pin = 2;			///< the pin  of this GPIO
	static constexpr uint8_t mask = (1 << pin);	///< the mask of this GPIO

	modm_always_inline static void setOutput(uint8_t dutycycle) {
		setOutput();
		set(dutycycle);
	}
	modm_always_inline static void setOutput() {
		disablePwm();
		DDRB |= mask;
	}
	modm_always_inline static void set() {
		disablePwm();
		PORTB |= mask;
	}
	modm_always_inline static void set(uint8_t dutycycle) {
		if (dutycycle == 255) {
			set();
		} else if (dutycycle == 0) {
			reset();
		} else {
			enablePwm();
			OCR1B = dutycycle - 1;
		}
	}
	modm_always_inline static void reset() {
		disablePwm();
		PORTB &= ~mask;
	}
	modm_always_inline static void toggle() {
		disablePwm();
		PINB = mask;
	}
	modm_always_inline static bool isSet() {
		return (PORTB & mask);
	}
	modm_always_inline static void
	disconnect() {
		disablePwm();
		DDRB &= ~mask;   // manual call to setInput()
		PORTB &= ~mask;
	}
};

/// Output class for Pin B3
/// @ingroup	atmega328p_gpio
struct GpioPwmB3 : public GpioOutputB3
{
protected:
	modm_always_inline static void enablePwm() {
		// Enable counter, reset pin on match
		TCCR2A |=  (1 << 7) | (1 << 1) | (1 << 0);
		TCCR2A &= ~(1 << 6);

		// Fast PWM, prescaler 1
		TCCR2B = 0x01;
	}

	modm_always_inline static void disablePwm() {
		// OC1A disconnected
		TCCR2A &= ~((1 << 7) | (1 << 6));
	}
public:
	static constexpr Port port = Port::B;		///< the port of this GPIO
	static constexpr uint8_t pin = 3;			///< the pin  of this GPIO
	static constexpr uint8_t mask = (1 << pin);	///< the mask of this GPIO

	modm_always_inline static void setOutput(uint8_t dutycycle) {
		setOutput();
		set(dutycycle);
	}
	modm_always_inline static void setOutput() {
		disablePwm();
		DDRB |= mask;
	}
	modm_always_inline static void set() {
		disablePwm();
		PORTB |= mask;
	}
	modm_always_inline static void set(uint8_t dutycycle) {
		if (dutycycle == 255) {
			set();
		} else if (dutycycle == 0) {
			reset();
		} else {
			enablePwm();
			OCR2A = dutycycle - 1;
		}
	}
	modm_always_inline static void reset() {
		disablePwm();
		PORTB &= ~mask;
	}
	modm_always_inline static void toggle() {
		disablePwm();
		PINB = mask;
	}
	modm_always_inline static bool isSet() {
		return (PORTB & mask);
	}
	modm_always_inline static void
	disconnect() {
		disablePwm();
		DDRB &= ~mask;   // manual call to setInput()
		PORTB &= ~mask;
	}
};

/// Output class for Pin D3
/// @ingroup	atmega328p_gpio
struct GpioPwmD3 : public GpioOutputD3
{
protected:
	modm_always_inline static void enablePwm() {
		// Enable counter, reset pin on match
		TCCR2A |=  (1 << 5) | (1 << 1) | (1 << 0);
		TCCR2A &= ~(1 << 4);

		// Fast PWM, prescaler 1
		TCCR2B = 0x01;
	}

	modm_always_inline static void disablePwm() {
		// OC1A disconnected
		TCCR2A &= ~((1 << 5) | (1 << 4));
	}
public:
	// static constexpr Port port = Port::D;		///< the port of this GPIO
	// static constexpr uint8_t pin = 3;			///< the pin  of this GPIO
	// static constexpr uint8_t mask = (1 << pin);	///< the mask of this GPIO

	modm_always_inline static void setOutput(uint8_t dutycycle) {
		setOutput();
		set(dutycycle);
	}
	modm_always_inline static void setOutput() {
		disablePwm();
		DDRD |= mask;
	}
	modm_always_inline static void set() {
		disablePwm();
		PORTD |= mask;
	}
	modm_always_inline static void set(uint8_t dutycycle) {
		if (dutycycle == 255) {
			set();
		} else if (dutycycle == 0) {
			reset();
		} else {
			enablePwm();
			OCR2B = dutycycle - 1;
		}
	}
	modm_always_inline static void reset() {
		disablePwm();
		PORTD &= ~mask;
	}
	modm_always_inline static void toggle() {
		disablePwm();
		PIND = mask;
	}
	modm_always_inline static bool isSet() {
		return (PORTD & mask);
	}
	modm_always_inline static void
	disconnect() {
		disablePwm();
		DDRD &= ~mask;   // manual call to setInput()
		PORTD &= ~mask;
	}
};

/// Output class for Pin D6
/// @ingroup	atmega328p_gpio
struct GpioPwmD6 : public GpioOutputD6
{
protected:
	modm_always_inline static void enablePwm() {
		// Enable counter, reset pin on match
		TCCR0A |=  (1 << 7) | (1 << 1) | (1 << 0);
		TCCR0A &= ~(1 << 6);

		// Fast PWM, prescaler 1
		TCCR0B = 0x01;
	}

	modm_always_inline static void disablePwm() {
		// OC1A disconnected
		TCCR0A &= ~((1 << 7) | (1 << 6));
	}
public:
	// static constexpr Port port = Port::D;		///< the port of this GPIO
	// static constexpr uint8_t pin = 6;			///< the pin  of this GPIO
	// static constexpr uint8_t mask = (1 << pin);	///< the mask of this GPIO

	modm_always_inline static void setOutput(uint8_t dutycycle) {
		setOutput();
		set(dutycycle);
	}
	modm_always_inline static void setOutput() {
		disablePwm();
		DDRD |= mask;
	}
	modm_always_inline static void set() {
		disablePwm();
		PORTD |= mask;
	}
	modm_always_inline static void set(uint8_t dutycycle) {
		if (dutycycle == 255) {
			set();
		} else if (dutycycle == 0) {
			reset();
		} else {
			enablePwm();
			OCR0A = dutycycle - 1;
		}
	}
	modm_always_inline static void reset() {
		disablePwm();
		PORTD &= ~mask;
	}
	modm_always_inline static void toggle() {
		disablePwm();
		PIND = mask;
	}
	modm_always_inline static bool isSet() {
		return (PORTD & mask);
	}
	modm_always_inline static void
	disconnect() {
		disablePwm();
		DDRD &= ~mask;   // manual call to setInput()
		PORTD &= ~mask;
	}
};

/// Output class for Pin D5
/// @ingroup	atmega328p_gpio
struct GpioPwmD5 : public GpioOutputD5
{
protected:
	modm_always_inline static void enablePwm() {
		// Enable counter, reset pin on match
		TCCR0A |=  (1 << 5) | (1 << 1) | (1 << 0);
		TCCR0A &= ~(1 << 4);

		// Fast PWM, prescaler 1
		TCCR0B = 0x01;
	}

	modm_always_inline static void disablePwm() {
		// OC1A disconnected
		TCCR0A &= ~((1 << 5) | (1 << 4));
	}
public:
	// static constexpr Port port = Port::D;		///< the port of this GPIO
	// static constexpr uint8_t pin = 5;			///< the pin  of this GPIO
	// static constexpr uint8_t mask = (1 << pin);	///< the mask of this GPIO

	modm_always_inline static void setOutput(uint8_t dutycycle) {
		setOutput();
		set(dutycycle);
	}
	modm_always_inline static void setOutput() {
		disablePwm();
		DDRD |= mask;
	}
	modm_always_inline static void set() {
		disablePwm();
		PORTD |= mask;
	}
	modm_always_inline static void set(uint8_t dutycycle) {
		if (dutycycle == 255) {
			set();
		} else if (dutycycle == 0) {
			reset();
		} else {
			enablePwm();
			OCR0B = dutycycle - 1;
		}
	}
	modm_always_inline static void reset() {
		disablePwm();
		PORTD &= ~mask;
	}
	modm_always_inline static void toggle() {
		disablePwm();
		PIND = mask;
	}
	modm_always_inline static bool isSet() {
		return (PORTB & mask);
	}
	modm_always_inline static void
	disconnect() {
		disablePwm();
		DDRD &= ~mask;   // manual call to setInput()
		PORTD &= ~mask;
	}
};
/// @endcond

}	// namespace platform

}	// namespace modm

#endif
