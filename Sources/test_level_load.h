#pragma once

struct levelinfo
{
	std::string pav;
	std::string fileinfo;
};

struct std::vector<levelinfo> load(const std::string &id,const clan::XMLResourceDocument &doc);

