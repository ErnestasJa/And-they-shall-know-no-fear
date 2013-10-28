#pragma once

struct message
{
	uint32_t type;
	uint32_t timestamp;
	
	message(uint32_t msg_type) type(msg_type){}
};