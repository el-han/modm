/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstdint>
#include <modm/board.hpp>
#include "modm/platform/sai/sai_base.hpp"
#include "stm32f767xx.h"
// #include <modm/sai_master.hpp>

using namespace Board;

using Sai = SaiMaster1;

using Mclk = GpioOutputE2;
using Fs = GpioOutputE4;
using Sck = GpioOutputE5;
using Sd = GpioOutputE6;

int
main()
{
	Board::initialize();

	Sai::connect<Mclk::Mclka, Fs::Fsa, Sck::Scka, Sd::Sda>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	Rcc::enable<Peripheral::Sai1>();

	SAI1->GCR = 0x00000000;
	// SAI1_Block_A->CR1 = 0x00200280;
	SAI1_Block_A->CR1 = static_cast<uint32_t>(SaiBase::DataSize::DataSize16Bit) |
	                    static_cast<uint32_t>(SaiBase::MasterClockDivider::Div4) |
	                    static_cast<uint32_t>(SaiBase::ConfigurationRegister1::CKSTR) |
	                    static_cast<uint32_t>(SaiBase::Mode::MasterTransmitter);
	SAI1_Block_A->CR2 = 0x00000000;
	// SAI1_Block_A->FRCR = 0x0006001f;
	SAI1_Block_A->FRCR = static_cast<uint32_t>(SaiBase::FrameLength_t(16*8-1).value) |
	                     static_cast<uint32_t>(SaiBase::FrameConfigurationRegister::FSPOL) |
	                     static_cast<uint32_t>(SaiBase::FrameConfigurationRegister::FSOFF);
	// SAI1_Block_A->SLOTR = 0x00030200;
	SAI1_Block_A->SLOTR = static_cast<uint32_t>(SaiBase::SlotNumber_t(2).value) |
	                      static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN0) |
	                      static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN1);

	// Enable FIFO request interrupt
	Sai::HalA::enableInterrupt(SaiBase::Interrupt::FIFORequest);
	Sai::HalA::enableInterruptVector(true, 10);

	// Set SAIEN
	Sai::HalA::enableTransfer();

	SAI1_Block_A->DR = 0x00bb00aa;
	SAI1_Block_A->DR = 0x001100ff;

	MODM_LOG_INFO << "CR1: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->CR1) << modm::endl;
	MODM_LOG_INFO << "CR2: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->CR2) << modm::endl;
	MODM_LOG_INFO << "FRCR: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->FRCR) << modm::endl;
	MODM_LOG_INFO << "SLOTR: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->SLOTR) << modm::endl;
	MODM_LOG_INFO << "SR: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->SR) << modm::endl;

	while (true)
	{
		LedGreen::toggle();

		modm::delay(1s);

		MODM_LOG_INFO << "CR1: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->CR1) << modm::endl;
		MODM_LOG_INFO << "CR2: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->CR2) << modm::endl;
		MODM_LOG_INFO << "FRCR: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->FRCR) << modm::endl;
		MODM_LOG_INFO << "SLOTR: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->SLOTR) << modm::endl;
		MODM_LOG_INFO << "SR: 0x" << modm::hex << (uint32_t)(SAI1_Block_A->SR) << modm::endl;
	}

	return 0;
}

MODM_ISR(SAI1)
{
	if ( (SAI1_Block_A->SR & (1 << 3)) != 0) {
		SAI1_Block_A->DR = 0x00bb00aa;
		SAI1_Block_A->DR = 0x001100ff;
	}
}
