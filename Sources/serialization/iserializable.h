#pragma once

class ISerializable
{
public:
	virtual void serialize(clan::File & file) = 0;
	virtual void deserialize(clan::File & file) = 0;
};

class INetSerializable
{
public:
	virtual void net_serialize(clan::NetGameEvent & net_event) = 0;
	virtual void net_deserialize(const clan::NetGameEvent & net_event) = 0;
};

class INetValueSerializable
{
public:
	virtual void net_value_serialize(clan::NetGameEventValue & net_value) = 0;
	virtual void net_value_serialize(const clan::NetGameEventValue & net_value) = 0;
};