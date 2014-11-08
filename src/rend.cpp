#include <wip.hpp>
Rend::Rend(SDL_Window *w){
	r=SDL_CreateRenderer(w, -1, 0);
	return;
}
Rend::~Rend(){
	if(r) SDL_DestroyRenderer(r);
	r=nullptr;
	return;
}
Rend &Rend::operator=(Rend &&a){
	if(&a==this) return *this;
	if(r) SDL_DestroyRenderer(r);
	r=a.r;
	a.r=nullptr;
	return *this;
}
void Rend::start(){
	SDL_RenderSetViewport(r, nullptr);
	SDL_Rect re;
	SDL_RenderGetViewport(r, &re);
	size.x=re.w;
	size.y=re.h;
	std::cout<<size.x<<' '<<size.y<<std::endl;
	return;
}
void Rend::finish(){
	SDL_RenderPresent(r);
	return;
}
void Rend::clear(Color c){
	SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
	SDL_RenderClear(r);
}
void Rend::blit(Recti dst, SDL_Texture *img){
	SDL_Rect rect;
	rect.x=dst.pos.x;
	rect.y=dst.pos.y;
	rect.w=dst.size.x;
	rect.h=dst.size.y;
	SDL_RenderCopy(r, img, nullptr, &rect);
}
void Rend::fill(Recti dst, Color c){
	SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
	SDL_Rect rect;
	rect.x=dst.pos.x;
	rect.y=dst.pos.y;
	rect.w=dst.size.x;
	rect.h=dst.size.y;
	SDL_RenderFillRect(r, &rect);
	return;
}
