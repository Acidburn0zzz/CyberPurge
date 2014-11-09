#include <wip.hpp>
#include <SDL_image.h>
Rend::Rend(SDL_Window *w){
	r=SDL_CreateRenderer(w, -1, 0);
	return;
}
Rend::~Rend(){
	for(auto i:texcache) SDL_DestroyTexture(i.second);
	texcache.clear();
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
	return;
}
void Rend::finish(){
	SDL_RenderPresent(r);
	return;
}
void Rend::clear(Color c){
	SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
	SDL_RenderClear(r);
	return;
}
void Rend::blit(Recti dst, SDL_Texture *img, SDL_RendererFlip flip){
	SDL_Rect rect;
	rect.x=dst.pos.x;
	rect.y=dst.pos.y;
	rect.w=dst.size.x;
	rect.h=dst.size.y;
	SDL_RenderCopyEx(r, img, nullptr, &rect, 0, nullptr, flip);
	return;
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
SDL_Texture *Rend::getTex(SDL_Surface *img){
	if(!img) return nullptr;
	SDL_Texture *&tex=texcache[img];
	if(!tex){
		tex=SDL_CreateTextureFromSurface(r, img);
	}
	return tex;
}
