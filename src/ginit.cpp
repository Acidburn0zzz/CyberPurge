#include <ginit.hpp>
#include <game.hpp>
#include <fstream>
void GInit::enabled(){
	return;
}
void GInit::key(KEType ev, TS t, SDL_Keysym k){
	std::ifstream fin("game.in");
	std::cout<<"Start read"<<std::endl;
	Game *g=new Game(fin);
	std::cout<<"End read"<<std::endl;
	ctx->state(g);
	return;
}
void GInit::render(Rend *r){
	SDL_Texture *t=r->getTex("ass/init.png");
	double scale=std::max(r->size.x/1280.0, r->size.y/1024.0);
	Vec2i a=(r->size-Vec2i(Vec2{1280, 1024}*scale))/2;
	r->blit(Recti{a,r->size-2*a}, t);
	return;
}

