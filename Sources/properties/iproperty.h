#pragma once


class IProperty
{
public:
	virtual ~IProperty(){}

	virtual void set(IProperty * other)=0;
	
	virtual bool is_null() const=0;
	virtual const std::string & get_name() const=0;
	virtual uint32_t get_type() const=0;
	virtual uint32_t get_flags() const=0;

	virtual void serialize(clan::File & f)const=0;
	virtual void deserialize(clan::File & f)=0;
	virtual void net_value_serialize(clan::NetGameEventValue & e)const=0;
	virtual void net_value_deserialize(const clan::NetGameEventValue & e)=0;
};