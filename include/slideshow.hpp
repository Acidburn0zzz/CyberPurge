#pragma once
#include <wip.hpp>
#include <functional>
#include <vector>
struct Slideshow: State{
	std::function<void()> f;
	std::vector<SDL_Surface*> texi;
	std::vector<std::string> tex;
	std::size_t at=0;
	Slideshow(std::function<void()> a, std::vector<std::string> b):f(std::move(a)),tex(std::move(b)){
		return;
	}
	virtual void enabled();
	virtual void render(Rend *r);	
	virtual void key(KEType type, TS time, SDL_Keysym k);
};
