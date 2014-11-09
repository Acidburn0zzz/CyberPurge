#pragma once
#include <wip.hpp>
#include <random>
#include <queue>
template<class T>
struct Array2d{
	std::size_t r, c;
	T *data;
	Array2d(){
		r=0;
		c=0;
		data=nullptr;
		return;
	}
	Array2d(std::size_t r, std::size_t c):r(r),c(c){
		data=new T[r*c];
		return;
	}
	~Array2d(){
		clear();
		return;
	}
	void clear(){
		if(data){
			delete[] data;
			data=nullptr;
			r=0;
			c=0;
		}
		return;
	}
	Array2d(const Array2d&)=delete;
	Array2d(Array2d &&that):r(that.r),c(that.c),data(that.data){
		that.data=nullptr;
		that.r=0;
		that.c=0;
		return;
	}
	Array2d &operator=(const Array2d&)=delete;
	Array2d &operator=(Array2d &&that){
		if(this==&that) return *this;
		clear();
		data=that.data;
		r=that.r;
		c=that.c;
		that.data=nullptr;
		that.r=0;
		that.c=0;
		return *this;
	}
	T *operator[](std::size_t i){
		return data+i*c;
	}
	const T *operator[](std::size_t i) const{
		return data+i*c;
	}
};
struct Animation{
	struct Frame{
		SDL_Surface *img;
		Vec2 size;
		TS time;
	};
	Vec2 pos;
	std::queue<Frame> q;
	TS until=0;
	bool direction;
};
struct Object{
	Vec2 pos, vel;
	double mass, drag;

	bool direction=false, dirflip=false;
	std::vector<std::pair<SDL_Surface*, Vec2>> texs;
	std::size_t stage=0;
};
struct OBall: Object{
	double r;
};
struct ORect: Object{
	bool onGround=false;
	Vec2 size;
};
struct Boulder: OBall{
	bool hit;
	double damage, acc, kb, lifesteal;
	std::size_t hits;
};
struct Entity: ORect{
	double hp, maxhp, jump;
	double gndAcc, airAcc;
	std::size_t walkStages, jumpStage, forceStage;
	TS forceStageUntil=0;
	double walkLen;
	Animation death;
	Mix_Chunk *deathsound;
};
struct Weapon{
	double dmg, range, angle, kb, lifesteal;
	std::size_t stage;
	TS time;
	Animation anim;
	Mix_Chunk *snd;
};
struct Player: Entity{
	bool spacemask=false;
	unsigned dj=0;
	double fallUntil=1/0.;
	bool ctlL=false, ctlR=false, ctlSpace=false, ctlX=false, ctlZ=false;
	TS shootTime=0;
	Weapon bb, sg;
};
struct Enemy: Entity{
	double damage;
	Vec2 tgt;
	TS notTrackingUntil=0;
	unsigned ntReason=0;
};
struct CellInfo{
	bool solidU, solidD, solidL, solidR;
	double dragWalk, dragStop;
	double jump;
	SDL_Surface *tex;
};
struct Game: State{
	using Cell=std::uint8_t;
	Array2d<Cell> grid;
	std::mt19937_64 rnd;
	Game(std::istream &game, std::istream &terrain);
	~Game();

	std::unordered_map<Cell, CellInfo> cellinfo;
	std::string fnBG, fnSnd, fnBall, fnEnd, fnSndEnd;
	int volSnd, volSndEnd;
	Mix_Chunk *snd, *sndEnd;
	SDL_Surface *imgBG;
	Vec2 bgsize, bgshift;

	Rect visible;
	double ticklen;
	Vec2 gravity;
	
	std::vector<std::pair<Enemy,double>> enemyKinds;

	Boulder ball;
	double ballv;
	Vec2 balloff;
	Player plr;
	std::vector<Enemy> enemies;
	bool hasBall=false;

	void tBase(Object &o, double drag, double gc);
	void tGround(ORect &o, bool dir);
	bool tWall(ORect &o, bool dir);
	bool tReflect(OBall &o);
	void tick(Boulder &o);
	void tick(Enemy &e);
	void tick(Player &p);
	CellInfo *gndcell(ORect &o);
	void attack(Player &plr, const Weapon &w);
	void spawn(Enemy kind);
	virtual void enabled();
	virtual void key(KEType type, TS time, SDL_Keysym k);
	virtual void tick();

	Mat2 vm, vmi;
	Rect setView(Vec2i screen);
	Vec2i viewport(Vec2);
	Recti viewport(Rect);
	inline Recti viewport(Vec2 a, Vec2 b){
		return viewport(Rect{a,b});
	}
	virtual void render(Rend *r);
	void render(Rend *r, Object&);
	void render(Rend *r, Entity&);
	void render(Rend *r, Animation&);
	void anim(Vec2 pos, bool dir, Animation a);
	std::vector<Animation> animations;
};
