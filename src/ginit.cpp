#include <ginit.hpp>
void GInit::enabled(){
	return;
}
void GInit::render(Rend *r){
	SDL_Texture *t=r->getTex("init.png");
	double scale=std::min(r->size.x/1280.0, r->size.y/1024.0);
	Vec2i a=(r->size-Vec2i{1280, 1024}*scale)/2;
	r->blit(Recti{a,r->size-2*a}, t);
	return;
}

