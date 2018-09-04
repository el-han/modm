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
#include "message.hpp"

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
	static void setRaw(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
	static void receive();
	static void send();

protected:
	static LichtData current_color;
	static LichtData next_color;

private:
	static constexpr uint8_t lut[256] = {
		//   0,   1,   2,   3,   4,   5,   7,   9,
		//  12,  15,  18,  22,  27,  32,  38,  44,
		//  51,  58,  67,  76,  86,  96, 108, 120,
		// 134, 148, 163, 180, 197, 216, 235, 255

		  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   1,   1,   1,   1,   1,
		  1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
		  2,   2,   3,   3,   3,   3,   3,   4,   4,   4,
		  4,   5,   5,   5,   5,   6,   6,   6,   6,   7,
		  7,   7,   8,   8,   8,   9,   9,   9,  10,  10,
		 11,  11,  11,  12,  12,  13,  13,  13,  14,  14,
		 15,  15,  16,  16,  17,  17,  18,  18,  19,  19,
		 20,  20,  21,  22,  22,  23,  23,  24,  25,  25,
		 26,  26,  27,  28,  28,  29,  30,  30,  31,  32,
		 33,  33,  34,  35,  35,  36,  37,  38,  39,  39,
		 40,  41,  42,  43,  43,  44,  45,  46,  47,  48,
		 49,  49,  50,  51,  52,  53,  54,  55,  56,  57,
		 58,  59,  60,  61,  62,  63,  64,  65,  66,  67,
		 68,  69,  70,  71,  73,  74,  75,  76,  77,  78,
		 79,  81,  82,  83,  84,  85,  87,  88,  89,  90,
		 91,  93,  94,  95,  97,  98,  99, 100, 102, 103,
		105, 106, 107, 109, 110, 111, 113, 114, 116, 117,
		119, 120, 121, 123, 124, 126, 127, 129, 130, 132,
		133, 135, 137, 138, 140, 141, 143, 145, 146, 148,
		149, 151, 153, 154, 156, 158, 159, 161, 163, 165,
		166, 168, 170, 172, 173, 175, 177, 179, 181, 182,
		184, 186, 188, 190, 192, 194, 196, 197, 199, 201,
		203, 205, 207, 209, 211, 213, 215, 217, 219, 221,
		223, 225, 227, 229, 231, 234, 236, 238, 240, 242,
		244, 246, 248, 251, 253, 255
};

};

} // namespace ding

#include "licht_impl.hpp"

#endif
