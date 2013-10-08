#pragma once

struct levelinfo
{
	std::string pav;
	std::string fileinfo;
};

std::vector<levelinfo> load(const std::string &id,const clan::XMLResourceDocument &doc);

