#include "precomp.h"
#include "iproperty.h"
#include "property.h"
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


	throw clan::Exception("Tried to get non-existing property : " + name);
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
		(*it)->serialize(file);
	}
}

void PropertyContainer::deserialize(clan::File & file)
{
	uint32_t size = file.read_uint32();
	for(uint32_t i = 0; i < size; i++)
	{
		m_props[i]->deserialize(file);
	}
}

void PropertyContainer::net_serialize(clan::NetGameEventValue & e, bool only_changed) const
{
	e.add_member(clan::NetGameEventValue((bool)only_changed));

	if(only_changed)
	{
		uint8_t i = 0;
		for(auto it = m_props.begin(); it != m_props.end(); it++)
		{
			const IProperty * p = (*it);
			if( (!(p->get_flags()&EPF_UNCHANGED)) || p->get_flags()&EPF_ALWAYS_SEND ) i++;
		}

		e.add_member((uint8_t)i);

		for(i = 0; i < m_props.size(); i++)
		{
			const IProperty * p = m_props[i];

			if((!(p->get_flags()&EPF_UNCHANGED)) || p->get_flags()&EPF_ALWAYS_SEND)
			{
				e.add_member((uint8_t)i);
				clan::NetGameEventValue v(clan::NetGameEventValue::complex);
				p->net_value_serialize(v);
				e.add_member(v);
			}
		}
	}
	else
	{
		e.add_member((uint8_t)m_props.size());
		for(auto it = m_props.begin(); it != m_props.end(); it++)
		{
			const IProperty * p = (*it);

			clan::NetGameEventValue v(clan::NetGameEventValue::complex);
			p->net_value_serialize(v);
			e.add_member(v);
		}
	}

}

void PropertyContainer::net_deserialize(const clan::NetGameEventValue & e)
{
	bool only_changed = e.get_member(0).to_boolean();
	uint8_t size = e.get_member(1).to_ucharacter();

	if(only_changed)
	{
		for(uint32_t i = 0; i < size; i+=2)
		{
			uint8_t pos = e.get_member(2+i);
			const clan::NetGameEventValue & v = e.get_member(3+i);
			m_props[pos]->net_value_deserialize(v);
		}
	}
	else
	{
		for(uint32_t i = 0; i < size; i++)
		{
			const clan::NetGameEventValue & v = e.get_member(2+i);
			m_props[i]->net_value_deserialize(v);
		}
	}
}


void PropertyContainer::xml_serialize(clan::DomDocument doc, clan::DomElement e, bool dynamic) const
{
	clan::DomElement epc(doc,"property_containter");

	if(e.is_null())
		doc.append_child(epc);
	else
		e.append_child(epc);

	if(dynamic)
		epc.set_attribute("dynamic","");

	for(auto it = m_props.begin(); it != m_props.end(); it++)
	{
		clan::DomElement prop(doc,"property");
		(*it)->xml_serialize(doc,prop);
		epc.append_child(prop);
	}
}

void PropertyContainer::xml_deserialize(clan::DomElement e)
{
	clan::DomNodeList list = e.get_child_nodes();

	if(e.has_attribute("dynamic"))
	{
		for(uint32_t i = 0; i < (uint32_t)list.get_length(); i++)
		{
			clan::DomElement el = list.item(i).to_element();
			std::string name = el.get_attribute("name");

			for(uint32_t j = 0; j < m_props.size(); j++)
			if(m_props[j]->get_name()==name)
			{
				m_props[j]->xml_deserialize(el);
				break;
			}
		}
	}
	else
	{
		for(uint32_t i = 0; i < (uint32_t)list.get_length(); i++)
		{
			clan::DomElement el = list.item(i).to_element();
			m_props[i]->xml_deserialize(el);
		}
	}
}
