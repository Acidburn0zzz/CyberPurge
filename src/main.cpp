#include <wip.hpp>
#include <ginit.hpp>
static TS tickRender(TS d, void *arg){
	SDL_Event ev;
	ev.type=SDL_WINDOWEVENT;
	ev.window.windowID=reinterpret_cast<std::size_t>(arg);
	ev.window.event=SDL_WINDOWEVENT_EXPOSED;
	SDL_PushEvent(&ev);
	return d;
}
int main(){
	SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_EVENTS);
	IMG_Init(IMG_INIT_PNG);
	SDL_Window *w=SDL_CreateWindow("WIP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 800, SDL_WINDOW_MAXIMIZED|SDL_WINDOW_RESIZABLE);
	SDL_TimerID rendTimer=SDL_AddTimer(20, &tickRender, reinterpret_cast<void*>(SDL_GetWindowID(w)));
	Ctx *c=new Ctx(w);
	Rend r=Rend(w);
	c->state(new GInit);
	while(true){
		SDL_Event e;
		SDL_WaitEvent(&e);
		c->handle(e);
		if(e.type == SDL_QUIT) break;
		if(e.type == SDL_WINDOWEVENT){
			c->render(&r);
		}
	}
	SDL_RemoveTimer(rendTimer);
	delete c;
	SDL_DestroyWindow(w);

	return 0;
}
