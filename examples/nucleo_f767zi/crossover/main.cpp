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

#include <tusb.h>
#include <so_butterworth_hpf.h>
#include <so_butterworth_lpf.h>

#include <cstdint>
#include <modm/board.hpp>
#include <modm/io.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <modm/processing.hpp>


volatile uint32_t current_sample = 0;
int16_t samples[] = {
    0,
    267,
    529,
    783,
    1023,
    1246,
    1447,
    1623,
    1772,
    1891,
    1977,
    2029,
    2047,
    2029,
    1977,
    1891,
    1772,
    1623,
    1447,
    1246,
    1023,
    783,
    529,
    267,
    0,
    -267,
    -529,
    -783,
    -1023,
    -1246,
    -1447,
    -1623,
    -1772,
    -1891,
    -1977,
    -2029,
    -2047,
    -2029,
    -1977,
    -1891,
    -1772,
    -1623,
    -1447,
    -1246,
    -1023,
    -783,
    -529,
    -267,
};

enum
{
    VOLUME_CTRL_0_DB = 0,
    VOLUME_CTRL_10_DB = 2560,
    VOLUME_CTRL_20_DB = 5120,
    VOLUME_CTRL_30_DB = 7680,
    VOLUME_CTRL_40_DB = 10240,
    VOLUME_CTRL_50_DB = 12800,
    VOLUME_CTRL_60_DB = 15360,
    VOLUME_CTRL_70_DB = 17920,
    VOLUME_CTRL_80_DB = 20480,
    VOLUME_CTRL_90_DB = 23040,
    VOLUME_CTRL_100_DB = 25600,
    VOLUME_CTRL_SILENCE = 0x8000,
};
using namespace Board;

SO_BUTTERWORTH_LPF woofer_lpf_1_l;
SO_BUTTERWORTH_LPF woofer_lpf_2_l;
SO_BUTTERWORTH_LPF mid_lpf_1_l;
SO_BUTTERWORTH_LPF mid_lpf_2_l;
SO_BUTTERWORTH_HPF mid_hpf_1_l;
SO_BUTTERWORTH_HPF mid_hpf_2_l;
SO_BUTTERWORTH_HPF tweeter_hpf_1_l;
SO_BUTTERWORTH_HPF tweeter_hpf_2_l;
volatile double woofer_tmp_l = 0.0;
volatile double woofer_sample_l = 0.0;
volatile double mid_tmp0_l = 0.0;
volatile double mid_tmp1_l = 0.0;
volatile double mid_tmp2_l = 0.0;
volatile double mid_sample_l = 0.0;
volatile double tweeter_tmp_l = 0.0;
volatile double tweeter_sample_l = 0.0;

SO_BUTTERWORTH_LPF woofer_lpf_1_r;
SO_BUTTERWORTH_LPF woofer_lpf_2_r;
SO_BUTTERWORTH_LPF mid_lpf_1_r;
SO_BUTTERWORTH_LPF mid_lpf_2_r;
SO_BUTTERWORTH_HPF mid_hpf_1_r;
SO_BUTTERWORTH_HPF mid_hpf_2_r;
SO_BUTTERWORTH_HPF tweeter_hpf_1_r;
SO_BUTTERWORTH_HPF tweeter_hpf_2_r;
volatile double woofer_tmp_r = 0.0;
volatile double woofer_sample_r = 0.0;
volatile double mid_tmp0_r = 0.0;
volatile double mid_tmp1_r = 0.0;
volatile double mid_tmp2_r = 0.0;
volatile double mid_sample_r = 0.0;
volatile double tweeter_tmp_r = 0.0;
volatile double tweeter_sample_r = 0.0;

using Sai = SaiMaster1;

using Mclk = GpioOutputE2;
using Fs = GpioOutputE4;
using Sck = GpioOutputE5;
using Sd = GpioOutputE6;

using Debug = GpioOutputG0;
using Freq = GpioOutputG1;

modm::PeriodicTimer tmr{2.5s};

// Invoked when device is mounted
void tud_mount_cb() { tmr.restart(1s); }
// Invoked when device is unmounted
void tud_umount_cb() { tmr.restart(250ms); }
// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool) { tmr.restart(2.5s); }
// Invoked when usb bus is resumed
void tud_resume_cb() { tmr.restart(1s); }

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTOTYPES
//--------------------------------------------------------------------+

