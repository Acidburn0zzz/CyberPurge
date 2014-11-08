#include <wip.hpp>
#include <ginit.hpp>
int main(){
	SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_EVENTS);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window *w=SDL_CreateWindow("WIP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 800, SDL_WINDOW_MAXIMIZED|SDL_WINDOW_RESIZABLE);
	Ctx *c=new Ctx(w);
	Rend r=Rend(w);
	c->state(new GInit);
	while(true){
		SDL_Event e;
		SDL_PollEvent(&e);
		c->handle(e);
		if(e.type == SDL_QUIT) break;
		if(e.type == SDL_WINDOWEVENT){
			c->render(&r);
		}
	}
	delete c;
	SDL_DestroyWindow(w);

	return 0;
}
