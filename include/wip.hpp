#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <cstdint>
#include <iostream>
#include <vector>
enum class KEType{
	Up,Down
};
using TS=Uint32;
struct State;
struct Vec2{
	double x, y;
};
struct Rect{
	Vec2 pos, size;
};
struct Vec2i{
	int x, y;
};
struct Recti{
	Vec2i pos, size;
};
struct Color{
	std::uint8_t r,g,b,a;
};
struct Rend{
	SDL_Renderer *r;
	Vec2i size;
	Rend():r(nullptr){}
	Rend(SDL_Window *w);
	~Rend();
	Rend(const Rend&)=delete;
	inline Rend(Rend &&a):r(a.r){
		a.r=nullptr;
	}
	Rend &operator=(const Rend &a)=delete;
	Rend &operator=(Rend &&a);
	void start();
	void finish();
	void clear(Color c);
	void blit(Recti dst, SDL_Texture *img);
	void fill(Recti dst, Color c);
};
struct Ctx{
	State *cur;
	SDL_TimerID timerTick=0, timerRend=0;
	Uint32 id;

	Ctx(SDL_Window*);
	~Ctx();
	void state(State *s);
	void handle(SDL_Event ev);
	void render(Rend *r);
	void ttick(TS time);
	void _tRemove();
	void _timer();
	void clear();
	TS now();
};
struct State{
	Ctx *ctx;
	virtual void enabled();
	virtual void key(KEType type, TS time, SDL_Keysym k);
	virtual void tick();
	virtual void render(Rend *r);
	virtual void quit();
	virtual ~State();
};
