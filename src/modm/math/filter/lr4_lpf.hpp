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

namespace modm
{
	namespace filter {

        template<typename T, T fc, T fs>
        class Lr4Lpf
        {
        public:
            inline void
            reset()
            {
                butterworth_lpf_1.reset();
                butterworth_lpf_2.reset();
            }

            inline void
            update(const T& input)
            {
                butterworth_lpf_1.update(input);
                butterworth_lpf_2.update(butterworth_lpf_1.getValue());
            }

            inline const T&
            getValue()
            {
                return butterworth_lpf_2.getValue();
            }

        private:
            SoButterworthLpf<T, fc, fs> butterworth_lpf_1;
            SoButterworthLpf<T, fc, fs> butterworth_lpf_2;
        };
    }
}