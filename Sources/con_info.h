#ifndef CON_INFO_H
#define CON_INFO_H

#include "properties/property.h"
#include "properties/property_container.h"

class ConnectionInfo: public PropertyContainer
{
public:
	Property<std::string> ip, port, pass, name;

	ConnectionInfo()
	{
		ip   = add_property<std::string>("ip");
		port = add_property<std::string>("port");
		pass = add_property<std::string>("pass");
		name = add_property<std::string>("name");
	}
};

#endif // CON_INFO_H
