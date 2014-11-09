#include <game.hpp>
#include <algorithm>
Rect Game::setView(Vec2i screen){
	// (0,0) to (0,w)
	// (c,r) to (?,0)
	double scale=70;
	//scale=std::floor(scale);
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
	visible=cells;

	{
		double off=bgsize.x/2.71828, rshift;
		SDL_Texture *bg=r->getTex(imgBG);
		Vec2 ll=cells.pos, ur=cells.pos+cells.size;
		Vec2 p1=Vec2{bgsize.x*std::floor((ll.x-bgshift.x)/bgsize.x), bgsize.y*(rshift=std::floor((ll.y-bgshift.y)/bgsize.y))};
		p1.x-=rshift*off;
		p1+=bgshift;
		while(p1.y<ur.y){
			while(p1.x+bgsize.x<ll.x) p1.x+=bgsize.x;
			Vec2 p=p1;
			while(p.x<ur.x){
				Recti br=viewport(p, bgsize);
				r->blit(br, bg);
				p.x+=bgsize.x;
			}
			p1.y+=bgsize.y;
			p1.x-=off;
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
					continue;
				}
				info=&cellinfo[grid[i][j]];
				if(!info->tex) continue;
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

	for(auto &i:animations){
		render(r, i);
	}
	animations.erase(
		std::remove_if(animations.begin(), animations.end(), [](const Animation &a)->bool{
			return a.q.empty();
		}),
		animations.end()
	);

	return;
}
void Game::render(Rend *r, Object &o){
	SDL_Texture *t=r->getTex(o.texs[o.stage].first);
	Vec2 size=o.texs[o.stage].second;
	r->blit(viewport(o.pos-size/2, size), t, (o.direction!=o.dirflip)?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
	return;
}

void Game::render(Rend *r, Entity &o){
	if(o.forceStageUntil>ctx->now()){
		o.stage=o.forceStage;
	}else if(o.jumpStage+1 && !o.onGround){
		o.stage=o.jumpStage;
	}else if(o.vel.len()<0.1){
		o.stage=0;
	}else{
		int d=o.pos.x/o.walkLen;
		o.stage=d%o.walkStages;
	}
	render(r, static_cast<ORect&>(o));
	Rect healthbar;
	healthbar.pos=o.pos+Vec2{-o.size.x/2,o.size.y/2+0.3};
	healthbar.size=Vec2{o.size.x, 0.2};
	r->fill(viewport(healthbar), Color{0,0,0,255});
	healthbar.size.x*=o.hp/o.maxhp;
	r->fill(viewport(healthbar), Color{255,0,0,255});
	return;
}
void Game::render(Rend *r, Animation &a){
	TS now=ctx->now();
	if(now>a.until){
		a.q.pop();
		if(a.q.empty()) return;
		a.until=now+a.q.front().time;
	}
	Rect pos{
		a.pos-a.q.front().size/2,
		a.q.front().size
	};
	r->blit(viewport(pos), r->getTex(a.q.front().img), a.direction?SDL_FLIP_HORIZONTAL:SDL_FLIP_VERTICAL);
	return;
}
