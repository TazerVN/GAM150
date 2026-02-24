#pragma once
#include "AEEngine.h"
#include "vector"

namespace TextureFactory
{
	class TextureFactory
	{
		private:
		std::vector<AEGfxTexture*> data;
		s8 fontID;

		public:
		void textureInit();
		TextureFactory();
		~TextureFactory();
		AEGfxTexture* getTexture(s8 id);
		s8& getFontID();
		void addTexture(AEGfxTexture* pTex);
		
	};
}