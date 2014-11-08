#pragma once
#include <wip.hpp>
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
struct Object{
	Vec2 pos, vel;
	double mass, drag;
	std::string tex;
};
struct OBall: Object{
	double r;
};
struct ORect: Object{
	bool onGround=false;
	Vec2 size;
};
struct Boulder: OBall{
	// Move along, there's nothing to see here.
	bool hit;
	double damage, acc;
	std::size_t hits;
	Vec2 initvel;
};
struct Entity: ORect{
	double hp, maxhp;
};
struct Player: Entity{
	double gndAcc, airAcc;
	double jump;
	bool spacemask=false;
	unsigned dj=0;
	bool ctlL=false, ctlR=false, ctlSpace=false;
};
struct Enemy: Entity{
	double r, acc, damage;
};
struct CellInfo{
	bool solidU, solidD, solidL, solidR;
	double dragWalk, dragStop;
	bool fastStop;
	double jump;
	std::string tex;
};
struct Game: State{
	using Cell=std::uint8_t;
	Array2d<Cell> grid;
	Game(std::istream &in);
	~Game();

	std::unordered_map<Cell, CellInfo> cellinfo;
	Vec2 bgsize;

	double ticklen;
	Vec2 gravity;
	double baseKB;

	Boulder ball;
	Player plr;
	std::vector<Enemy> enemies;
	bool hasBall=false;

	void tBase(Object &o, double drag, double gc);
	void tGround(ORect &o, bool dir);
	void tWall(ORect &o, bool dir);
	bool tReflect(OBall &o);
	void tick(Boulder &o);
	void tick(Enemy &e);
	void tick(Player &p);
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
	void render(Rend *r, ORect&);
	void render(Rend *r, OBall&);
	void render(Rend *r, Entity&);
};