// Buffer for speaker data
int16_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 4];

// List of supported sample rates
const uint32_t sample_rates[] = {48000};
uint32_t current_sample_rate  = 48000;

#define N_SAMPLE_RATES  TU_ARRAY_SIZE(sample_rates)

// uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

// Audio controls
// Current states
int8_t mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];       // +1 for master channel 0
int16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];    // +1 for master channel 0

// Speaker data size received in the last frame
volatile int spk_data_size = 0;
int spk_data_size_q = 0;
// Resolution per format
const uint8_t resolutions_per_format[CFG_TUD_AUDIO_FUNC_1_N_FORMATS] = {CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX};
// Current resolution, update on format change
uint8_t current_resolution;


int main()
{
    Board::initialize();
    Board::initializeUsbFs();

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
    Freq::setOutput(modm::Gpio::High);

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
    SAI1_Block_A->SLOTR = static_cast<uint32_t>(SaiBase::SlotNumber_t(7).value) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN0) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN1) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN2) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN3) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN4) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN5) |
                          static_cast<uint32_t>(SaiBase::SlotRegister::SLOTEN6);

    // Enable FIFO request interrupt
    Sai::HalA::enableInterrupt(SaiBase::Interrupt::FIFORequest);
    Sai::HalA::enableInterruptVector(true, 10);

    tusb_init();

    // Set SAIEN
    Sai::HalA::enableTransfer();

    while (true)
    {
        tud_task();

        if (tmr.execute())
        {
            LedGreen::toggle();
            // usb_stream << "Hello World from USB: " << (counter++) << "\r\n";
        }

        if (spk_data_size != spk_data_size_q)
        {
            MODM_LOG_DEBUG << "spk_data_size: " << (int)spk_data_size << modm::endl;
            spk_data_size_q = spk_data_size;
        }

        // Debug::toggle();
    }

    return 0;
}

MODM_ISR(SAI1)
{
    // Check if interrupt is FIFORequest
    if ( (SAI1_Block_A->SR & (1 << 3)) != 0) {
        Freq::set();
        // if (spk_data_size >= 4) {
        //     Debug::reset();
        // } else {
        //     Debug::set();
        // }
        SAI1_Block_A->DR = (int32_t)woofer_sample_l;
        SAI1_Block_A->DR = (int32_t)mid_sample_l;
        SAI1_Block_A->DR = (int32_t)tweeter_sample_l;
        SAI1_Block_A->DR = (int32_t)woofer_sample_r;
        SAI1_Block_A->DR = (int32_t)mid_sample_r;
        SAI1_Block_A->DR = (int32_t)tweeter_sample_r;
        SAI1_Block_A->DR = (int32_t)(spk_buf[1]/32);
        // SAI1_Block_A->DR = (int32_t)(samples[current_sample]);

        Debug::set();
        // SAI1_Block_A->DR = spk_buf[1];
        spk_data_size = tud_audio_read((void*)spk_buf, 4);
        Debug::reset();

        woofer_sample_l = woofer_lpf_2_l.process(woofer_tmp_l);
        woofer_tmp_l = woofer_lpf_1_l.process(spk_buf[0]);

        mid_sample_l = mid_lpf_2_l.process(mid_tmp2_l);
        mid_tmp2_l = mid_lpf_1_l.process(mid_tmp1_l);

        mid_tmp1_l = mid_hpf_2_l.process(mid_tmp0_l);
        mid_tmp0_l = mid_hpf_1_l.process(spk_buf[0]);

        tweeter_sample_l = tweeter_hpf_2_l.process(tweeter_tmp_l);
        tweeter_tmp_l = tweeter_hpf_1_l.process(spk_buf[0]);

        woofer_sample_r = woofer_lpf_2_r.process(woofer_tmp_r);
        woofer_tmp_r = woofer_lpf_1_r.process(spk_buf[1]);

        mid_sample_r = mid_lpf_2_r.process(mid_tmp2_r);
        mid_tmp2_r = mid_lpf_1_r.process(mid_tmp1_r);

        mid_tmp1_r = mid_hpf_2_r.process(mid_tmp0_r);
        mid_tmp0_r = mid_hpf_1_r.process(spk_buf[1]);

        tweeter_sample_r = tweeter_hpf_2_r.process(tweeter_tmp_r);
        tweeter_tmp_r = tweeter_hpf_1_r.process(spk_buf[1]);

        current_sample = (current_sample + 1) % (sizeof(samples) / sizeof(samples[0]));

        Freq::reset();
    }
}

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
   // TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
            // TU_LOG1("Clock get current freq %" PRIu32 "\r\n", current_sample_rate);

            audio_control_cur_4_t curf = { (int32_t) tu_htole32(current_sample_rate) };
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
        }
        else if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_4_n_t(N_SAMPLE_RATES) rangef;
            rangef.wNumSubRanges = tu_htole16(N_SAMPLE_RATES);
            // TU_LOG1("Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
            for(uint8_t i = 0; i < N_SAMPLE_RATES; i++)
            {
                rangef.subrange[i].bMin = (int32_t) sample_rates[i];
                rangef.subrange[i].bMax = (int32_t) sample_rates[i];
                rangef.subrange[i].bRes = 0;
                // TU_LOG1("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
            }

            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
        }
    }
    else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
             request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t cur_valid = { .bCur = 1 };
        // TU_LOG1("Clock get is valid %u\r\n", cur_valid.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
    }
    // TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
    //                 request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

