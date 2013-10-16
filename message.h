#pragma once

class GameObject;

struct message
{
	uint32_t type;
	uint32_t timestamp;

	GameObject * sender;
	
	message(uint32_t msg_type) type(msg_type){}
};