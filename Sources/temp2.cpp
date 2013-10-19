#include "precomp.h"
#include "temp2.h"
file_header createsec()
{
	file_header head1;
	section sec1;

	head1.map_format_version=2;
	head1.sections_count=5;
	head1.tile_count=16;
	head1.sections = new section[head1.sections_count];

	for (int i=0;i<head1.sections_count;i++)
	{
		head1.sections->length=i+1;
		head1.sections->start=i;
		head1.sections->type=i*2;
	}
	return head1;
}

void saveheader(std::string file,file_header head)
{
	clan::File f(file,clan::File::create_always,clan::File::access_write);
	f.write_uint32(head.map_format_version);
	f.write_uint32(head.sections_count);
	f.write_uint32(head.tile_count);
	for (int i=0;i<head.sections_count;i++)
	{
		f.write_uint32(head.sections->length);
		f.write_uint32(head.sections->start);
		f.write_uint32(head.sections->type);
	
	}
	f.close();
}
file_header loadheader(std::string file)
{
	file_header head1;
	clan::File f(file,clan::File::open_existing,clan::File::access_read);
	head1.map_format_version=f.read_uint32();
	head1.sections_count=f.read_uint32();
	head1.tile_count=f.read_uint32();
	head1.sections=new section[head1.sections_count];
	for (int i=0;i<head1.sections_count;i++)
	{
		head1.sections->length=f.read_uint32();
		head1.sections->start=f.read_uint32();
		head1.sections->type=f.read_uint32();
	}
	f.close();
	return head1;
}
void printhead(file_header head)
{
	clan::Console::write_line("op: h=%1", head.map_format_version);
	clan::Console::write_line("op: h=%1", head.sections_count);
	clan::Console::write_line("op: h=%1", head.tile_count);
	for (int i=0;i<head.sections_count;i++)
	{
		clan::Console::write_line("asd: h=%1", head.sections->length);
		clan::Console::write_line("234: h=%1", head.sections->start);
		clan::Console::write_line("oks: h=%1", head.sections->type);
	}
}
