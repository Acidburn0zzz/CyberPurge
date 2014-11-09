#ifndef _INCLUDED_wip_hpp
#define _INCLUDED_wip_hpp
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <vec.hpp>
enum class KEType{
	Up,Down
};
using TS=Uint32;
struct State;
using Vec2=Vec2t<double>;
using Mat2=Mat2t<double>;
using Rect=Rectt<double>;
using Vec2i=Vec2t<int>;
using Recti=Rectt<int>;
struct Color{
	std::uint8_t r,g,b,a;
};
struct Rend{
	SDL_Renderer *r;
	Vec2i size;
	std::unordered_map<SDL_Surface*, SDL_Texture*> texcache;

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
	void blit(Recti dst, SDL_Texture *img, SDL_RendererFlip flip=SDL_FLIP_NONE);
	void fill(Recti dst, Color c);
	SDL_Texture *getTex(SDL_Surface *srf);
};
struct Ctx{
	State *cur;
	SDL_TimerID timerTick=0, timerRend=0;
	Uint32 id;
	std::unordered_map<std::string, SDL_Surface*> imgcache;
	std::unordered_map<std::string, Mix_Chunk*> sndcache;

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
	SDL_Surface *getImg(const std::string &fn);
	Mix_Chunk *getSnd(const std::string &fn, int vol=128);
	int playSnd(Mix_Chunk *snd, int loops=0, int time=-1);
	void stopSnd(int id=-1);
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
#endif
