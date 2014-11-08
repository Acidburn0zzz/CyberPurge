#include <game.hpp>

Game::~Game(){
	return;
}
Game::Game(std::istream &fin){
	std::size_t r, c;
	fin>>r>>c;
	grid=Array2d<Cell>(r, c);
	for(std::size_t i=0;i<r;i++){
		for(std::size_t j=0;j<c;j++){
			char c;
			fin>>c;
			grid[i][j]=c;
		}
	}
	fin>>bgsize.x>>bgsize.y;

	plr.pos=Vec2{1.375,2};
	plr.vel=Vec2{0.1,0};
	plr.mass=60;
	plr.drag=0.5;
	plr.size=Vec2{0.75,2};
	plr.gndAcc=10;
	plr.airAcc=7;
	plr.jump=6;

	ball.pos=Vec2{-100,-100};
	ball.vel=Vec2{0,0};
	ball.r=1;
	ball.mass=1;
	ball.r=1;
	ball.drag=1;
	ticklen=0.01;

	drAir=1;
	gravity=Vec2{0,-9.81};

	cellinfo['#']=CellInfo{
		1,1,1,1,
		10,30,true,
		"ass/cell/wall.png"
	};
	cellinfo['.']=CellInfo{
		0,0,0,0,
		0,0,false,
		"ass/cell/air.png"
	};
	return;
}
void Game::tBase(Object &o, double drag){
	o.vel+=gravity*ticklen;
	o.pos+=o.vel*ticklen;
	double dragc=(1.0-o.vel.len2()*o.drag*drag/o.mass*ticklen);
	if(dragc<=0) dragc=0.01;
	o.vel*=dragc;
	return;
}
void Game::tGround(ORect &o, bool dir){
	if(dir==0){
		if(o.vel.y>0) return;
	}else{
		if(o.vel.y<0) return;
	}
	double y=o.pos.y;
	if(dir==0) y-=o.size.y/2;
	else y+=o.size.y/2;
	double minx=o.pos.x-o.size.x/2+0.1;
	double maxx=o.pos.x+o.size.x/2;
	int maxxi=maxx+0.9;
	int yi=y;
	for(int i=minx;i<maxxi;i++){
		CellInfo &c=cellinfo[grid[yi][i]];
		if(dir==0?c.solidU:c.solidD){
			o.vel.y=0;
			if(dir==0) o.onGround=true;
			if(dir==0) o.pos.y=yi+1+o.size.y/2;
			else o.pos.y=yi-o.size.y/2;
			break;
		}
	}
	return;
}
void Game::tWall(ORect &o, bool dir){
	if(dir){
		if(o.vel.x>0) return;
	}else{
		if(o.vel.x<0) return;
	}
	double x=o.pos.x;
	if(dir==0) x+=o.size.x/2;
	else x-=o.size.x/2;
	double miny=o.pos.y-o.size.y/2+0.1;
	double maxy=o.pos.y+o.size.y/2;
	int maxyi=maxy+0.9;
	int xi=x;
	for(int i=miny;i<maxyi;i++){
		CellInfo &c=cellinfo[grid[i][xi]];
		if(dir==0?c.solidL:c.solidR){
			o.vel.x=0;
			if(dir==0) o.pos.x=xi-o.size.x/2;
			else o.pos.x=xi+1+o.size.x/2;
			break;
		}
	}
	return;
}
void Game::tReflect(OBall &o){
	return;
}
void Game::tick(Boulder &o){
	tBase(o, 0);
	tReflect(o);
	return;
}
void Game::tick(Enemy &e){
	tBase(e, drAir);
	e.onGround=false;
	tGround(e, 1);
	tGround(e, 0);
	tWall(e, 0);
	tWall(e, 1);
	return;
}
void Game::tick(Player &p){
	double acc=p.onGround?p.gndAcc:p.airAcc;
	if(p.ctlR){
		p.vel+=Vec2{acc*ticklen,0};
	}
	if(p.ctlL){
		p.vel+=Vec2{-acc*ticklen,0};
	}
	if(p.ctlSpace){
		if(p.onGround){
			p.vel+=Vec2{0,p.jump};
			p.dj=3;
		}else if(p.dj){
			p.vel+=Vec2{0,p.jump};
			p.dj--;
		}
		p.ctlSpace=0;
	}
	double dr;
	double crf=std::floor(p.pos.y-p.size.y/2-0.01), ccf=std::floor(p.pos.x);
	if(crf<0 || ccf<0){
		dr=100000000;
	}else{
		std::size_t cr=crf, cc=ccf;
		if(cr>=grid.r || cc>=grid.c){
			dr=1000000000;
		}else{
			CellInfo &info=cellinfo[grid[cr][cc]];
			// кофти
			if(!p.ctlL && !p.ctlR && !p.ctlSpace){
				dr=info.dragStop;
				if(info.fastStop){
					p.vel*=1.0-std::min(acc/p.vel.len(), 0.2);
				}
			}else{
				dr=info.dragWalk;
			}
		}
	}
	tBase(p, dr);
	p.onGround=false;
	tGround(p, 1);
	tGround(p, 0);
	tWall(p, 0);
	tWall(p, 1);
	return;
}
void Game::enabled(){
	ctx->ttick(std::floor(ticklen*1000+0.5));
	return;
}
void Game::key(KEType type, TS time, SDL_Keysym k){
	if(k.sym==SDLK_LEFT){
		plr.ctlL=(type==KEType::Down);
	}
	if(k.sym==SDLK_RIGHT){
		plr.ctlR=(type==KEType::Down);
	}
	if(k.sym==SDLK_SPACE){
		plr.ctlSpace=(type==KEType::Down);
	}
	return;
}
void Game::tick(){
	//tick(ball);
	tick(plr);
	for(auto &i:enemies) tick(i);
	return;
}
Rect Game::setView(Vec2i screen){
	// (0,0) to (0,w)
	// (c,r) to (?,0)
	double scale=static_cast<double>(screen.y)/grid.r;
	scale=std::floor(scale+0.5);
	vm.mx=Vec2{1,0}*scale;
	vm.my=-Vec2{0,1}*scale;
	vm.a=Vec2{-plr.pos.x*scale+screen.x/2.0, static_cast<double>(screen.y)};
	vmi=vm.invert();
	Vec2 ll=vmi*Vec2{0,screen.y};
	return Rect{ll, vmi*Vec2{screen.x,0}-ll};
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
		while(p.x<ur.x){
			Recti br=viewport(p, bgsize);
			br.size.x++;
			r->blit(br, bg);
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

	{
		SDL_Texture *plrt=r->getTex("ass/player.png");
		r->blit(viewport(Rect{plr.pos-plr.size/2, plr.size}), plrt);
	}
	std::cout<<plr.pos.x<<' '<<plr.vel.y<<'\n';


	return;
}
