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

#include "biquad.hpp"
#include <cmath>
#include <numbers>

using std::numbers::pi;
using std::numbers::sqrt2;

namespace modm
{
	namespace filter {

        template<typename T, T fc, T fs>
        class SoButterworthLpf : public Biquad<
            T,
            1.0 / (1.0 + sqrt2*(1.0 / (std::tan(pi*fc / fs))) + std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0) ),
            2.0 * (1.0 / (1.0 + sqrt2*(1.0 / (std::tan(pi*fc / fs))) + std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0) )),
            1.0 / (1.0 + sqrt2*(1.0 / (std::tan(pi*fc / fs))) + std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0) ),
            2.0 * (1.0 / (1.0 + sqrt2*(1.0 / (std::tan(pi*fc / fs))) + std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0) ))*(1.0 - std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0)),
            (1.0 / (1.0 + sqrt2*(1.0 / (std::tan(pi*fc / fs))) + std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0) )) * (1.0 - sqrt2*(1.0 / (std::tan(pi*fc / fs))) + std::pow((1.0 / (std::tan(pi*fc / fs))), 2.0) )
        >
        {};
    }
}