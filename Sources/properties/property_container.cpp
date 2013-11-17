#include "precomp.h"
#include "iproperty.h"
#include "property.h"
#include "property_util.h"
#include "property_container.h"


std::map<uint32_t, PropertyContainer::prop_create_func> PropertyContainer::m_prop_create;

void PropertyContainer::register_properties()
{
	#define X(type,type_enum) PropertyContainer::register_property<Property<type> >();
	TYPE_LIST
	#undef X
}

PropertyContainer::PropertyContainer()
{
	
}

PropertyContainer::~PropertyContainer()
{

}

void PropertyContainer::load_properties(const PropertyContainer & p)
{
	///FIX ME:
	for(auto it = p.m_props.begin(); it != p.m_props.end(); it++)
	{
		get_property((*it)->get_name())->set((*it));
	}
}

void PropertyContainer::add_property(IProperty * p)
{
	auto it = std::find_if(m_props.begin(), m_props.end(), [&p](IProperty * o){return o->get_name()==p->get_name();});

	if(it==m_props.end())
	{
		m_props.push_back(p);
	}
}

bool PropertyContainer::has_property(const std::string & name)
{
	const auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});
	return it != m_props.end();
}

bool PropertyContainer::has_property(const std::string & name, uint32_t type)
{
	const auto it = std::find_if(m_props.begin(), m_props.end(), [&name,&type](IProperty * o){return o->get_name()==name && o->get_type()==type;});
	return it != m_props.end();
}

IProperty * PropertyContainer::get_property(const std::string & name)
{
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
		return (*it);

	throw clan::Exception("Tried to get non-existing property");
}

bool PropertyContainer::remove_property(const std::string & name)
{
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it==m_props.end())
		return false;

	m_props.erase(it);
	return false;
}

void PropertyContainer::serialize(clan::File & file) const
{
	file.write_uint32(m_props.size());
	for(auto it = m_props.begin(); it != m_props.end(); it++)
	{
		file.write_uint32((*it)->get_type());
		(*it)->serialize(file);
	}
}

void PropertyContainer::deserialize(clan::File & file)
{
	uint32_t size = file.read_uint32();
	for(uint32_t i = 0; i < size; i++)
	{
		uint32_t type = file.read_uint32();
		IProperty * p = PropertyContainer::create_property(type,std::string());
		p->deserialize(file);

		clan::Console::write_line("Deserialized property with name='%1', type='%2'",p->get_name(),p->get_type());

		if(this->has_property(p->get_name(), p->get_type()))
		{
			this->get_property(p->get_name())->set(p);
		}
		else
		{
			this->add_property(p);
		}
	}
}

void PropertyContainer::net_serialize(clan::NetGameEventValue & e, bool only_changed) const
{
	if(only_changed)
	{
		int32_t i = 0;
		for(auto it = m_props.begin(); it != m_props.end(); it++)
		{
			const IProperty * p = (*it);
			if( (!(p->get_flags()&EPF_UNCHANGED)) || p->get_flags()&EPF_ALWAYS_SEND ) i++;
		}

		e.add_member((uint32_t)i);

		for(auto it = m_props.begin(); it != m_props.end(); it++)
		{
			const IProperty * p = (*it);

			if((!(p->get_flags()&EPF_UNCHANGED)) || p->get_flags()&EPF_ALWAYS_SEND)
			{
				clan::NetGameEventValue v(clan::NetGameEventValue::complex);
				v.add_member(p->get_type());
				p->net_value_serialize(v);
				e.add_member(v);
			}
		}
	}
	else
	{
		e.add_member((uint32_t)m_props.size());
		for(auto it = m_props.begin(); it != m_props.end(); it++)
		{
			const IProperty * p = (*it);
			
			clan::NetGameEventValue v(clan::NetGameEventValue::complex);
			v.add_member(p->get_type());
			p->net_value_serialize(v);
			e.add_member(v);
		}
	}
	
}

void PropertyContainer::net_deserialize(const clan::NetGameEventValue & e)
{
	uint32_t size = e.get_member(0).to_uinteger();

	for(uint32_t i = 0; i < size; i++)
	{
		const clan::NetGameEventValue & v = e.get_member(1+i);

		uint32_t type = v.get_member(0).to_uinteger();

		IProperty * p = PropertyContainer::create_property(type,std::string());
		p->net_value_deserialize(v);

		clan::Console::write_line("Deserialized property with name='%1', type='%2'",p->get_name(),p->get_type());

		if(this->has_property(p->get_name(), p->get_type()))
		{
			this->get_property(p->get_name())->set(p);
		}
		else
		{
			this->add_property(p);
		}
	}
}