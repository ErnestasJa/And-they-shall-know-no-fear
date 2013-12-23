#include "precomp.h"

class SelectedSprites
{
private:
	int32_t ** selected_sprites, x,y;
	
public:

	SelectedSprites(clan::Rect frame)
	{
		if(frame.get_width() > TILE_SIZE || frame.get_height() > TILE_SIZE)
		{
			x=abs(frame.top-frame.bottom)/TILE_SIZE, 
			y=abs(frame.left-frame.right)/TILE_SIZE;

			selected_sprites = new int32_t*[sizeof(int32_t)*x];
			for(int i=0; i<x; i++)
			{
				selected_sprites[i] = new int32_t[sizeof(int32_t)*y];
				for(int j=0; j<y; j++)
				{
					selected_sprites[i][j]=((frame.get_top_left().y+j) * TILE_SIZE + frame.get_top_left().x)/TILE_SIZE + i*TILE_SIZE;
				}
			}
		}
		else
		{
			x=1; y=1;
			selected_sprites = new int32_t*[sizeof(int32_t)];
			selected_sprites[0] = new int32_t[sizeof(int32_t)];
			selected_sprites[0][0] = (frame.get_top_left().y * TILE_SIZE + frame.get_top_left().x)/TILE_SIZE;
		}
	}

	void print()
	{
		clan::Console::write_line("%1,%2",x,y);
		for(int i=0; i<x; i++)
		{
			for(int j=0; j<y; j++)
				clan::Console::write("%1 ",selected_sprites[i][j]); //DEBUG
			clan::Console::write_line(" ");
		}
	}

	int32_t& at(int x, int y)
	{
		return selected_sprites[x][y];
	}

	int32_t ** get_sprites(){return selected_sprites;}
	int32_t get_x(){return x;}
	int32_t get_y(){return y;}
	
};