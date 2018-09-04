// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_LICHT_HPP
#   error "Don't include this file directly, use 'ding_licht.hpp' instead!"
#endif

#include "licht.hpp"


template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
ding::LichtData ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::current_color;
template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
ding::LichtData ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::next_color;

ding::LichtMessage::LichtMessage(uint8_t destination_id, uint8_t source_id, LichtData rgbw)
{
	payload_bytes = sizeof(LichtData);
	destination   = destination_id;
	source        = source_id;
	type          = ding::MessageType::Rgbw;

	payload[0] = rgbw.red;
	payload[1] = rgbw.green;
	payload[2] = rgbw.blue;
	payload[3] = rgbw.white;
}

ding::LichtMessage::LichtMessage(const ding::Message& message)
{
	payload_bytes = message.payload_bytes;
	destination   = message.destination;
	source        = message.source;
	type          = message.type;
	payload       = message.payload;
}

ding::LichtData ding::LichtMessage::getLichtData()
{
	return ding::LichtData(payload[0], payload[1], payload[2], payload[3]);
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void
ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::begin(ding::LichtData initial_color)
{
	RED::setOutput(0);
	GREEN::setOutput(0);
	BLUE::setOutput(0);
	WHITE::setOutput(0);

	current_color = ding::LichtData(0,0,0,0);
	next_color = initial_color;

	fade();
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void
ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::setRaw(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
	RED::set(r);
	GREEN::set(g);
	BLUE::set(b);
	WHITE::set(w);
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void
ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::set(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
	RED::set(lut[r]);
	GREEN::set(lut[g]);
	BLUE::set(lut[b]);
	WHITE::set(lut[w]);
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::fade(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
{
	next_color = ding::LichtData(r,g,b,w);
	fade();
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::fade()
{
	// fade
	if (current_color != next_color)
	{
		LichtData prev_color = current_color;

		for(float f=0.0; f<=1.0; f+=0.01) {
			current_color.red   = prev_color.red   + (next_color.red   - prev_color.red  )*f;
			current_color.green = prev_color.green + (next_color.green - prev_color.green)*f;
			current_color.blue  = prev_color.blue  + (next_color.blue  - prev_color.blue )*f;
			current_color.white = prev_color.white + (next_color.white - prev_color.white)*f;

			set(current_color.red, current_color.green, current_color.blue, current_color.white);
			modm::delayMilliseconds(15);
		}
	}
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::receive()
{
	LichtMessage message = Ding<NRF24>::receive();

	if (message.destination == ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::device_id && message.type == ding::MessageType::Rgbw)
		next_color = message.getLichtData();
}

template <typename NRF24, typename RED, typename GREEN, typename BLUE, typename WHITE>
void ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::send()
{
	ding::LichtMessage message = ding::LichtMessage(0x00, ding::Licht<NRF24, RED, GREEN, BLUE, WHITE>::device_id, current_color);
	Ding<NRF24>::send(message);
}
