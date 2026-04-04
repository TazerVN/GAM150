#include "pch.h"
#include "UIObject.h"
#include <string>

namespace UIO
{

	void button_onHover(Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.2f;



		c->d_color.r = minimum + (1.f - minimum) * lerp;
		c->d_color.b = minimum + (1.f - minimum) * lerp;
		c->d_color.g = minimum + (1.f - minimum) * lerp;
		//t->size.x = t->size_og.x * 1.1f;
		//t->size.y = t->size_og.y * 1.1f;

	}


	void button_offHover(Entity id)
	{
		Components::Color* c = ecs.getComponent<Components::Color>(id);
		Components::Transform* t = ecs.getComponent<Components::Transform>(id);

		Components::Timer* timer = ecs.getComponent<Components::Timer>(id);
		timer->start = true;

		f32 lerp = timer->seconds / (timer->max_seconds / 2.f) >= 1.f ? timer->max_seconds - timer->seconds : timer->seconds;
		f32 minimum = 0.6f;

		c->d_color = c->c_color;
		timer->seconds = 0.5f;

	}
	void textbutton_onHover(UIO::TextureButton* button)
	{

		Components::Timer* timer = ecs.getComponent<Components::Timer>(button->timer);
		Components::Transform* tb = ecs.getComponent<Components::Transform>(button->button);
		Components::Transform* tt = ecs.getComponent<Components::Transform>(button->text);
		Components::Text* text = ecs.getComponent<Components::Text>(button->text);

		timer->start = true;
		f32 lerp = timer->seconds / (timer->max_seconds);

		f32 scale_factor = 0.1f;

		tb->size.x = tb->size_og.x + tb->size_og.x * scale_factor * lerp;
		tb->size.y = tb->size_og.y + tb->size_og.y * scale_factor * lerp;


		f32 text_width, text_height;
		AEGfxGetPrintSize(TF.getFontID(), text->text.c_str(), tt->size_og.x, &text_width, &text_height);
		f32 offset_x = -text_width * tb->size.x;
		tt->size.x = tt->size_og.x + tt->size_og.x * scale_factor * lerp;

		button_onHover(button->button);
		button_onHover(button->text);
	}

	void textbutton_offHover(UIO::TextureButton* button)
	{

		Components::Timer* timer = ecs.getComponent<Components::Timer>(button->timer);
		Components::Transform* tb = ecs.getComponent<Components::Transform>(button->button);
		Components::Transform* tt = ecs.getComponent<Components::Transform>(button->text);

		f32 lerp = 1.f - (timer->seconds / timer->max_seconds);
		/*timer->start = true;*/

		if(tb->size.x > tb->size_og.x && tb->size.y > tb->size_og.y)
		{
			tb->size.x = tb->size_og.x + tb->size_og.x * 0.2f * lerp;
			tb->size.y = tb->size_og.y + tb->size_og.y * 0.2f * lerp;

		}
		if (tt->size.x > tt->size_og.x )
		{

			tt->size.x = tt->size_og.x + tt->size_og.x * 0.2f * lerp;

		}

		button_offHover(button->button);
		button_offHover(button->text);
	}
	
	TextButton::TextButton(f32 x, f32 y, f32 width, f32 height, f32 text_size, f32 rotation, s32 z, std::string a, std::function<void()> func, Components::RGBA rgba)
	{
		this->z = z;
		this->button = ui_button(x, y, width, height, 0, z, func, {1.f - rgba.r, 1.f - rgba.g, 1.f - rgba.b, 1 - rgba.a });
		this->text = UIO::ui_text( x - 64.f * text_size, y - 64.f * text_size / 2.f, text_size, text_size,0, z + 1, a, {1.f, 1.f, 1.f, 1.f} );
		this->on = false;
	}

	void TextureButton::setOpacity(f32 a)
	{
		auto button = ecs.getComponent<Components::Color>(this->button);
		auto text = ecs.getComponent<Components::Color>(this->text);
		button->d_color = a;
		text->d_color = a;
	}

