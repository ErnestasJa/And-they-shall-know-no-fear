#include "precomp.h"
#include "test_level_load.h"
struct std::vector<levelinfo> load(const std::string &id, const clan::XMLResourceDocument &doc)
{
	levelinfo li;
	std::vector<levelinfo>levels;

	clan::XMLResourceNode resource = doc.get_resource(id);

	if (resource.get_type() == "levellist") clan::Console::write_line("labas");
	else throw clan::Exception("nera tokio level list");

	clan::DomNode cur_node = resource.get_element().get_first_child();

	while (!cur_node.is_null())
	{

		if (!cur_node.is_element())
			continue; 

		clan::DomElement cur_element = cur_node.to_element();
		std::string tag_name = cur_element.get_tag_name();

		if(tag_name=="level")
		{
			if (cur_element.has_attribute("name")) li.pav=cur_element.get_attribute("name");
			if (cur_element.has_attribute("file")) li.fileinfo=cur_element.get_attribute("file");
			levels.push_back(li);
		}

		cur_node = cur_node.get_next_sibling();
		return levels;
	}
}