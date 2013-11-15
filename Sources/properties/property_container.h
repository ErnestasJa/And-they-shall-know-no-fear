#pragma once

class IProperty;

template <class T>
class Property;

///Every function that may fail will throw clan::Exception
class PropertyContainer
{
///property container
protected:
	std::vector<IProperty*> m_props;

public:
	PropertyContainer();
	virtual ~PropertyContainer();

	void		add_property(IProperty * p);
	bool		has_property(const std::string & name);
	bool		has_property(const std::string & name, uint32_t type);
	IProperty*  get_property(const std::string & name); ///no throw
	IProperty*  get_last_used(); ///no throw
	bool		remove_property(const std::string & name);
	
	template <class T> Property<T> add_property(const Property<T> & prop);
	template <class T> Property<T> add_property(const std::string & name, const T & data);
	template <class T> Property<T> add_property(const std::string & name);
	template <class T> Property<T> get_property(const std::string & name);

public:
	virtual void serialize(clan::File & file) const;
	virtual void deserialize(clan::File & file);

	virtual void net_serialize(clan::NetGameEvent & e, bool only_changed = true) const;
	virtual void net_deserialize(const clan::NetGameEvent & e, uint32_t start_at_argument=1);

///factory methods
private:
	typedef IProperty * (*prop_create_func)(const std::string &);
	static std::map<uint32_t, prop_create_func> m_prop_create;

public:
	template <class T>
	static bool register_property()
	{
		m_prop_create[T::get_property_type_id()]=&T::create;
		return true;
	}

	static IProperty * create_property(uint32_t type, const std::string & name)
	{
		auto it = m_prop_create.find(type);

		if(it != m_prop_create.end())
			return it->second(name);
		
		clan::StringFormat fmt("Property type id=%1 is not valid type id for property creation");
		fmt.set_arg(1,type);

		throw clan::Exception(fmt.get_result());
	}
};

template <class T>
Property<T> PropertyContainer::get_property(const std::string & name)
{
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		if(get_type_id<T>()!=(*it)->get_type())
			throw clan::Exception("Another property already exists with the same name but different type");

		return (*static_cast< Property<T> *>(*it));
	}

	///for now let's just throw exception
	throw clan::Exception("Tried to get non-existing property");
}

template <class T>
Property<T> PropertyContainer::add_property(const Property<T> & prop)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&prop](IProperty * o){return o->get_name()==prop.get_name();});

	if(it!=m_props.end())
	{
		if(prop.get_type()!=(*it)->get_type())
			throw clan::Exception("Another property already exists with the same name but different type");
		
		r = static_cast< Property<T> *>(*it);
		*r =prop; // not sure if data should be replaced or just set
		return *r;
	}

	r = new Property<T>(prop);
	m_props.push_back(r);
	return *r;
}

template <class T>
Property<T> PropertyContainer::add_property(const std::string & name)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		r = static_cast< Property<T> *>(*it);

		if(r->get_type()!=get_type_id<T>())
			throw clan::Exception("Another property already exists with the same name but different type");
		
		return *r;
	}

	r = new Property<T>(name);
	m_props.push_back(r);
	return *r;
}

template <class T>
Property<T> PropertyContainer::add_property(const std::string & name, const T & data)
{
	Property<T> * r;
	auto it = std::find_if(m_props.begin(), m_props.end(), [&name](IProperty * o){return o->get_name()==name;});

	if(it!=m_props.end())
	{
		r = static_cast< Property<T> *>(*it);

		if(r->get_type()!=get_type_id<T>())
			throw clan::Exception("Another property already exists with the same name but different type");
		
		r->set(data); //not sure if data should be replaced or just set
		return *r;
	}

	r = new Property<T>(name,data);
	m_props.push_back(r);
	return *r;
}

