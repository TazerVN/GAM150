#pragma once
#include "AEEngine.h"
#include <iostream>
#include <string.h>
#include <vector>

#define MAX_TEXT 1000
#define MAX_WC 2048

typedef struct {
	s8 id;
	s8 pFont; 
	char content[MAX_WC];
	f32 x, y;
	f32 w, h;
}Text;

class Text_Manager {
	public:
		Text_Manager(){
			for (int i = 0; i < MAX_TEXT; ++i) {
				id[i] = -1;
				contents[i].clear();
				x[i] = y[i] = w[i] = h[i] = 0.f;
			}
		}
		// find a free slot from the system
		int Find_Free_Slot() const{
			for(int i = 0; i < MAX_TEXT; ++i)
		}
	private:
		s8 id[MAX_TEXT];
		std::string contents[MAX_TEXT];
		f32 x[MAX_TEXT];
		f32 y[MAX_TEXT];
		f32 w[MAX_TEXT];
		f32 h[MAX_TEXT];
};