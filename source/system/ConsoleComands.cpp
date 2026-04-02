#include "pch.h"
#include "ConsoleCommands.h"
#include "ConsoleConstants.h"
#include "global.h"
#include <iostream>
#include <iomanip>

Console::Console()
{
}

void Console::init()
{
	lbc_pos_ = { (float)AEGfxGetWinMinX(), -120.f};
	lbc_size = { (float)AEGfxGetWindowWidth() * 0.375f, (float)AEGfxGetWindowHeight() * 0.375f };
	tc_pos_ = { lbc_pos_.x,lbc_pos_.y - lbc_size.y * 0.80f };
	tc_size = { lbc_size.x,lbc_size.y * 0.15f };
	pos_ = { tc_pos_.x ,tc_pos_.y - tc_size.y * 0.7f };
	text_size_ = { 0.5f };

	//============================Log Container================================
	log_box_container = ecs.createEntity();
	ecs.addComponent(log_box_container, Components::Mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER),COLOR, MESH_RECTANGLE_CORNER,9997 });
	ecs.addComponent(log_box_container, Components::Transform{ lbc_pos_,lbc_pos_ ,lbc_size,lbc_size,0.f });
	ecs.addComponent(log_box_container, lbc_color);
	ecs.addComponent(log_box_container, Components::TagClass{ Components::Tag::UI });


	//=============================Text_box======================================
	
	input_text = ecs.createEntity();
	ecs.addComponent(input_text, Components::Text{ "",TF.getFontID() ,9999 });
	ecs.addComponent(input_text, Components::Transform{ pos_,pos_ ,text_size_,text_size_,0.f});
	ecs.addComponent(input_text, Components::Color{ 1.f,1.f,1.f,1.f });
	ecs.addComponent(input_text, Components::TagClass{ Components::Tag::UI });

	//========================Text_box_container==================================
	text_box_container = ecs.createEntity();
	ecs.addComponent(text_box_container, Components::Mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER),COLOR, MESH_RECTANGLE_CORNER,9998 });
	ecs.addComponent(text_box_container, Components::Transform{ tc_pos_,tc_pos_ ,tc_size,tc_size,0.f });
	ecs.addComponent(text_box_container, tc_color);
	ecs.addComponent(text_box_container, Components::TagClass{ Components::Tag::UI });
}

void Console::text_box_update()
{

	Components::Text* text = ecs.getComponent<Components::Text>(input_text);
	for (unsigned char cur = 0x20; cur <= 0x7E; cur++)
	{
		char _fcur = static_cast<char>(cur);
		if (AEInputCheckTriggered(cur) && text->text.size() <= _MAX_CHAR_COUNT)
		{
			if (cur >= AEVK_A && cur <= AEVK_Z)
			{
				if (!(AEInputCheckCurr(AEVK_LSHIFT) || AEInputCheckCurr(AEVK_RSHIFT)))
					_fcur = static_cast<char>(static_cast<int>(cur) + 32);
			}
			text->text += std::string{ _fcur };
			std::cout << text->text << '\n';
		}
	}
	if (!text->text.empty() && AEInputCheckTriggered(AEVK_BACK))
	{
		Components::Text* text = ecs.getComponent<Components::Text>(input_text);
		text->text.pop_back();
		std::cout << text->text << '\n';
	}
	
	if (AEInputCheckTriggered(AEVK_SLASH))
	{
		text->text += std::string{ "/" };
		std::cout << text->text << '\n';
	}

	if (AEInputCheckTriggered(AEVK_RETURN) && !text->text.empty())
	{
		for (size_t i = 0; i < static_cast<size_t>(CommandTypes::CT_COUNT); ++i )
		{
			if (text->text == command_inputs[i])
			{
				ConsoleEvents.template_pool[i].triggered = true;
				break;
			}
		}

		text->text.clear();
	}
}

void Console::update()
{
	if (!this->active && AEInputCheckTriggered(AEVK_RETURN))
	{
		this->init();
		this->active = true;
	}
	if (this->active && AEInputCheckTriggered(AEVK_ESCAPE))
	{
		this->free();
		this->active = false;
	}
	if(this->active){
		text_box_update();
	}
}

void Console::free()
{
	ecs.destroyEntity(log_box_container);
	ecs.destroyEntity(input_text);
	ecs.destroyEntity(text_box_container);
}