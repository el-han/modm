// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_LICHT_HPP
#define MODM_DING_LICHT_HPP

namespace ding
{

#include <stdint.h>

#include "ding.hpp"
#include "ding_message.hpp"

class LichtData {
public:
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t white;

	// LichtData(uint8_t r, uint8_t g, uint8_t b, uint8_t w) : red(r), green(g), blue(b), white(w) {};
	// LichtData() : red(0), green(0), blue(0), white(0) {};

	constexpr LichtData(uint8_t r, uint8_t g, uint8_t b, uint8_t w) : red(r), green(g), blue(b), white(w) {};
	constexpr LichtData() : red(0), green(0), blue(0), white(0) {};

	bool operator==(const LichtData& p) const
	{
		return (this->red==p.red) && (this->green==p.green) && (this->blue==p.blue) && (this->white==p.white);
	}
	bool operator!=(const LichtData& p) const
	{
		return (this->red!=p.red) || (this->green!=p.green) || (this->blue!=p.blue) || (this->white!=p.white);
	}
};

class LichtMessage : public ding::Message {
public:

	LichtMessage(uint8_t destination_id, uint8_t source_id, LichtData rgbw);
	LichtMessage(const ding::Message& message);

	LichtData getLichtData();
};

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
class Licht : public Ding<NRF24>
{
public:
	static void begin(LichtData initial_color = LichtData(0,0,0,0));
	static void fade();
	static void fade(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	static void set(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	static void receive();
	static void send();

protected:
	static LichtData current_color;
	static LichtData next_color;

private:
	static constexpr uint8_t lut[32] = {
		  0,   1,   2,   3,   4,   5,   7,   9,
		 12,  15,  18,  22,  27,  32,  38,  44,
		 51,  58,  67,  76,  86,  96, 108, 120,
		134, 148, 163, 180, 197, 216, 235, 255
};

};

} // namespace ding

#include "ding_licht_impl.hpp"

#endif
