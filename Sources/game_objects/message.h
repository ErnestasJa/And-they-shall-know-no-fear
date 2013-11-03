#pragma once

#include "properties/property.h"
#include "properties/property_container.h"

enum MESSAGE_TYPE
{
	MSG_UPDATE=0,
};

class Message: public PropertyContainer
{
public:
	Property<uint32_t> type;	  ///enumerated types
	Property<uint32_t> timestamp; ///time in ms
	
	Message(uint32_t msg_type, uint32_t time_stamp)
	{
		type = add_property<uint32_t>("type",msg_type);
		type = add_property<uint32_t>("timestamp",0);
	}
};