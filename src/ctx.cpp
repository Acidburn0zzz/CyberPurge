#include <wip.hpp>
static Uint32 doTimerTick(Uint32 delay, void *arg);
Ctx::Ctx(SDL_Window *w){
	cur=nullptr;
	id=SDL_GetWindowID(w);
	return;
}
void Ctx::clear(){
	if(cur){
		delete cur;
		cur=nullptr;
		_tRemove();
	}
	return;
}
Ctx::~Ctx(){
	clear();
	return;
}
void Ctx::state(State *s){
	if(cur){
		delete cur;
		cur=nullptr;
	}
	cur=s;
	cur->ctx=this;
	cur->enabled();
	return;
}
void Ctx::handle(SDL_Event ev){
	switch(ev.type){
		case SDL_USEREVENT:
			if(ev.user.code==0){
				if(cur) cur->tick();
			}
			break;
		case SDL_KEYUP:
			if(cur) cur->key(KEType::Up, ev.key.timestamp, ev.key.keysym);
			break;
		case SDL_KEYDOWN:
			if(cur) cur->key(KEType::Down, ev.key.timestamp, ev.key.keysym);
			break;
		case SDL_QUIT:
			if(cur){
				cur->quit();
				clear();
			}
			break;
	}
	return;
}
void Ctx::render(Rend *r){
	r->start();
	r->clear(Color{0,0,0,255});
	if(cur) cur->render(r);
	r->finish();
	return;
}
static Uint32 doTimerTick(Uint32 delay, void *arg){
	Ctx *c=static_cast<Ctx*>(arg);
	SDL_Event event;
	event.type=SDL_USEREVENT;
	event.user.code=0;
	event.user.data1=c;
	return delay;
}
void Ctx::_tRemove(){
	if(timerTick){
		SDL_RemoveTimer(timerTick);
		timerTick=0;
	}
	return;
}
void Ctx::ttick(TS t){
	_tRemove();
	timerTick=SDL_AddTimer(t, &doTimerTick, this);
	return;
}
void Ctx::_timer(){
	return;
}
TS Ctx::now(){
	return SDL_GetTicks();
}
void State::enabled(){}
void State::key(KEType type, TS time, SDL_Keysym k){}
void State::tick(){}
void State::render(Rend *r){}
void State::quit(){}
State::~State(){}
