// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_MESSAGE_HPP
#   error "Don't include this file directly, use 'ding_message.hpp' instead!"
#endif

#include "ding_message.hpp"

modm::Vector<uint8_t, 32> ding::Message::getPacket() const
{
	modm::Vector<uint8_t, 32> packet;

	packet[0] = destination;
	packet[1] = source;
	packet[2] = (uint8_t)type;
	for (int i=0; i<payload_bytes; i++)
		packet[i+3] = payload[i];

	return packet;
}

ding::Message::Message(uint8_t size, modm::Vector<uint8_t, 32> packet)
{
	payload_bytes = size;
	destination   = packet[0];
	source        = packet[1];
	type          = (ding::MessageType)packet[2];

	for (int i=0; i<size; i++)
		payload[i] = packet[i+3];
}

ding::Message::Message(modm::Vector<uint8_t, 32> packet)
{
	payload_bytes = 29;
	destination   = packet[0];
	source        = packet[1];
	type          = (ding::MessageType)packet[2];

	for (int i=0; i<29; i++)
		payload[i] = packet[i+3];
}

ding::Message::Message()
{
	payload_bytes = 0;
	source        = 0xff;
	destination   = 0xff;
	type          = ding::MessageType::Error;
}

ding::Message::Message(uint8_t destination_id, uint8_t source_id, uint16_t value)
{
	payload_bytes = 2;
	destination   = destination_id;
	source        = source_id;
	type          = ding::MessageType::Integer;
	payload[0]    = (value>>0) & 0xff;
	payload[1]    = (value>>8) & 0xff;
}

uint16_t ding::Message::getInteger()
{
	return (payload[1] << 8) | (payload[0]);
}
