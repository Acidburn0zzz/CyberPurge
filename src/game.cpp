#include <game.hpp>
#include <slideshow.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>

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
	return dist<(a.size.len()+b.size.len())/4;
}
static bool close(ORect &a, OBall &b){
	double dist=(a.pos-b.pos).len();
	return dist<(a.size.len()+b.r)/2;
}
static void read(std::istream &in, Animation &a, Ctx *ctx){
	std::size_t n;
	in>>a.pos.x>>a.pos.y>>n;
	while(n--){
		Animation::Frame f;
		std::string name;
		in>>f.time>>name>>f.size.x>>f.size.y;
		f.img=ctx->getImg(name);
		a.q.push(f);
	}
	return;
}
static void read(std::istream &in, Entity &e, Ctx *ctx){
	e.vel=Vec2{0,0};
	int js;
	in>>e.maxhp>>e.mass>>e.drag>>e.size.x>>e.size.y>>e.gndAcc>>e.airAcc>>e.jump>>e.dirflip>>e.walkStages>>e.walkLen>>js;
	e.jumpStage=js;
	e.hp=e.maxhp;
	std::string texs, tex;
	std::getline(in, texs);
	std::istringstream tin{texs};
	while(tin>>tex){
		SDL_Surface *srf=ctx->getImg(tex);
		Vec2 srfs{srf->w, srf->h};
		Vec2 size=srfs*e.size.y/srfs.y;
		e.texs.emplace_back(srf, size);
	}
	read(in, e.death, ctx);
	int s;
	in>>tex>>s;
	e.deathsound=ctx->getSnd(tex, s);
	return;
}
static void read(std::istream &in, Weapon &w, Ctx *ctx){
	std::string fn;
	in>>w.dmg>>w.range>>w.angle>>w.kb>>w.lifesteal>>w.stage>>w.time;
	read(in, w.anim, ctx);
	int s;
	in>>fn>>s;
	w.snd=ctx->getSnd(fn, s);
	return;
}
void Game::attack(Player &plr, const Weapon &w){
	plr.shootTime=ctx->now()+w.time;
	if(w.stage+1){
		plr.forceStage=w.stage;
		plr.forceStageUntil=plr.shootTime;
	}
	anim(plr.pos, plr.direction, w.anim);
	ctx->playSnd(w.snd);
	for(auto &i:enemies){
		if((plr.pos-i.pos).len()<w.range+i.size.len()/2){
			Vec2 d=i.pos-plr.pos, knockback=d;
			if(plr.direction) d=-d;
			double dy=std::min(std::abs(i.size.y-plr.size.y)/2, std::abs(d.y));
			if(d.y>0) d.y-=dy;
			else d.y+=dy;
			if(2*std::abs(std::atan2(d.y, d.x))>w.angle) continue;
			plr.hp+=std::min(i.hp, w.dmg)*w.lifesteal;
			i.hp-=w.dmg;
			knockback/=knockback.len();
			knockback*=w.kb;
			i.vel+=knockback/i.mass;
		}
	}
	return;
}
Game::~Game(){
	return;
}
Game::Game(std::istream &gin, std::istream &tin){
	ticklen=0.01;

	unsigned long long seed;
	gin>>seed;
	gin>>gravity.x>>gravity.y;

	gin>>ball.mass>>ball.drag>>ballv>>ball.r>>balloff.x>>balloff.y;
	gin>>ball.acc>>ball.kb>>ball.damage>>ball.lifesteal;
	gin>>fnBall;
	ball.vel=Vec2{0,0};

	gin>>fnBG>>bgsize.x>>bgsize.y>>bgshift.x>>bgshift.y;
	gin>>fnSnd>>volSnd;

	gin>>fnEnd>>fnSndEnd>>volSndEnd;

	rnd.seed(seed);

	std::size_t r, c;
	tin>>r>>c;
	grid=Array2d<Cell>(r, c);
	for(std::size_t i=r;i--;){
		for(std::size_t j=0;j<c;j++){
			char c;
			tin>>c;
			grid[i][j]=c;
		}
	}

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
bool Game::tWall(ORect &o, bool dir){
	if(dir){
		if(o.vel.x>0) return false;
	}else{
		if(o.vel.x<0) return false;
	}
	double x=o.pos.x;
	if(dir==0) x+=o.size.x/2;
	else x-=o.size.x/2;
	double miny=o.pos.y-o.size.y/2+0.2;
	double maxy=o.pos.y+o.size.y/2;
	int maxyi=maxy+0.8;
	int xi=x;
	if(dir==0){
		if(x-xi>0.7) return false;
	}else{
		if(x-xi<0.3) return false;
	}
	for(int i=miny;i<maxyi;i++){
		CellInfo &c=cellinfo[grid[i][xi]];
		if(dir==0?c.solidL:c.solidR){
			o.vel.x=0;
			if(dir==0) o.pos.x=xi-o.size.x/2;
			else o.pos.x=xi+1+o.size.x/2;
			return true;
		}
	}
	return false;
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
			plr.hp+=std::min(dmg, i.hp)*o.lifesteal;
			i.hp-=dmg;
			if(i.hp>0){
				o.vel*=o.acc;
				o.vel=reflect(o.vel, o.pos-i.pos);
				Vec2 knockback=i.pos-o.pos;
				knockback/=knockback.len();
				knockback*=ball.kb*dmg;
				i.vel+=knockback/i.mass;
			}
		}
	}
	return;
}
void Game::tick(Enemy &e){
	CellInfo *info=gndcell(e);

	double acc=e.onGround?e.gndAcc:e.airAcc;
	double dr;
	TS now=ctx->now();
	bool closeToTgt=std::abs(e.pos.x-e.tgt.x)<(plr.size.x+e.size.x)/8;
	if(e.notTrackingUntil > now){
		if(e.ntReason==0 && std::abs(e.pos.y-plr.pos.y)<std::abs(e.size.y-plr.size.y)/2){
			e.notTrackingUntil=0;
		}else if(e.ntReason==1 && std::abs(e.pos.x-e.tgt.x)<1){
			e.notTrackingUntil=0;
		}
	}
	if(e.notTrackingUntil <= now){
		e.tgt=plr.pos;
		if(closeToTgt && !close(e, plr)){
			e.notTrackingUntil=now+5000;
			std::normal_distribution<double> dist(0, 10);
			e.tgt.x+=dist(rnd);
			e.ntReason=0;
		}
	}
	if(e.pos.x<e.tgt.x){
		e.vel+=Vec2{acc*ticklen,0};
		e.direction=0;
	}else if(e.pos.x>e.tgt.x){
		e.vel+=Vec2{-acc*ticklen,0};
		e.direction=1;
	}
	if(e.vel.x*(e.tgt.x-e.pos.x)>0){
		dr=info->dragWalk;
	}else{
		dr=info->dragStop;
	}

	if(close(e,plr)){
		plr.hp-=e.damage*ticklen;
	}

	if(e.tgt.y - e.pos.y > 0.1 && e.onGround){
		e.vel.y+=e.jump*info->jump;
	}

	tBase(e, dr, 1.0);
	e.onGround=false;
	tGround(e, 1);
	tGround(e, 0);
	bool w1=tWall(e, 0);
	bool w2=tWall(e, 1);
	if((w1 || w2) && e.onGround){
		if(e.notTrackingUntil>now && e.ntReason==0 && rnd()%2){
			e.notTrackingUntil=0;
		}else{
			e.notTrackingUntil=now+3000;
			e.tgt.x=plr.pos.x;
			e.tgt.y=grid.r-1;
			e.ntReason=1;
		}
	}
	return;
}
void Game::tick(Player &p){
	if(p.hp>p.maxhp) p.hp=p.maxhp;
	double acc=p.onGround?p.gndAcc:p.airAcc;
	if(p.ctlR){
		p.vel+=Vec2{acc*ticklen,0};
		p.direction=0;
	}
	if(p.ctlL){
		p.vel+=Vec2{-acc*ticklen,0};
		p.direction=1;
	}
	if(p.ctlZ && p.shootTime<ctx->now()){
		if(hasBall && close(plr, ball)){
			ball.vel*=ball.acc;
			ball.vel=reflect(ball.vel, ball.pos-plr.pos);
			ball.hits=5;
		}
		attack(plr, plr.bb);
	}
	if(p.ctlX && p.shootTime<ctx->now()){
		attack(plr, plr.sg);
	}
	double dr;

	CellInfo *info=gndcell(p);

	if(p.onGround) p.dj=1;
	if(p.ctlSpace){
		if(!p.spacemask){
			if(p.onGround){
				p.vel.y+=p.jump*info->jump;
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
		if(p.onGround){
			p.vel*=1.0-std::min(acc/std::max(1.0, p.vel.len()), 0.2);
		}
	}else{
		dr=info->dragWalk;
	}
	tBase(p, dr, 1.0);
	p.onGround=false;
	tGround(p, 1);
	if(p.pos.y<plr.fallUntil){
		plr.fallUntil=1/0.;
		tGround(p, 0);
	}
	tWall(p, 0);
	tWall(p, 1);
	return;
}
void Game::enabled(){
	imgBG=ctx->getImg(fnBG);
	snd=ctx->getSnd(fnSnd, volSnd);
	sndEnd=ctx->getSnd(fnSndEnd, volSndEnd);
	ball.texs.emplace_back(ctx->getImg(fnBall), Vec2{ball.r,ball.r}*2);
	ctx->stopSnd();
	ctx->playSnd(snd, 1);
	ctx->ttick(std::floor(ticklen*1000+0.5));

	{
		std::ifstream fin("data/player");
		read(fin, plr, ctx);
		read(fin, plr.bb, ctx);
		read(fin, plr.sg, ctx);
		plr.vel=Vec2{0,0};
	}
	plr.pos=Vec2{1,1}+plr.size/2;

	{
		std::ifstream fin("data/enemies");
		std::size_t n;
		fin>>n;
		while(n--){
			Enemy e;
			double chance;
			fin>>chance;
			read(fin, e, ctx);
			fin>>e.damage;
			enemyKinds.emplace_back(e, chance);
		}
	}

	{
		std::ifstream fin("data/cells");
		std::size_t n;
		fin>>n;
		while(n--){
			char id;
			CellInfo i;
			std::string tex;
			fin>>id;
			fin>>i.solidU>>i.solidD>>i.solidL>>i.solidR;
			fin>>i.dragWalk>>i.dragStop>>i.jump;
			fin>>tex;
			i.tex=ctx->getImg(tex);
			cellinfo[id]=i;
		}
	}

	return;
}
void Game::key(KEType type, TS time, SDL_Keysym k){
	if(k.sym==SDLK_LEFT || k.sym == SDLK_a){
		plr.ctlL=(type==KEType::Down);
	}
	if(k.sym==SDLK_RIGHT || k.sym == SDLK_d){
		plr.ctlR=(type==KEType::Down);
	}
	if(k.sym==SDLK_SPACE || k.sym==SDLK_UP || k.sym==SDLK_w){
		plr.ctlSpace=(type==KEType::Down);
	}
	if(k.sym==SDLK_x){
		plr.ctlX=(type==KEType::Down);
	}
	if(k.sym==SDLK_z){
		plr.ctlZ=(type==KEType::Down);
	}
	if(type!=KEType::Down) return;
	if(k.sym==SDLK_DOWN || k.sym==SDLK_s){
		CellInfo *info=gndcell(plr);
		if(!info->solidD) plr.fallUntil=plr.pos.y-1.01;
	}
	if(k.sym==SDLK_c && !hasBall && plr.shootTime<ctx->now()){
		ball.vel=plr.vel;
		ball.pos=balloff;
		if(plr.direction==0){
			ball.vel.x+=ballv;
		}else{
			ball.vel.x-=ballv;
			ball.pos.x=-ball.pos.x;
		}
		ball.pos+=plr.pos;
		ball.hit=false;
		ball.hits=4;
		hasBall=true;
	}
	if(k.sym==SDLK_v && plr.shootTime<ctx->now()){
		hasBall=false;
		plr.shootTime=ctx->now()+1000;
	}
	return;
}
void Game::tick(){
	if(plr.hp<=0){
		anim(plr.pos, plr.direction, plr.death);
		Slideshow *e=new Slideshow([this]()->void{
			SDL_Event ev;
			ev.type=SDL_QUIT;
			SDL_PushEvent(&ev);
		}, {fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd,fnEnd});
		ctx->stopSnd();
		ctx->playSnd(sndEnd);
		ctx->state(e);
		return;
	}

	{
		std::uniform_real_distribution<double> dist(0,enemies.size()<5?1:1./ticklen);
		double val=dist(rnd);
		if(val<1){
			double total=0.0;
			for(auto &i:enemyKinds){
				total+=i.second;
			}
			val*=total;
			for(auto &i:enemyKinds){
				if(val<=i.second){
					spawn(i.first);
					break;
				}else val-=i.second;
			}
		}
	}

	if(hasBall){
		tick(ball);
	}
	tick(plr);
	for(auto &i:enemies){
		tick(i);
		if(i.hp<=0){
			anim(i.pos, i.direction, i.death);
			if(i.deathsound) ctx->playSnd(i.deathsound);
		}
	}
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(), [this](const Enemy &a)->bool{
			return a.hp<=0 || a.hp!=a.hp || a.pos.x-a.pos.x/2<0 || a.pos.y-a.size.y/2<0 || a.pos.x+a.size.x/2 > grid.c || a.pos.y+a.size.y/2 > grid.r;
		}),
		enemies.end()
	);
	return;
}
void Game::anim(Vec2 pos, bool dir, Animation a){
	if(dir) a.pos.x=-a.pos.x;
	a.pos+=pos;
	a.direction=dir;
	if(a.q.size()==0) return;
	a.until=ctx->now()+a.q.front().time;
	animations.push_back(std::move(a));
	return;
}
CellInfo *Game::gndcell(ORect &o){
	std::size_t cr=std::floor(o.pos.y-o.size.y/2-0.02), cc=std::floor(o.pos.x);
	return &cellinfo[grid[cr][cc]];
}
void Game::spawn(Enemy e){
	Rect vis=visible;
	vis.pos-=e.size/2;
	vis.size+=e.size;
	std::vector<Vec2> poses;
	for(std::size_t i=0;i<grid.r;i++){
		for(std::size_t j=0;j<grid.c;j++){
			bool free=true;
			for(std::size_t dx=0;free&&dx<e.size.x;dx++){
				for(std::size_t dy=0;free&&dy<e.size.y;dy++){
					if(i+dy>=grid.r){
						free=false;
					}else if(j+dx>=grid.c){
						free=false;
					}else{
						if(grid[i+dy][j+dx]!='-'){
							free=false;
						}
					}
				}
			}
			if(free){
				Vec2 pos{j,i};
				pos+=e.size/2;
				if((pos.x<vis.pos.x || pos.x>vis.pos.x+vis.size.x) && (pos.y<vis.pos.y || pos.y>vis.pos.y+vis.size.y)){
					poses.push_back(pos);
				}
			}
		}
	}
	std::sort(poses.begin(), poses.end(), [this](Vec2 a, Vec2 b)->bool{
		return (a-plr.pos).len()<(b-plr.pos).len();
	});
	std::normal_distribution<double> ndist(0, 1+poses.size()/16);
	while(true){
		std::size_t val=std::abs(ndist(rnd));
		if(val>=poses.size()) continue;
		e.pos=poses[val];
		enemies.push_back(std::move(e));
		break;
	}
	return;
}