	TextureButton& TextureButton::operator=(const TextureButton& rhs)
	{
		this->button = rhs.button;
		this->text = rhs.text;
		this->z = rhs.z;
		this->timer = rhs.timer;
		this->on = rhs.on;


		Components::Timer timer{ 1.f, 0.f, true, true };
		ecs.addComponent(this->text, timer);
		ecs.addComponent(this->button, timer);

		auto in = ecs.getComponent<Components::Input>(this->button);

		in->onHover = [this]() { textbutton_onHover(this); };
		in->offHover = [this]() { textbutton_offHover(this); };
		return *this;
	}
	TextureButton::TextureButton(const TextureButton& rhs)
	{
		this->button = rhs.button;
		this->text = rhs.text;
		this->z = rhs.z;
		this->timer = rhs.timer;
		this->on = rhs.on;

		Components::Timer timer{ 1.f, 0.f, true, true };
		ecs.addComponent(this->text, timer);
		ecs.addComponent(this->button, timer);

		auto in = ecs.getComponent<Components::Input>(this->button);

		in->onHover = [this]() { textbutton_onHover(this); };
		in->offHover = [this]() { textbutton_offHover(this); };
	}

	TextureButton::TextureButton(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 text_size, f32 rotation, s32 z, std::string a, std::function<void()> func, Components::RGBA rgba) : TextButton()
	{
		f32 text_width, text_height;

		AEGfxGetPrintSize(TF.getFontID(), a.c_str(), text_size, &text_width, &text_height);
		f32 offset_x = -text_width * width;
		f32 offset_y = -height * text_height - 5.f;

		this->on = false;
		this->z = z;
		this->button = ui_button_texture(texture, x, y, width, height, 0, z, func);
		this->text = UIO::ui_text( x + offset_x, y + offset_y, text_size, text_size ,0, z + 1, a, rgba);
		//========= timer ==============
		this->timer = ecs.createEntity(); 
		Components::Timer timer{ 0.2f, 0.f, false, false };
		ecs.addComponent(this->timer, timer);

	}


	void TextButton::free()
	{
		if(this->button != 0) ecs.destroyEntity(this->button);
		if(this->text != 0) ecs.destroyEntity(this->text);
		this->button = 0;
		this->text = 0;
	}

	void TextureButton::free()
	{
		if (this->button != 0) ecs.destroyEntity(this->button);
		if (this->text != 0) ecs.destroyEntity(this->text);
		this->button = 0;
		this->text = 0;
	}

	TextShadow& TextShadow::operator=(TextShadow const& rhs)
	{
		this->free();
		this->text = rhs.text;
		this->text_shadow = rhs.text_shadow;
		this->z = rhs.z;
		return *this;
	}
	TextShadow& TextShadow::operator+=(const char* rhs)
	{

		auto text = ecs.getComponent<Components::Text>(this->text);
		auto text_shadow = ecs.getComponent<Components::Text>(this->text_shadow);
		text->text += rhs;
		text_shadow->text += rhs;



		return *this;
	}

	bool TextShadow::newline() const
	{
		auto text = ecs.getComponent<Components::Text>(this->text);
		return *(text->text.end()--) == '\n';
	}

	TextShadow::TextShadow(f32 x, f32 y, f32 text_size, s32 z, std::string a, Components::RGBA rgba)
	{
		this->z = z;
		this->text = ui_interactive_text(x, y, text_size, text_size, 0, z + 1, a, rgba);
		this->text_shadow = ui_text(x + 5.f, y - 5.f, text_size, text_size, 0, z, a, { 0.f, 0.f, 0.f, 1.f });
	}

	void TextShadow::free()
	{
		if(this->text != 0) ecs.destroyEntity(this->text);
		if(this->text_shadow != 0) ecs.destroyEntity(this->text_shadow);
		this->text = 0;
		this->text_shadow = 0;
	}
	
	ScreenTransition& ScreenTransition::operator=(ScreenTransition const& rhs)
	{
		this->free();

		this->fadeIn = rhs.fadeIn;
		this->finished = rhs.finished;
		this->max = rhs.max;
		this->min = rhs.min;
		this->z = rhs.z;
		this->dim = rhs.dim;
		return *this;
	}

