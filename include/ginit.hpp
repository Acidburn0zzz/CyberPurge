#pragma once
#include <wip.hpp>
struct GInit: State{
	virtual void enabled();
	virtual void render(Rend *r);	
	virtual void key(KEType type, TS time, SDL_Keysym k);
};
