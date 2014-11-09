#include <slideshow.hpp>
void Slideshow::enabled(){
	for(auto &i:tex) texi.push_back(ctx->getImg(i));
	return;
}
void Slideshow::key(KEType ev, TS t, SDL_Keysym k){
	if(ev!=KEType::Down) return;
	at++;
	if(at==tex.size()){
		at=0;
		f();
	}
	return;
}
void Slideshow::render(Rend *r){
	SDL_Texture *t=r->getTex(texi[at]);
	int a, b;
	SDL_QueryTexture(t, nullptr, nullptr, &a, &b);
	Vec2 size{a,b};
	double scale=std::min(r->size.x/size.x, r->size.y/size.y);
	Vec2i off=(r->size-Vec2i(size*scale))/2;
	r->blit(Recti{off,r->size-2*off}, t);
	return;
}

