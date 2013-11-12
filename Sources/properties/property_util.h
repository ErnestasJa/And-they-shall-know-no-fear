#pragma once

/**
Most functions in this header are not meant to be used directly.
**/

#include "property_types.h"
#include "property_container.h"

void register_property_types();

///type ids
template <class T>
uint32_t get_type_id();

///X Macro, google it
#define TYPE_LIST \
X(bool,			EPT_BOOL)\
X(int32_t,		EPT_INT32)\
X(uint32_t,		EPT_UINT32)\
X(float,		EPT_FLOAT)\
X(clan::vec2,	EPT_VEC2)\
X(clan::vec2f,	EPT_VEC2F)\
X(std::string,	EPT_STRING)

#define X(type,type_enum) template <> uint32_t get_type_id<type>();
TYPE_LIST
#undef X

///data <-> net game event
template <class T>
void serialize_type(clan::NetGameEventValue & v, const T & data)
{
	v.add_member(data);
}

template <> void serialize_type<bool>(clan::NetGameEventValue & e, const bool & data);
template <> void serialize_type<clan::vec2>(clan::NetGameEventValue & e, const clan::vec2 & data);
template <> void serialize_type<clan::vec2f>(clan::NetGameEventValue & e, const clan::vec2f & data);

template <class T>
void deserialize_type(const clan::NetGameEventValue & v, T & data);

///data <-> file
template <class T>
void serialize_type(clan::File & f, const T & data);

template <class T>
void deserialize_type(clan::File & f, T & data);