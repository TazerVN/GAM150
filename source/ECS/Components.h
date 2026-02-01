#pragma once

#ifndef COMPONENT_H
#define COMPONENT_H


#include "AEEngine.h"
#include <array>
#include <string>

#include "../rendering/Mesh_factory.h"

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

	struct color
	{
		f32 r;
		f32 g;
		f32 b;
		f32 a;
	};


	struct Transform
	{
		AEVec2 pos{ 0.f,0.f };
		AEVec2 pos_onscreen{ 0.f,0.f };
		AEVec2 size{ 0,0 };
		AEMtx33 mtx{};
		float rotation{ 0.f };
	};

	struct Texture
	{
		AEGfxTexture* texture{ nullptr };
	};

	struct Mesh
	{
		AEGfxVertexList* mesh = nullptr;
		RENDER_MODE r_mode;
		MESH_TYPE type;
		s8 z;
	};

	struct Color
	{
		color c_color;
		color p_color;
	};

	struct Input
	{
		u8 type; //for AE internal input system check
		bool hover;
		void (*fptype)();
		void (*fphover)();
	};


	//===========================CARDS=========================================
	struct Attack
	{
		f32 damage;
		DamageType type;
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
		std::array<size_t, MAX_HAND> card_storage = {};
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