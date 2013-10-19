#pragma once
struct section
{
 uint32_t type;
 uint32_t start;
 uint32_t length;
};

struct file_header
{
 uint32_t map_format_version;
 uint32_t tile_count;
 uint32_t sections_count;
 section * sections;
};

file_header createsec();
void saveheader(std::string file,file_header head);
file_header loadheader(std::string file);
void printhead(file_header head);

