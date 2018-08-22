// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DING_MESSAGE_HPP
#define MODM_DING_MESSAGE_HPP

namespace ding
{

enum class MessageType : uint8_t {
	Integer        = 0,
	Switch         = 1,
	Humi           = 2,
	Rgbw           = 3,
	HumiDisplay    = 4,
	RgbwController = 5,
	Error          = 0xff
};

class Message {
public:
	uint8_t                   payload_bytes;
	uint8_t                   destination;
	uint8_t                   source;
	ding::MessageType         type;
	modm::Vector<uint8_t, 29> payload;

	modm::Vector<uint8_t, 32> getPacket() const;

	Message();
	Message(uint8_t size, modm::Vector<uint8_t, 32> packet);
	Message(modm::Vector<uint8_t, 32> packet);
	Message(uint8_t destination_id, uint8_t source_id, uint16_t value);

	uint16_t getInteger();
};

} // namespace modm

#include "ding_message_impl.hpp"

#endif
