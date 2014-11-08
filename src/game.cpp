#include <game.hpp>
#include <algorithm>

static Vec2 reflect(Vec2 a, Vec2 b){
	Vec2 dc=a _dc_ b;
	dc/=dc.len();
	Vec2 dc1={dc.x,-dc.y};
	Vec2 dc2={dc.y, dc.x};
	double al=a.len();
	a=Vec2{
		a _dot_ dc1,
		a _dot_ dc2
	};
	a/=a.len();
	a*=al;
	return a;
}
static bool close(ORect &a, ORect &b){
	double dist=(a.pos-b.pos).len();
	return dist<(a.size.len()+b.size.len())/2;
}
static bool close(ORect &a, OBall &b){
	double dist=(a.pos-b.pos).len();
	return dist<(a.size.len()+b.r)/2;
}
Game::~Game(){
	return;
}
Game::Game(std::istream &fin){
	std::size_t r, c;
	fin>>r>>c;
	grid=Array2d<Cell>(r, c);
	for(std::size_t i=r;i--;){
		for(std::size_t j=0;j<c;j++){
			char c;
			fin>>c;
			grid[i][j]=c;
		}
	}
	fin>>bgsize.x>>bgsize.y;

	plr.pos=Vec2{5,5};
	plr.vel=Vec2{0.1,0};
	plr.tex="ass/player-0.png";
	plr.mass=60;
	plr.drag=0.5;
	plr.size=Vec2{9.24/4,12.03/4};
	plr.gndAcc=40;
	plr.airAcc=30;
	plr.jump=15;

	plr.maxhp=100;
	plr.hp=100;

	ball.pos=Vec2{-100,-100};
	ball.vel=Vec2{0,0};
	ball.tex="ass/ball.png";
	ball.mass=1;
	ball.r=0.4;
	ball.drag=1;
	ball.damage=0.1;
	ball.acc=1.3;
	ball.initvel=Vec2{0,0};
	ticklen=0.01;

	Enemy e;
	e.maxhp=30;
	e.hp=e.maxhp;
	e.mass=1;
	e.drag=2;
	e.pos=Vec2{10,10};
	e.size=Vec2{991,779}/200;
	e.vel=Vec2{0,0};
	e.tex="ass/enemy-1-0.png";
	e.damage=20;
	e.acc=5;
	enemies.push_back(e);

	//gravity=Vec2{0,-4};
	gravity=Vec2{0,-55};
	baseKB=10.0;

	cellinfo['#']=CellInfo{
		1,1,1,1,
		10,60,true,
		1,
		"ass/cell/wall.png"
	};
	cellinfo['J']=CellInfo{
		1,1,1,1,
		10,60,true,
		2,
		"ass/cell/jumpwall.png"
	};
	cellinfo['j']=CellInfo{
		1,0,0,0,
		10,60,true,
		2,
		"ass/cell/jump.png"
	};
	cellinfo['.']=CellInfo{
		0,0,0,0,
		1,1,false,
		1,
		"ass/cell/air.png"
	};
	return;
}
void Game::tBase(Object &o, double drag, double gc){
	o.vel+=gravity*ticklen*gc;
	o.pos+=o.vel*ticklen;
	drag=drag*std::sqrt(o.vel.x*o.vel.x/o.vel.len2());
	double dragc=(1.0-o.vel.len2()*o.drag*drag/o.mass*ticklen);
	if(dragc<=0.01) dragc=0.01;
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
	double minx=o.pos.x-o.size.x/2+0.2;
	double maxx=o.pos.x+o.size.x/2;
	int maxxi=maxx+0.8;
	int yi=y;
	if(dir==0){
		if(y-yi<0.3) return;
	}else{
		if(y-yi>0.7) return;
	}
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
	double miny=o.pos.y-o.size.y/2+0.2;
	double maxy=o.pos.y+o.size.y/2;
	int maxyi=maxy+0.8;
	int xi=x;
	if(dir==0){
		if(x-xi>0.7) return;
	}else{
		if(x-xi<0.3) return;
	}
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
bool Game::tReflect(OBall &o){
	if(o.vel.y<0){
		int x=o.pos.x;
		int y=o.pos.y-o.r;
		CellInfo &cell=cellinfo[grid[y][x]];
		if(cell.solidU){
			o.vel.y=-o.vel.y*std::sqrt(cell.jump);
			o.pos.y=y+1+o.r;
			return true;
		}
	}
	if(o.vel.y>0){
		int x=o.pos.x;
		int y=o.pos.y+o.r;
		CellInfo &cell=cellinfo[grid[y][x]];
		if(cell.solidD){
			o.vel.y=-o.vel.y;
			o.pos.y=y-o.r;
			return true;
		}
	}
	if(o.vel.x<0){
		int x=o.pos.x-o.r;
		int y=o.pos.y;
		CellInfo &cell=cellinfo[grid[y][x]];
		if(cell.solidR){
			o.vel.x=-o.vel.x;
			o.pos.x=x+1+o.r;
			return true;
		}
	}
	if(o.vel.x>0){
		int x=o.pos.x+o.r;
		int y=o.pos.y;
		CellInfo &cell=cellinfo[grid[y][x]];
		if(cell.solidR){
			o.vel.x=-o.vel.x;
			o.pos.x=x-o.r;
			return true;
		}
	}
	return false;
}
void Game::tick(Boulder &o){
	if(o.pos.x<1 || o.pos.y<1 || o.pos.x>=grid.c || o.pos.y>=grid.r){
		hasBall=false;
		return;
	}
	tBase(o, 0, 0);
	if(tReflect(o)){
		if(o.hits==0){
			hasBall=false;
			return;
		}
		o.hits--;
	}
	for(auto &i:enemies){
		if(close(i, o)){
			double dmg=o.vel.len()*o.damage;
			i.hp-=dmg;
			if(i.hp>0){
				o.vel*=o.acc;
				o.vel=reflect(o.vel, o.pos-i.pos);
				Vec2 knockback=i.pos-o.pos;
				knockback/=knockback.len();
				knockback*=baseKB*dmg;
				i.vel+=knockback/i.mass;
			}
		}
	}
	return;
}
void Game::tick(Enemy &e){
	std::size_t cr=std::floor(e.pos.y-e.size.y/2-0.01), cc=std::floor(e.pos.x);
	CellInfo *info=&cellinfo[grid[cr][cc]];

	double acc=e.acc;
	double dr;
	if(e.pos.x<plr.pos.x){
		e.vel+=Vec2{acc*ticklen,0};
		dr=info->dragWalk;
	}else if(e.pos.x>plr.pos.x){
		e.vel+=Vec2{-acc*ticklen,0};
		dr=info->dragWalk;
	}else{
		dr=info->dragStop;
	}

	if((e.pos-plr.pos).len()<plr.size.len()){
		plr.hp-=e.damage*ticklen;
	}

	tBase(e, dr, 1.0);
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
	double dr;

	std::size_t cr=std::floor(p.pos.y-p.size.y/2-0.01), cc=std::floor(p.pos.x);
	CellInfo *info=&cellinfo[grid[cr][cc]];

	if(p.ctlSpace){
		if(!p.spacemask){
			if(p.onGround){
				p.vel.y+=p.jump*info->jump;
				p.dj=1;
			}else if(p.dj){
				p.vel.y=p.jump*info->jump;
				p.dj--;
			}
			info=&cellinfo['.'];
			p.spacemask=true;
		}
	}else p.spacemask=false;

	if((!p.ctlL && !p.ctlR && !p.ctlSpace) || (p.ctlL && p.vel.x>0) || (p.ctlR && p.vel.x<0)){
		dr=info->dragStop;
		if(info->fastStop){
			p.vel*=1.0-std::min(acc/std::max(1.0, p.vel.len()), 0.2);
		}
	}else{
		dr=info->dragWalk;
	}
	tBase(p, dr, 1.0);
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
	if(k.sym==SDLK_c && type==KEType::Down && !hasBall){
		ball.vel=(1+20/std::max(1.0, plr.vel.len()))*plr.vel;
		ball.vel+=ball.initvel;
		ball.pos=plr.pos;
		ball.hit=false;
		ball.hits=4;
		hasBall=true;
	}
	if(k.sym==SDLK_x){
		if(hasBall && close(plr, ball)){
			ball.vel*=ball.acc;
			ball.vel=reflect(ball.vel, ball.pos-plr.pos);
			ball.hits=5;
		}
		for(auto &i:enemies){
			if(close(plr, i)){
				i.hp-=1;
				Vec2 knockback=i.pos-plr.pos;
				knockback/=knockback.len();
				knockback*=baseKB;
				i.vel+=knockback/i.mass;
			}
		}
	}
	return;
}
void Game::tick(){
	if(hasBall){
		tick(ball);
	}
	tick(plr);
	for(auto &i:enemies) tick(i);
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(), [](const Enemy &a)->bool{
			return a.hp<=0;
		}),
		enemies.end()
	);
	return;
}
