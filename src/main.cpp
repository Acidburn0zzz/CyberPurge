#include <wip.hpp>
#include <slideshow.hpp>
#include <game.hpp>
#include <fstream>
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
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, AUDIO_U16LSB, 2, 4096);

	SDL_Window *w=SDL_CreateWindow("WIP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 800, SDL_WINDOW_MAXIMIZED|SDL_WINDOW_RESIZABLE);
	SDL_TimerID rendTimer=SDL_AddTimer(25, &tickRender, reinterpret_cast<void*>(SDL_GetWindowID(w)));
	Ctx *c=new Ctx(w);
	Rend r=Rend(w);
	c->state(new Slideshow([c]()->void{
		std::ifstream fin("game.in");
		Game *g=new Game(fin);
		c->state(g);
	}, {"init0.png","init1.png"}));

	bool fst=true;
	while(true){
		SDL_Event e;
		SDL_WaitEvent(&e);
		c->handle(e);
		if(e.type == SDL_QUIT) break;
		if(e.type == SDL_WINDOWEVENT){
			c->render(&r);
			if(fst){
				c->playSnd(c->getSnd("init.ogg", 32), 1);
				c->getSnd("soundtrack.ogg", 32);
				fst=false;
			}
		}
	}
	SDL_RemoveTimer(rendTimer);
	delete c;
	SDL_DestroyWindow(w);

	return 0;
}