	ScreenTransition::ScreenTransition(bool fadeIn, f32 min, f32 max, f32 duration) : z{1300}
	{
		this->fadeIn = fadeIn;
		this->finished = false;
		this->min = min;
		this->max = max;
		if(fadeIn){
			this->dim = ui_blank_solid_center(0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, this->z, 0.f, 0.f, 0.f, 1.f);
			Components::Timer timer{ duration, 0.f, true, false};
			Components::TagClass tag{Components::Tag::UI};
			ecs.addComponent(this->dim, timer);
			ecs.addComponent(this->dim, tag);
		}
		else
		{
			this->dim = ui_blank_solid_center(0, 0, AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, this->z, 0.f, 0.f, 0.f, 0.f);
			Components::Timer timer{ duration, 0.f, true, false };
			Components::TagClass tag{ Components::Tag::UI };
			ecs.addComponent(this->dim, timer);
			ecs.addComponent(this->dim, tag);
		}
		
	}
	bool ScreenTransition::update()
	{
		if(this->dim != 0 && !this->finished){
			auto timer = ecs.getComponent<Components::Timer>(this->dim);
			auto color = ecs.getComponent<Components::Color>(this->dim);
			f32 lerp = timer->seconds/timer->max_seconds;

			if(timer->seconds >= timer->max_seconds)
			{
				this->finished = true;
				return false;
			}

			if(this->fadeIn)
			{
				color->d_color.a = AEClamp( 1.f - lerp, min, max);
			}
			else
			{
				color->d_color.a = AEClamp(lerp, min, max);
			}
			return true;
		}
		return false;
	}

	void ScreenTransition::free()
	{
		if(this->dim != 0) ecs.destroyEntity(this->dim);
		this->dim = 0;
	}

	void slider_onDrag(Entity button, Entity container, Entity fill)
	{

		s32 mousex, mousey;

		AEInputGetCursorPosition(&mousex, &mousey);

		mousex = mousex - f32(AEGfxGetWindowWidth()) * 0.5f;
		mousey = -mousey + f32(AEGfxGetWindowHeight()) * 0.5f;

		mousex = AEClamp(mousex, AEGfxGetWinMinX(), AEGfxGetWinMaxX());
		mousey = AEClamp(mousey, AEGfxGetWinMinY(), AEGfxGetWinMaxY());


		auto pos_button = ecs.getComponent<Components::Transform>(button);
		auto pos_container = ecs.getComponent<Components::Transform>(container);
		auto pos_fill = ecs.getComponent<Components::Transform>(fill);

		pos_button->pos.x = AEClamp(mousex, pos_container->pos.x,pos_container->pos.x + pos_container->size.x);
		pos_button->pos_onscreen.x = pos_button->pos.x;

		pos_fill->size.x = (pos_button->pos.x - pos_container->pos.x)/pos_container->size.x * pos_container->size.x;


	}

	void Slider::update()
	{
		auto pos_button = ecs.getComponent<Components::Transform>(button);
		auto pos_container = ecs.getComponent<Components::Transform>(blank);
		this->current = (pos_button->pos_onscreen.x - pos_container->pos.x)/ pos_container->size.x;
	}

	Slider::Slider(f32 x, f32 y, f32 width, f32 height, s32 z, std::function<void()> func, f32 current, f32 max)
	{
		this->current = current;
		this->max = max;
		this->blank = ui_blank_solid_corner(x, y, width, height, 0, z, 0.1f, 0.f, 0.f, 1.f);
		this->fill = ui_blank_solid_corner(x, y, current * width, height, 0, z + 1, 0.290f, 0.640f, 0.710f, 1.f);

		f32 button_x = x + current/max * width;

		this->button = ui_button(button_x, y - height / 2, 50.f, 100.f, 0, z + 2, nullptr, { 1.f, 1.f, 1.f, 1.f });

		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(this->blank, tag);
		ecs.addComponent(this->fill, tag);
		ecs.addComponent(this->button, tag);

		Entity button = this->button;
		Entity container = this->blank;
		Entity fill = this->fill;

		auto button_in = ecs.getComponent<Components::Input>(this->button);
		button_in->onDrag = [button, container, fill]() { slider_onDrag(button, container, fill); };
		button_in->drag = true;
	}

