/**
* Second order Butterworth band-pass filter
* Dimitris Tassopoulos 2016-2020
*
* fc , corner frequency
* BW , bandwidth of peak/notch = fc/Q
* Butterworth BPF and BSF are made by cascading (BPF) or paralleling (BSF) a Butterworth
* LPF and Butterworth HPF.
*/
#pragma once

#include "so_butterworth_lpf.hpp"

namespace modm
{
	namespace filter {

        template<typename T, T fc, T fs>
        class Lr4Hpf
        {
        public:
            inline void
            reset()
            {
                butterworth_hpf_1.reset();
                butterworth_hpf_2.reset();
            }

            inline void
            update(const T& input)
            {
                butterworth_hpf_1.update(input);
                butterworth_hpf_2.update(butterworth_hpf_1.getValue());
            }

            inline const T&
            getValue()
            {
                return butterworth_hpf_2.getValue();
            }

        private:
            SoButterworthHpf<T, fc, fs> butterworth_hpf_1;
            SoButterworthHpf<T, fc, fs> butterworth_hpf_2;
        };
    }
}