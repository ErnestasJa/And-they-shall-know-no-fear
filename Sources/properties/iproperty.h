#pragma once

#include "serialization/iserializable.h"

class IProperty: public ISerializable, public INetValueSerializable
{
public:
	virtual ~IProperty(){}

	virtual void set(IProperty * other)=0;
	
	virtual bool is_null() const=0;
	virtual const std::string & get_name() const=0;
	virtual uint32_t get_type() const=0;
	virtual uint32_t get_flags() const=0;
};