// Helper for clock set requests
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    // TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);
    // TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        // TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

        current_sample_rate = (uint32_t) ((audio_control_cur_4_t const *)buf)->bCur;

        // TU_LOG1("Clock set current freq: %" PRIu32 "\r\n", current_sample_rate);

        return true;
    }
    else
    {
        // TU_LOG1("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
        //                 request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

// Helper for feature unit get requests
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
    // TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t mute1 = { .bCur = mute[request->bChannelNumber] };
        // TU_LOG1("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
    }
    else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_2_n_t(1) range_vol;
            range_vol.wNumSubRanges = tu_htole16(1),
            range_vol.subrange[0].bMin = tu_htole16(-VOLUME_CTRL_50_DB);
            range_vol.subrange[0].bMax = tu_htole16(VOLUME_CTRL_0_DB);
            range_vol.subrange[0].bRes = tu_htole16(256);
            // TU_LOG1("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
            //                 range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
        }
        else if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
            audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(volume[request->bChannelNumber]) };
            // TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
        }
    }
    // TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
    //                 request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

// Helper for feature unit set requests
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
    (void)rhport;

    // TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);
    // TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
    {
        // TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

        mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

        // TU_LOG1("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

        return true;
    }
    else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        // TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

        volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

        // TU_LOG1("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

        return true;
    }
    else
    {
        // TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
        //                 request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return tud_audio_clock_get_request(rhport, request);
    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
        return tud_audio_feature_unit_get_request(rhport, request);
    else
    {
        // TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
        //                request->bEntityID, request->bControlSelector, request->bRequest);
    }
    return false;
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
    audio_control_request_t const *request = (audio_control_request_t const *)p_request;

    if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
        return tud_audio_feature_unit_set_request(rhport, request, buf);
    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return tud_audio_clock_set_request(rhport, request, buf);
    // TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
    //                request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
    (void)rhport;

    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt == 0) {
        // Audio streaming stop
        // blink_interval_ms = BLINK_MOUNTED;
    }

    return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
    (void)rhport;
    uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    // TU_LOG2("Set interface %d alt %d\r\n", itf, alt);
    if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt != 0) {
        // Audio streaming start
        // blink_interval_ms = BLINK_STREAMING;
    }

    // Clear buffer when streaming format is changed
    spk_data_size = 0;
    if(alt != 0)
    {
        current_resolution = resolutions_per_format[alt-1];
    }

    return true;
}

void tud_audio_feedback_params_cb(uint8_t func_id, uint8_t alt_itf, audio_feedback_params_t* feedback_param)
{
    (void)func_id;
    (void)alt_itf;
    // Set feedback method to fifo counting
    feedback_param->method = AUDIO_FEEDBACK_METHOD_FIFO_COUNT;
    feedback_param->sample_freq = current_sample_rate;
}
