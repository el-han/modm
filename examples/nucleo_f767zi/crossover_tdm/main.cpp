/*
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <so_butterworth_hpf.h>
#include <so_butterworth_lpf.h>

#include <cstdint>
#include <modm/board.hpp>
#include <modm/io.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <modm/processing.hpp>

using namespace Board;

SO_BUTTERWORTH_LPF woofer_lpf_1_l;
SO_BUTTERWORTH_LPF woofer_lpf_2_l;
SO_BUTTERWORTH_LPF mid_lpf_1_l;
SO_BUTTERWORTH_LPF mid_lpf_2_l;
SO_BUTTERWORTH_HPF mid_hpf_1_l;
SO_BUTTERWORTH_HPF mid_hpf_2_l;
SO_BUTTERWORTH_HPF tweeter_hpf_1_l;
SO_BUTTERWORTH_HPF tweeter_hpf_2_l;

SO_BUTTERWORTH_LPF woofer_lpf_1_r;
SO_BUTTERWORTH_LPF woofer_lpf_2_r;
SO_BUTTERWORTH_LPF mid_lpf_1_r;
SO_BUTTERWORTH_LPF mid_lpf_2_r;
SO_BUTTERWORTH_HPF mid_hpf_1_r;
SO_BUTTERWORTH_HPF mid_hpf_2_r;
SO_BUTTERWORTH_HPF tweeter_hpf_1_r;
SO_BUTTERWORTH_HPF tweeter_hpf_2_r;

volatile double woofer_tmp_l = 0.0;
volatile double woofer_sample_l = 0.0;
volatile double mid_tmp0_l = 0.0;
volatile double mid_tmp1_l = 0.0;
volatile double mid_tmp2_l = 0.0;
volatile double mid_sample_l = 0.0;
volatile double tweeter_tmp_l = 0.0;
volatile double tweeter_sample_l = 0.0;

volatile double woofer_tmp_r = 0.0;
volatile double woofer_sample_r = 0.0;
volatile double mid_tmp0_r = 0.0;
volatile double mid_tmp1_r = 0.0;
volatile double mid_tmp2_r = 0.0;
volatile double mid_sample_r = 0.0;
volatile double tweeter_tmp_r = 0.0;
volatile double tweeter_sample_r = 0.0;


using SaiA = SaiMaster1BlockA;  // |  |      |
using SaiB = SaiMaster1BlockB;  // +--+------+
                                // |  | Gnd  |
                                // +--+------+
using Mclka = GpioOutputE2;     // |  | Mclk |  -+
                                // +--+------+   |
using Fsa = GpioOutputE4;       // |  |  Fs  |   |
                                // +--+------+ SAI_A
using Scka = GpioOutputE5;      // |  | Sclk |   |
                                // +--+------+   |
using Sda = GpioOutputE6;       // |  | Dout |  -+
                                // +--+------+
using Sdb = GpioInputE3;        // |  | Din  |  -+
                                // +--+------+   |
using Sckb = GpioInputF8;       // |  | Sclk |   |
                                // +--+------+ SAI_B
using Mclkb = GpioInputF7;      // |  | Mclk |   |
                                // +--+------+   |
using Fsb = GpioInputF9;        // |  | Fclk |  -+
                                // +--+------+
using Debug = GpioOutputG1;     // |  | Dbg  |
                                // +--+------+
                                //
                                //    CN9

bool running = false;

modm::PeriodicTimer tmr{2.5s};

int main()
{
    Board::initialize();

    woofer_lpf_1_l.calculate_coeffs(150, 48000);
    woofer_lpf_2_l.calculate_coeffs(150, 48000);

    mid_hpf_1_l.calculate_coeffs(150, 48000);
    mid_hpf_2_l.calculate_coeffs(150, 48000);

    mid_lpf_1_l.calculate_coeffs(1130, 48000);
    mid_lpf_2_l.calculate_coeffs(1130, 48000);

    tweeter_hpf_1_l.calculate_coeffs(1130, 48000);
    tweeter_hpf_2_l.calculate_coeffs(1130, 48000);

    woofer_lpf_1_r.calculate_coeffs(150, 48000);
    woofer_lpf_2_r.calculate_coeffs(150, 48000);

    mid_hpf_1_r.calculate_coeffs(150, 48000);
    mid_hpf_2_r.calculate_coeffs(150, 48000);

    mid_lpf_1_r.calculate_coeffs(1130, 48000);
    mid_lpf_2_r.calculate_coeffs(1130, 48000);

    tweeter_hpf_1_r.calculate_coeffs(1130, 48000);
    tweeter_hpf_2_r.calculate_coeffs(1130, 48000);

    Debug::setOutput(modm::Gpio::High);

    SaiA::connectTransmitter<Mclka::Mclka, Fsa::Fsa, Scka::Scka, Sda::Sda>();
    SaiB::connectReceiver<Mclkb::Mclkb, Fsb::Fsb, Sckb::Sckb, Sdb::Sdb>();

    // Use the logging streams to print some messages.
    // Change MODM_LOG_LEVEL above to enable or disable these messages
    MODM_LOG_DEBUG   << "debug"   << modm::endl;
    MODM_LOG_INFO    << "info"    << modm::endl;
    MODM_LOG_WARNING << "warning" << modm::endl;
    MODM_LOG_ERROR   << "error"   << modm::endl;

    Rcc::enable<Peripheral::Sai1>();

    SAI1->GCR = 0x00000020;
    MODM_LOG_INFO << "GCR: 0x" << modm::hex << (uint32_t)SAI1->GCR << modm::endl;
    // SAI1_Block_A->CR1 = 0x00200280;
    SAI1_Block_A->CR1 = static_cast<uint32_t>(SaiBase::DataSize::DataSize16Bit) |
                        static_cast<uint32_t>(SaiBase::SyncEn::Internal) |
                        static_cast<uint32_t>(SaiBase::ConfigurationRegister1::CKSTR) |
                        static_cast<uint32_t>(SaiBase::Mode::SlaveTransmitter);
    MODM_LOG_INFO << "A CR1: 0x" << modm::hex << (uint32_t)SAI1_Block_A->CR1 << modm::endl;
    SAI1_Block_A->CR2 = 0x00000000;
    MODM_LOG_INFO << "A CR2: 0x" << modm::hex << (uint32_t)SAI1_Block_A->CR2 << modm::endl;
    // SAI1_Block_A->FRCR = 0x0006007f;
    SAI1_Block_A->FRCR = static_cast<uint32_t>(SaiBase::FrameLength_t(16*8-1).value) |
                         static_cast<uint32_t>(SaiBase::FrameConfigurationRegister::FSPOL) |
                         static_cast<uint32_t>(SaiBase::FrameConfigurationRegister::FSOFF);

    MODM_LOG_INFO << "A FRCR: 0x" << modm::hex << (uint32_t)SAI1_Block_A->FRCR << modm::endl;
    // SAI1_Block_A->SLOTR = 0x007f0700;
    SAI1_Block_A->SLOTR = static_cast<uint32_t>(SaiBase::SlotNumber_t(8-1).value) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN0) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN1) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN2) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN3) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN4) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN5) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN6);
    MODM_LOG_INFO << "A SLOTR: 0x" << modm::hex << (uint32_t)SAI1_Block_A->SLOTR << modm::endl;

    // SAI1_Block_B->CR1 = 0x00200280;
    SAI1_Block_B->CR1 = static_cast<uint32_t>(SaiBase::DataSize::DataSize16Bit) |
                        static_cast<uint32_t>(SaiBase::SyncEn::Async) |
                        static_cast<uint32_t>(SaiBase::ConfigurationRegister1::CKSTR) |
                        static_cast<uint32_t>(SaiBase::Mode::SlaveReceiver);
    MODM_LOG_INFO << "B CR1: 0x" << modm::hex << (uint32_t)SAI1_Block_B->CR1 << modm::endl;
    SAI1_Block_B->CR2 = 0x00000000;
    MODM_LOG_INFO << "B CR2: 0x" << modm::hex << (uint32_t)SAI1_Block_B->CR2 << modm::endl;
    // SAI1_Block_B->FRCR = 0x0006007f;
    SAI1_Block_B->FRCR = static_cast<uint32_t>(SaiBase::FrameLength_t(16*8-1).value) |
                         static_cast<uint32_t>(SaiBase::FrameConfigurationRegister::FSPOL) |
                         static_cast<uint32_t>(SaiBase::FrameConfigurationRegister::FSOFF);

    MODM_LOG_INFO << "B FRCR: 0x" << modm::hex << (uint32_t)SAI1_Block_B->FRCR << modm::endl;
    // SAI1_Block_B->SLOTR = 0x007f0700;
    SAI1_Block_B->SLOTR = static_cast<uint32_t>(SaiBase::SlotNumber_t(8-1).value) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN0) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN1);
    MODM_LOG_INFO << "B SLOTR: 0x" << modm::hex << (uint32_t)SAI1_Block_B->SLOTR << modm::endl;

    // Enable FIFO request interrupt
    SaiB::Hal::enableInterrupt(SaiBase::Interrupt::FIFORequest);
    MODM_LOG_INFO << "IMR: 0x" << modm::hex << (uint32_t)SAI1_Block_B->IMR << modm::endl;
    SaiB::Hal::enableInterruptVector(true, 10);

    // Set SAIEN
    SaiA::Hal::enableTransfer();
    SaiB::Hal::enableTransfer();

    while (true)
    {

        if (tmr.execute())
        {
            // LedGreen::toggle();
            MODM_LOG_INFO << "Hello World digital crossover" << modm::endl;
        }
    }

    return 0;
}

MODM_ISR(SAI1)
{
    // Check if interrupt is FIFORequest
    if ( (SAI1_Block_B->SR & (1 << 3)) != 0) {

        Debug::set();
        LedGreen::toggle();

        int32_t input_sample_l = SAI1_Block_B->DR;
        int32_t input_sample_r = SAI1_Block_B->DR;

        SAI1_Block_A->DR = (int32_t)woofer_sample_l;
        SAI1_Block_A->DR = (int32_t)mid_sample_l;
        SAI1_Block_A->DR = (int32_t)tweeter_sample_l;
        SAI1_Block_A->DR = (int32_t)woofer_sample_r;
        SAI1_Block_A->DR = (int32_t)mid_sample_r;
        SAI1_Block_A->DR = (int32_t)tweeter_sample_r;
        SAI1_Block_A->DR = input_sample_l;

        woofer_sample_l = woofer_lpf_2_l.process(woofer_tmp_l);
        woofer_tmp_l = woofer_lpf_1_l.process((double)input_sample_l);

        mid_sample_l = mid_lpf_2_l.process(mid_tmp2_l);
        mid_tmp2_l = mid_lpf_1_l.process(mid_tmp1_l);

        mid_tmp1_l = mid_hpf_2_l.process(mid_tmp0_l);
        mid_tmp0_l = mid_hpf_1_l.process((double)input_sample_l);

        tweeter_sample_l = tweeter_hpf_2_l.process(tweeter_tmp_l);
        tweeter_tmp_l = tweeter_hpf_1_l.process((double)input_sample_l);

        woofer_sample_r = woofer_lpf_2_r.process(woofer_tmp_r);
        woofer_tmp_r = woofer_lpf_1_r.process((double)input_sample_r);

        mid_sample_r = mid_lpf_2_r.process(mid_tmp2_r);
        mid_tmp2_r = mid_lpf_1_r.process(mid_tmp1_r);

        mid_tmp1_r = mid_hpf_2_r.process(mid_tmp0_r);
        mid_tmp0_r = mid_hpf_1_r.process((double)input_sample_r);

        tweeter_sample_r = tweeter_hpf_2_r.process(tweeter_tmp_r);
        tweeter_tmp_r = tweeter_hpf_1_r.process((double)input_sample_r);

        Debug::reset();
    }
}
