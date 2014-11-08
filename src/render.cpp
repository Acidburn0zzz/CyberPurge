#include <game.hpp>
Rect Game::setView(Vec2i screen){
	// (0,0) to (0,w)
	// (c,r) to (?,0)
	double scale=20;
	scale=std::floor(scale);
	vm.mx=Vec2{1,0}*scale;
	vm.my=Vec2{0,1}*scale;
	vm.a=Vec2{-plr.pos.x*scale+screen.x/2.0, -plr.pos.y*scale+screen.y/2.0};
	if(vm.a.x>0) vm.a.x=0;
	if(scale*grid.c+vm.a.x<screen.x) vm.a.x=screen.x-scale*grid.c;
	if(vm.a.y>0) vm.a.y=0;
	if(scale*grid.r+vm.a.y<screen.y) vm.a.y=screen.y-scale*grid.r;
	vm.a.y=screen.y-vm.a.y;
	vm.my=-vm.my;
	vmi=vm.invert();
	Vec2 ll=vmi*Vec2{0,screen.y}, ur=vmi*Vec2{screen.x,0};
	return Rect{ll, ur-ll};
}
Vec2i Game::viewport(Vec2 a){
	Vec2 r=vm*a;
	return Vec2i{static_cast<int>(std::floor(r.x+0.5)), static_cast<int>(std::floor(r.y+0.5))};
}
Recti Game::viewport(Rect a){
	a.pos.y+=a.size.y;
	a.size.y=-a.size.y;
	Vec2 r=vm*a.pos;
	Vec2 s{
		vm.mx _dot_ a.size,
		vm.my _dot_ a.size
	};
	return Recti{
		Vec2i{std::floor(r.x),std::floor(r.y)},
		Vec2i{std::floor(s.x)+1,std::floor(s.y)+1},
	};
}
void Game::render(Rend *r){
	Rect cells=setView(r->size);

	{
		SDL_Texture *bg=r->getTex("ass/background.png");
		Vec2 ll=cells.pos, ur=cells.pos+cells.size;
		Vec2 p=Vec2{bgsize.x*std::floor(ll.x/bgsize.x), 0};
		p.x+=1;
		p.y+=1;
		while(p.x<ur.x){
			Recti br=viewport(p, bgsize);
			br.size.x++;
			if(p.x>=0) r->blit(br, bg);
			p.x+=bgsize.x;
		}
	}

	{
		Recti celli;
		celli.pos.x=std::floor(cells.pos.x);
		celli.pos.y=std::floor(cells.pos.y);
		celli.size.x=std::ceil(cells.size.x+2);
		celli.size.y=std::ceil(cells.size.y+2);

		celli.size+=celli.pos;
		for(int i=celli.pos.y;i<celli.size.y;i++){
			for(int j=celli.pos.x;j<celli.size.x;j++){
				CellInfo *info;
				if(i<0 || j<0 || i>= (int)grid.r || j>=(int)grid.c){
					info=&cellinfo['#'];
					continue;
				}else{
					info=&cellinfo[grid[i][j]];
				}
				SDL_Texture *tex=r->getTex(info->tex);
				Rect p;
				p.pos=Vec2{j,i};
				p.size=Vec2{1,1};
				r->blit(viewport(p), tex);
			}
		}
	}

	render(r, plr);
	for(auto &e:enemies){
		render(r, e);
	}
	if(hasBall){
		render(r, ball);
	}


	return;
}
void Game::render(Rend *r, ORect &o){
	SDL_Texture *t=r->getTex(o.tex);
	r->blit(viewport(o.pos-o.size/2, o.size), t);
	return;
}
void Game::render(Rend *r, OBall &o){
	SDL_Texture *t=r->getTex(o.tex);
	r->blit(viewport(o.pos-Vec2{o.r,o.r}, 2*Vec2{o.r,o.r}), t);
	return;
}

void Game::render(Rend *r, Entity &o){
	render(r, static_cast<ORect&>(o));
	Rect healthbar;
	healthbar.pos=o.pos+Vec2{-o.size.x/2,o.size.y/2+0.3};
	healthbar.size=Vec2{o.size.x, 0.2};
	r->fill(viewport(healthbar), Color{0,0,0,255});
	healthbar.size.x*=o.hp/o.maxhp;
	r->fill(viewport(healthbar), Color{0,255,0,255});
	return;
}
