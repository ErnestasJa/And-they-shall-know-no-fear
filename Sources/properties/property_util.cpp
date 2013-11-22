#include "precomp.h"
#include "property.h"
#include "property_util.h"

///Type id
#define X(type,type_enum) template <> uint32_t get_type_id<type>(){return (type_enum) ;}
TYPE_LIST
#undef X



///Data to Net game event data
template <>
void serialize_type<bool>(clan::NetGameEventValue & v, const bool & data)
{
	clan::NetGameEventValue val((bool)data);
	v.add_member(val);
}

template <>
void serialize_type<clan::vec2>(clan::NetGameEventValue & v, const clan::vec2 & data)
{
	clan::NetGameEventValue vec(clan::NetGameEventValue::complex);
	vec.add_member(data.x);
	vec.add_member(data.y);
	v.add_member(vec);
}

template <>
void serialize_type<clan::vec2f>(clan::NetGameEventValue & v, const clan::vec2f & data)
{
	clan::NetGameEventValue vec(clan::NetGameEventValue::complex);
	vec.add_member(data.x);
	vec.add_member(data.y);
	v.add_member(vec);
}

template <>
void serialize_type<PropertyContainer>(clan::NetGameEventValue & e, const PropertyContainer & data)
{
	clan::NetGameEventValue props(clan::NetGameEventValue::complex);
	
	data.net_serialize(props);
	e.add_member(props);

}

template <> void deserialize_type(const clan::NetGameEventValue & v, bool & data){data = v.to_boolean();}
template <> void deserialize_type(const clan::NetGameEventValue & v, int32_t & data){data = v.to_integer();}
template <> void deserialize_type(const clan::NetGameEventValue & v, uint32_t & data){data = v.to_uinteger();}
template <> void deserialize_type(const clan::NetGameEventValue & v, float & data){data = v.to_number();}
template <> void deserialize_type(const clan::NetGameEventValue & v, clan::vec2 & data){data.x = v.get_member(0).to_integer(); data.y = v.get_member(1).to_integer();}
template <> void deserialize_type(const clan::NetGameEventValue & v, clan::vec2f & data){data.x = v.get_member(0).to_number(); data.y = v.get_member(1).to_number();}
template <> void deserialize_type(const clan::NetGameEventValue & v, std::string & data){data = v.to_string();}

template <> void deserialize_type(const clan::NetGameEventValue & v, PropertyContainer & data)
{
	data.net_deserialize(v);
}

///property serialization to file
template <>
void serialize_type(clan::File & f, const bool & data)
{
	f.write_uint8((uint8_t) data);
}

template <>
void serialize_type(clan::File & f, const int32_t & data)
{
	f.write_int32( data );
}

template <>
void serialize_type(clan::File & f, const uint32_t & data)
{
	f.write_uint32( data );
}

template <>
void serialize_type(clan::File & f, const float & data)
{
	f.write_float( data );
}

template <>
void serialize_type(clan::File & f, const clan::vec2 & data)
{
	f.write_int32( data.x );
	f.write_int32( data.y );
}

template <>
void serialize_type(clan::File & f, const clan::vec2f & data)
{
	f.write_float( data.x );
	f.write_float( data.y );
}

template <>
void serialize_type(clan::File & f, const std::string & data)
{
	f.write_string_a( data );
}

template <>
void serialize_type(clan::File & f, const PropertyContainer & data)
{
	data.serialize(f);
}

///property deserialization to file
template <>
void deserialize_type(clan::File & f, bool & data)
{
	data = f.read_uint8();
}

template <>
void deserialize_type(clan::File & f, int32_t & data)
{
	data=f.read_int32();
}

template <>
void deserialize_type(clan::File & f, uint32_t & data)
{
	data=f.read_uint32();
}

template <>
void deserialize_type(clan::File & f, float & data)
{
	data=f.read_float();
}

template <>
void deserialize_type(clan::File & f, clan::vec2 & data)
{
	data.x=f.read_int32();
	data.y=f.read_int32();
}

template <>
void deserialize_type(clan::File & f, clan::vec2f & data)
{
	data.x=f.read_float();
	data.y=f.read_float();
}

template <>
void deserialize_type(clan::File & f, std::string & data)
{
	data=f.read_string_a();
}

template <>
void deserialize_type(clan::File & f, PropertyContainer & data)
{
	data.deserialize(f);
}


///property serialization to xml
template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const bool & data)
{
	n.set_attribute("data",clan::StringHelp::bool_to_text(data));
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const int32_t & data)
{
	n.set_attribute("data",clan::StringHelp::int_to_text(data));
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const uint32_t & data)
{
	n.set_attribute("data",clan::StringHelp::uint_to_text(data));
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const float & data)
{
	n.set_attribute("data",clan::StringHelp::float_to_text(data));
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const clan::vec2 & data)
{
	n.set_attribute("x",clan::StringHelp::int_to_text(data.x));
	n.set_attribute("y",clan::StringHelp::int_to_text(data.y));
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const clan::vec2f & data)
{
	n.set_attribute("x",clan::StringHelp::float_to_text(data.x));
	n.set_attribute("y",clan::StringHelp::float_to_text(data.y));
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const std::string & data)
{
	n.set_attribute("data",data);
}

template <>
void serialize_type(clan::DomDocument & f, clan::DomElement & n, const PropertyContainer & data)
{
	data.xml_serialize(f,n);
}


///property xml deserialization
template <>
void deserialize_type(clan::DomElement & n, bool & data)
{
	data = clan::StringHelp::text_to_bool(n.get_attribute("data"));
}

template <>
void deserialize_type(clan::DomElement & n, int32_t & data)
{
	data = clan::StringHelp::text_to_int(n.get_attribute("data"));
}

template <>
void deserialize_type(clan::DomElement & n, uint32_t & data)
{
	data = clan::StringHelp::text_to_uint(n.get_attribute("data"));
}

template <>
void deserialize_type(clan::DomElement & n, float & data)
{
	data = clan::StringHelp::text_to_float(n.get_attribute("data"));
}

template <>
void deserialize_type(clan::DomElement & n, clan::vec2 & data)
{
	data.x=clan::StringHelp::text_to_int(n.get_attribute("x"));
	data.y=clan::StringHelp::text_to_int(n.get_attribute("y"));
}

template <>
void deserialize_type(clan::DomElement & n, clan::vec2f & data)
{
	data.x=clan::StringHelp::text_to_float(n.get_attribute("x"));
	data.y=clan::StringHelp::text_to_float(n.get_attribute("y"));
}

template <>
void deserialize_type(clan::DomElement & n, std::string & data)
{
	data = n.get_attribute("data");
}

template <>
void deserialize_type(clan::DomElement & n, PropertyContainer & data)
{
	data.xml_deserialize(n.get_first_child_element());
}