#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H


#include "AEEngine.h"
#include <array>
#include <string>
#include <functional>

#include "../factory/MeshFactory.h"


#define MAX_CARDS 1000
#define MAX_COMPONENT 20

constexpr size_t NULL_INDEX = -1;
constexpr size_t MAX_HAND = 6;

namespace Components
{
	

	enum DamageType
	{
		SLASHING,
		PIERCING,
		BLUDGEONING,
		FIRE
	};

	struct Text{
		const char* text;
		s8 fontID;
		s8 z;
		Text(const char*, s8, s8);
	};


	struct RGBA
	{
		f32 r;
		f32 g;
		f32 b;
		f32 a;
		RGBA(f32 r, f32 g, f32 b, f32 a);
	};

	struct Timer{
		int seconds;
		int max_seconds;
		bool start;
	};

	struct GridCell{
		s32 x,y;
	};

	struct Switch
	{
		bool s;
	};


	struct Transform
	{
		AEVec2 pos{ 0.f,0.f };
		AEVec2 pos_onscreen{ 0.f,0.f };
		AEVec2 size{ 0,0 };
		AEVec2 size_col{ 0,0 };
		AEMtx33 mtx{};
		float rotation{ 0.f };
	};

	struct Texture
	{
		AEGfxTexture* texture{ nullptr };
	};

	struct Mesh
	{
		bool on;
		AEGfxVertexList* mesh = nullptr;
		RENDER_MODE r_mode;
		MESH_TYPE type;
		s8 z;
	};

	struct Color
	{
		RGBA c_color;
		RGBA p_color;
		Color(f32 r, f32 g, f32 b, f32 a);
	};

	struct Input
	{
		public:
		bool on;
		u8 type; //for AE internal input system check
		bool hover;
		Input(u8 type, bool hover, 
			  std::function<void()> onClick, 
			  std::function<void()> onHover, 
			  std::function<void()> offHover);
		std::function<void()> onClick;
		std::function<void()> onHover;
		std::function<void()> offHover;
	};


	//===========================CARDS=========================================
	struct Attack
	{
		f32 damage;
		DamageType type;
		f32 range;
	};
	struct Name
	{
		std::string value;
	};
	struct HP
	{
		f32 value;
	};
	class Card_Storage
	{
		private:
		size_t next_index = 0;
		public:
		size_t get_nextIndex()
		{
			if (next_index < card_storage.size()) return next_index++;
			else return -1;
		}
		
		size_t& index() { return next_index; }
		size_t size() const {return this->next_index;}

		std::array<size_t, MAX_HAND> card_storage = {NULL_INDEX};
	};

	//===================Turn Based======================================
	struct TurnBasedStats
	{
		bool yielded = false;		// true = no more actions and granted turns until round ends

		int  maxPoints = 0;			// per-round cap (set when entity created)
		int  points = 0;			// current round points (reset at round start)

		bool needsRedraw = false;	// set true at round start; card system consumes it later
	};

}
#endif