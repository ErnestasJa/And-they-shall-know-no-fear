#include "precomp.h"
#include "common.h"

Client::Client()
{
	m_id = add_property<uint32_t>("id");
	m_flags = add_property<uint32_t>("flags",0);
	m_name  = add_property<std::string>("name");
}

void Client::clear_flag(uint32_t flag)
{
	m_flags = m_flags & ~flag;
}

bool Client::check_flag(uint32_t flag)
{
	return ((m_flags & flag) == flag);
}

void Client::set_flag(uint32_t flag)
{
	m_flags = m_flags | flag;
}

uint32_t Client::get_id()
{
	return m_id;
}

void Client::set_id(uint32_t id)
{
	m_id = id;
}

std::string Client::get_name()
{
	return m_name;
}

void Client::set_name(const std::string & name)
{
	m_name = name;
}