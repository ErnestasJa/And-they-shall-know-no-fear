#pragma once
/**
* PropertyData structure holds the property name, data and property flags.
* this structure is shared between instances of Property<Type>
**/

#include "iproperty.h"
#include "property_util.h"

template <class T>
struct PropertyData
{
	std::string name;
	T data;
	uint32_t flags;

	PropertyData(const std::string & pname, T pdata, uint32_t flags)
	{
		name=pname;
		data=pdata;
		flags=flags;
	}

	PropertyData(const std::string & pname, T pdata)
	{
		name=pname;
		data=pdata;
		flags=0;
	}

	PropertyData(const std::string & pname)
	{
		name=pname;
		flags=EPF_UNCHANGED;
	}
};

template <class T>
class Property: public IProperty
{
public:
	static IProperty * create(const std::string & name);
	static uint32_t get_property_type_id(){return get_type_id<T>();}

protected:
	std::shared_ptr< PropertyData<T> > m_data;

public:
	///constructors
	virtual ~Property(){}

	Property()
	{
	}
	
	Property(const Property& copy)
	: m_data(copy.m_data)
	{
	}

	Property(std::string name, T data)
	{
		m_data = std::shared_ptr< PropertyData<T> >(new PropertyData<T>(name,data));
	}

	Property(std::string name, T data, uint32_t flags)
	{
		m_data = std::shared_ptr< PropertyData<T> >(new PropertyData<T>(name,data,flags));
	}

	Property(std::string name)
	{
		m_data = std::shared_ptr< PropertyData<T> >(new PropertyData<T>(name));
	}

public:
	///methods
	bool is_null() const
	{
		return m_data.get() == nullptr ? true : false;
	}

	inline void set(const T & data)
	{
		if(m_data->data!=data)
		{
			m_data->data = data;
			m_data->flags &= ~EPF_UNCHANGED;
		}
	}

	const T & get()
	{
		return m_data->data;
	}

	void set(IProperty * other)
	{
		if(get_type()!=other->get_type())
			throw clan::Exception("Tried to set data from other property with different type");

		Property<T> * o = static_cast<Property<T> *>(other);

		set(o->m_data->data);
	}

	const std::string & get_name() const
	{
		return m_data->name;
	}

	void set_name(const std::string & name)
	{
		m_data->name = name;
	}

	uint32_t get_type() const
	{
		return get_type_id<T>();
	}

	uint32_t get_flags() const
	{
		return m_data->flags;
	}

public:
	///operator overloads
	Property<T> operator = (const Property<T> & p)
	{
		m_data = p.m_data;
		return *this;
	}

	Property<T> operator = (const T & data)
	{
		set(data);
		return *this;
	}

	operator T() const { return m_data->data; }

///File serilization
public:
	virtual void serialize(clan::File & f) const
	{
		f.write_string_a(m_data->name);
		serialize_type<T>(f,m_data->data);
	}

	virtual void deserialize(clan::File & f)
	{
		m_data->name = f.read_string_a();
		deserialize_type<T>(f,m_data->data);
	}

///Net serilization
public:
	virtual void net_value_serialize(clan::NetGameEventValue & e) const
	{
		e.add_member(m_data->name);
		serialize_type<T>(e,m_data->data);
		m_data->flags|= EPF_UNCHANGED; ///#FIX ME: pasikeitus reiksmei tik pirmas kas serializuos sia savybe matys ja kaip pasikeitusia.
	}

	virtual void net_value_deserialize(const clan::NetGameEventValue & e)
	{
		m_data->name = e.get_member(1).to_string();
		deserialize_type<T>(e.get_member(2),m_data->data);
	}
};

template <class T> 
IProperty * Property<T>::create(const std::string & name){return new Property<T>(name);}