// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__COMP5_HPP
#define XPCC_STM32__COMP5_HPP

#if defined(STM32F3XX)
namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Comparator Class for STM32F3 series
		 * 
		 * The class provides an interface to the comparators 1-7 availably on
		 * ST's STM32F3 microcontroller series.
		 * 
		 * @internal
		 * @ingroup		stm32
		 */
		class Comp5
		{
		public:
			enum Mode
			{
				ULTRA_LOW_POWER = 0b00,
				LOW_POWER = COMP5_CSR_COMP5MODE_0,		//0b01
				MEDIUM_POWER = COMP5_CSR_COMP5MODE_1,		//0b10
				HIGH_SPEED = 	COMP5_CSR_COMP5MODE_0 |
								COMP5_CSR_COMP5MODE_1		//0b11
			};

			enum InvertingInput
			{
				VREF_1_4 = 0b000,	//0b000 1/4 of Vrefint
				VREF_1_2 =			// 1/2 of Vrefint
							COMP5_CSR_COMP5INSEL_0,	//0b001
				VREF_3_4 =			// 3/4 of Vrefint
							COMP5_CSR_COMP5INSEL_1,	//0b010
				VREF = 				// Vrefint
							COMP5_CSR_COMP5INSEL_1 |
							COMP5_CSR_COMP5INSEL_0,	//0b011
				// Common Port Aliases
				PA4 =	COMP5_CSR_COMP5INSEL_2,		//0b100
				DAC1 =	COMP5_CSR_COMP5INSEL_2,		//0b100
				PA5 = 	COMP5_CSR_COMP5INSEL_2 |
						COMP5_CSR_COMP5INSEL_0,		//0b101
				DAC2 = 	COMP5_CSR_COMP5INSEL_2 |
						COMP5_CSR_COMP5INSEL_0,		//0b101
			// PA4 or DAC1 output if enabled
				COMP5_INM4 = 	COMP5_CSR_COMP5INSEL_2,	//0b100
				// PA5 or DAC2 output if enabled
				COMP5_INM5 = 	COMP5_CSR_COMP5INSEL_2 |
								COMP5_CSR_COMP5INSEL_0,	//0b101
				// PD13
				COMP5_INM6 = 	COMP5_CSR_COMP5INSEL_2 |
								COMP5_CSR_COMP5INSEL_1,	//0b110
				// PB10
				COMP5_INM7 = 	COMP5_CSR_COMP5INSEL_2 |
								COMP5_CSR_COMP5INSEL_1 |
								COMP5_CSR_COMP5INSEL_0,	//0b111
				// Port Alias
				PD13 = 			COMP5_CSR_COMP5INSEL_2 |
								COMP5_CSR_COMP5INSEL_1,	//0b110
				PB10 = 			COMP5_CSR_COMP5INSEL_2 |
								COMP5_CSR_COMP5INSEL_1 |
								COMP5_CSR_COMP5INSEL_0	//0b111
			};

enum NonInvertingInput
			{
			PD12 = 0b0,
				PB13 = COMP5_CSR_COMP5NONINSEL	//0b1
			};
enum Output
			{
				NOT_CONNECTED = 0b0000,
				TIMER_1_BREAK_INPUT =
								COMP5_CSR_COMP5OUTSEL_0,	//0b0001
				TIMER_1_BREAK_INPUT_2 =
								COMP5_CSR_COMP5OUTSEL_1,	//0b0010
				TIMER_8_BREAK_INPUT =
								COMP5_CSR_COMP5OUTSEL_1 |
								COMP5_CSR_COMP5OUTSEL_0,	//0b0011
				TIMER_8_BREAK_INPUT_2 =
								COMP5_CSR_COMP5OUTSEL_2,	//0b0100
				TIMER_8_OR_1_BREAK_INPUTS_2 =
								COMP5_CSR_COMP5OUTSEL_2 |
								COMP5_CSR_COMP5OUTSEL_0,	//0b0101

			TIMER_2_INPUT_CAPTURE_1 =
								COMP5_CSR_COMP5OUTSEL_2 |
								COMP5_CSR_COMP5OUTSEL_1,	//0b0110
				TIMER_8_OCREF_CLEAR_INPUT =
								COMP5_CSR_COMP5OUTSEL_2 |
								COMP5_CSR_COMP5OUTSEL_1 |
								COMP5_CSR_COMP5OUTSEL_0,	//0b0111
				TIMER_17_INPUT_CAPTURE_1 =
								COMP5_CSR_COMP5OUTSEL_3,	//0b1000,
				TIMER_4_INPUT_CAPTURE_3 =
								COMP5_CSR_COMP5OUTSEL_3 |
								COMP5_CSR_COMP5OUTSEL_0,	//0b1001
				TIMER_16_BREAK_INPUT =
								COMP5_CSR_COMP5OUTSEL_3 |
								COMP5_CSR_COMP5OUTSEL_1,	//0b1010
				TIMER_3_OCREF_CLEAR_INPUT =
								COMP5_CSR_COMP5OUTSEL_3 |
								COMP5_CSR_COMP5OUTSEL_1 |
								COMP5_CSR_COMP5OUTSEL_0	//0b1011
			};

			enum Polarity
			{
				NOT_INVERTED = 0b0,
				INVERTED = COMP5_CSR_COMP5POL		//0b1
			};

			enum Hysteresis
			{
				NO_HYSTERESIS = 0b00,
				LOW_HYSTERESIS = COMP5_CSR_COMP5HYST_0,	//0b01
				MEDIUM_HYSTERESIS = COMP5_CSR_COMP5HYST_1,//0b10
				HIGH_HYSTERESIS = COMP5_CSR_COMP5HYST_1 |
									COMP5_CSR_COMP5HYST_0	//0b11
			};

			enum BlankingSource
			{
				NO_BLANKING = 0b000,
			TIMER_8_OC5 = COMP5_CSR_COMP5BLANKING_1,	// 0b010
				TIMER_3_OC3 = COMP5_CSR_COMP5BLANKING_1 |
								COMP5_CSR_COMP5BLANKING_0	// 0b011
			};

		public:

			/**
			 * Initialize and enable the comparator.
			 *
			 * Enables the comperator and sets important values.
			 * 
			 * Do NOT set lock = true if you want to be able to set other values
			 * later.
			 */
			static inline void
			initialize(
						InvertingInput n_in,
					NonInvertingInput p_in,
					Output out = Output::NOT_CONNECTED,
						Hysteresis hyst = Hysteresis::NO_HYSTERESIS,
						Mode mode = Mode::HIGH_SPEED,
						Polarity pol = Polarity::NOT_INVERTED,
						bool lock_comp = false)
			{
				setInvertingInput(n_in);
			setNonInvertingInput(p_in);
			setOutputSelection(out);
				setHysteresis(hyst);
				setMode(mode);
				setPolarity(pol);
				setEnabled(true);	// enable comparator
				if(lock_comp) lock();
			}
			


			/**
			 * \brief	Enable/Disable the comparator.
			 */
			static inline void
			setEnabled(bool enabled)
			{
				if(enabled)
					COMP5->CSR |= COMP5_CSR_COMP5EN;
				else
					COMP5->CSR &= ~COMP5_CSR_COMP5EN;
			}

			/**
			 * \brief	Returns whether the comparator is enabled.
			 */
			static inline bool
			isEnabled()
			{
				return COMP5->CSR & COMP5_CSR_COMP5EN;
			}

		/**
			 * \brief	Sets the mode that determins speed/power consumption.
			 * 
			 * This setting is also called "output mode".
			 */
			static inline void
			setMode(Mode mode)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5MODE)
					| static_cast<uint32_t>(mode);
			}

			/**
			 * \brief	Sets the mode that determins speed/power consumption.
			 * 
			 * This setting is also called "output mode".
			 */
			static inline Mode
			getMode()
			{
				return static_cast<Mode>
					(COMP5->CSR & COMP5_CSR_COMP5MODE);
			}

			/**
			 * \brief	Selects what the inverting input is connected to.
			 */
			static inline void
			setInvertingInput(InvertingInput input)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5INSEL)
					| static_cast<uint32_t>(input);
			}

			/**.
			 * \brief	Returns what is connected to the inverting input.
			 */
			static inline InvertingInput
			getInvertingInput()
			{
				return static_cast<InvertingInput>
					(COMP5->CSR & COMP5_CSR_COMP5INSEL);
			}

		/**
			 * \brief	Selects what the non-inverting input is connected to.
			 */
			static inline void
			setNonInvertingInput(NonInvertingInput input)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5NONINSEL)
					| static_cast<uint32_t>(input);
			}

			/**.
			 * \brief	Returns what is connected to the non-inverting input.
			 */
			static inline NonInvertingInput
			getNonInvertingInput()
			{
				return static_cast<NonInvertingInput>
					(COMP5->CSR & COMP5_CSR_COMP5NONINSEL);
			}
		/**
			 * \brief	Selects what the output is connected to.
			 */
			static inline void
			setOutputSelection(Output output)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5OUTSEL)
					| static_cast<uint32_t>(output);
			}

			/**.
			 * \brief	Returns what is connected to the output.
			 */
			static inline Output
			getOutputSelection()
			{
				return static_cast<Output>
					(COMP5->CSR & COMP5_CSR_COMP5OUTSEL);
			}

			/**
			 * \brief	Selects output polarity.
			 */
			static inline void
			setPolarity(Polarity pol)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5POL)
					| static_cast<uint32_t>(pol);
			}

			/**.
			 * \brief	Returns output polarity.
			 */
			static inline Polarity
			getPolarity()
			{
				return static_cast<Polarity>
					(COMP5->CSR & COMP5_CSR_COMP5POL);
			}

			/**
			 * \brief	Selects the hysteresis.
			 */
			static inline void
			setHysteresis(Hysteresis hyst)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5HYST)
					| static_cast<uint32_t>(hyst);
			}

			/**.
			 * \brief	Returns the hysteresis.
			 */
			static inline Hysteresis
			getHysteresis()
			{
				return static_cast<Hysteresis>
					(COMP5->CSR & COMP5_CSR_COMP5HYST);
			}

			/**
			 * \brief	Selects the blanking source.
			 */
			static inline void
			setBlankingSource(BlankingSource blanking)
			{
				COMP5->CSR |=
					(COMP5->CSR & ~COMP5_CSR_COMP5BLANKING)
					| static_cast<uint32_t>(blanking);
			}

			/**.
			 * \brief	Returns the blanking source.
			 */
			static inline BlankingSource
			getBlankingSource()
			{
				return static_cast<BlankingSource>
					(COMP5->CSR & COMP5_CSR_COMP5BLANKING);
			}

			/**
			 * \brief	Returns the current Comparator output.
			 */
			static inline bool
			getOutput()
			{
				return COMP5->CSR & COMP5_CSR_COMP5OUT;
			}

			/**
			 * \brief	Locks the comparator. 
			 *
			 * Comparator can only be unlocked by a system reset.
			 */
			static inline void
			lock(void)
			{
				COMP5->CSR |= COMP5_CSR_COMP5LOCK;
			}

			/**
			 * \brief	Returns true if the comparator is locked.
			 */
			static inline bool
			isLocked(void)
			{
				return COMP5->CSR & COMP5_CSR_COMP5LOCK;
			}
		};
	}
}
#else	// defined(STM32F3XX)
	#  error 	"STM32F3XX not defined. Are you sure the comparator hw " \
				"feature is enabled on you platform?"
#endif

#endif	//  XPCC_STM32__COMP5_HPP