	void Slider::free()
	{
		if(this->blank != 0) ecs.destroyEntity(this->blank);
		if(this->fill != 0) ecs.destroyEntity(this->fill);
		if(this->button != 0) ecs.destroyEntity(this->button);
		this->blank = 0;
		this->fill = 0;
		this->button = 0;
	}

	Entity ui_timer(f32 max, f32 min)
	{
		Entity id = ecs.createEntity();
		Components::Timer timer{ max, min, true, true };
		ecs.addComponent(id, timer);
		return id;
	}


	Entity ui_hp_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)

	{

		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.5f, 1.0f, 0.5f ,1.0f };
		Components::HP hp{ 100 };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, hp);

		return id;
	}

	Entity ui_stamina_bar(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},{}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ 0.0f, 0.8f, 1.0f ,1.0f };
		Components::TurnBasedStats stats{ 0,0,0, 100, 100, 100 };
		ecs.addComponent(id, stats);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}


	Entity ui_button(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::function<void()> func, Components::RGBA rgba)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ rgba.r, rgba.g, rgba.b, rgba.a };
		Components::Input in(AEVK_LBUTTON, true, func, [id] {button_onHover(id); }, [id] { button_offHover(id); }, z);	//add input system for grid
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		Components::Timer timer{ 1.f, 0.f, true, true };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, timer);


		return id;
	}

	Entity ui_button_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::function<void()> func)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Texture tex{ texture };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Input in(AEVK_LBUTTON, true, func, [id] { button_onHover(id); }, [id] { button_offHover(id); }, z);	//add input system for grid
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		Components::Timer timer{ 1.f, 0.5f, true, true };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, tex);
		ecs.addComponent(id, color);
		ecs.addComponent(id, in);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, timer);

		return id;
	}

	Entity ui_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::string a, Components::RGBA rgba)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{ a, TF.getFontID(), z };
		Components::Color color{ rgba.r, rgba.g, rgba.b, rgba.a };
		Components::TagClass tag{ Components::Tag::UI };	
		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);


		return id;
	}

	Entity ui_interactive_text(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, std::string a, Components::RGBA rgba)

	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height},0.0f };
		Components::Text text{ a, TF.getFontID(), z };
		Components::Color color{ rgba.r, rgba.g, rgba.b, rgba.a };
		Components::TagClass tag{ Components::Tag::UI };	//add input system for grid
		Components::Input in(AEVK_LBUTTON, true, nullptr, [id] { button_onHover(id); }, [id] { button_offHover(id); }, 10);	//add input system for grid
		Components::Timer timer{ 1.f, 0.5f, true, true };

		ecs.addComponent(id, trans);
		ecs.addComponent(id, text);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tag);
		ecs.addComponent(id, in);
		ecs.addComponent(id, timer);


		return id;
	}

	Entity ui_anchor(f32 x, f32 y, f32 width, f32 height, f32 rotation)
	{
		Entity id = ecs.createEntity();
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);


		return id;
	}

	Entity ui_blank_solid_corner(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 a)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CORNER), COLOR, MESH_RECTANGLE_CORNER, z };
		Components::Color color{ r, g, b, a };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity ui_blank_solid_center(f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z, f32 r, f32 g, f32 b, f32 a)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), COLOR, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ r, g, b, a };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);

		return id;
	}

	Entity ui_blank_texture(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture tex{ texture };
		Components::TagClass tag{ Components::Tag::UI };
		ecs.addComponent(id, tag);
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tex);

		return id;
	}
	Entity ui_blank_texture_world(AEGfxTexture* texture, f32 x, f32 y, f32 width, f32 height, f32 rotation, s32 z)
	{
		Entity id = ecs.createEntity();
		//default player values
		Components::Transform trans{ {x,y}, {x,y} ,{width, height} , {width, height}, {}, rotation };
		Components::Mesh mesh{ true, mf.MeshGet(MESH_RECTANGLE_CENTER), TEXTURE, MESH_RECTANGLE_CENTER, z };
		Components::Color color{ 1.0f, 1.0f, 1.0f ,1.0f };
		Components::Texture tex{ texture };
		ecs.addComponent(id, trans);
		ecs.addComponent(id, mesh);
		ecs.addComponent(id, color);
		ecs.addComponent(id, tex);

		return id;
	}


}