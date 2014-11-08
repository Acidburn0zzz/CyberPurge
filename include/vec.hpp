#pragma once
#include <cmath>
template<class T>
struct Vec2t{
	T x, y;
	T len2(){
		return x*x+y*y;
	}
	decltype(std::sqrt(std::declval<T>())) len(){
		return std::sqrt(len2());
	}
};
template<class T>
Vec2t<T> operator+(Vec2t<T> a, Vec2t<T> b){
	return Vec2t<T>{a.x+b.x, a.y+b.y};
}
template<class T>
Vec2t<T> &operator+=(Vec2t<T> &a, Vec2t<T> b){
	a.x+=b.x;
	a.y+=b.y;
	return a;
}
template<class T>
Vec2t<T> operator-(Vec2t<T> a, Vec2t<T> b){
	return Vec2t<T>{a.x-b.x, a.y-b.y};
}
template<class T>
Vec2t<T> &operator-=(Vec2t<T> &a, Vec2t<T> b){
	a.x-=b.x;
	a.y-=b.y;
	return a;
}
template<class T, class T2>
Vec2t<T> operator*(T2 a, Vec2t<T> b){
	return Vec2t<T>{static_cast<T>(a*b.x), static_cast<T>(a*b.y)};
}
template<class T, class T2>
Vec2t<T> operator*(Vec2t<T> a, T2 b){
	return Vec2t<T>{static_cast<T>(a.x*b), static_cast<T>(a.y*b)};
}
template<class T>
Vec2t<T> &operator*=(Vec2t<T> &a, T b){
	a.x*=b;
	a.y*=b;
	return a;
}
template<class T, class T2>
Vec2t<T> operator/(Vec2t<T> a, T2 b){
	return Vec2t<T>{static_cast<T>(a.x/b), static_cast<T>(a.y/b)};
}
template<class T>
Vec2t<T> &operator/=(Vec2t<T> &a, T b){
	a.x/=b;
	a.y/=b;
	return a;
}
template<class T>
T dot(Vec2t<T> a, Vec2t<T> b){
	return a.x*b.x+a.y*b.y;
}
template<class T>
T cross(Vec2t<T> a, Vec2t<T> b){
	return a.x*b.y-a.y*b.x;
}

#define WIP__OPERATOR_DEF(oper, as, call) \
	namespace wip_operator{namespace{ \
		struct tag_##oper{}; \
		template<class LHS> \
		struct helper_##oper{ \
			LHS &&lhs; \
		}; \
		template<class LHS> \
		helper_##oper<LHS> operator as(LHS &&lhs, tag_##oper){return helper_##oper<LHS>{std::forward<LHS>(lhs)};} \
		template<class LHS, class RHS> \
		decltype(call(std::declval<LHS>(), std::declval<RHS>())) operator as(helper_##oper<LHS> lhs, RHS &&rhs){ \
			return call(std::forward<LHS>(lhs.lhs), std::forward<RHS>(rhs)); \
		} \
	}}
#define WIP__OPERATOR(oper, as) as (wip_operator::tag_##oper()) as

WIP__OPERATOR_DEF(dot, *, dot)
WIP__OPERATOR_DEF(cross, *, cross)
#define _dot_ WIP__OPERATOR(dot, *)
#define _cross_ WIP__OPERATOR(cross, *)


template<class T>
struct Mat2t{
	Vec2t<T> mx, my, a;
};
template<class T>
Vec2t<T> operator*(Mat2t<T> a, Vec2t<T> b){
	return Vec2t<T>{
		a.mx _dot_ b,
		a.my _dot_ b
	}+a.a;
}
template<class T>
Mat2t<T> operator*(Mat2t<T> a, Mat2t<T> b){
	Mat2t<T> res;
	res.mx=Vec2t<T>{
		a.mx _dot_ Vec2t<T>{b.mx.x, b.my.x},
		a.mx _dot_ Vec2t<T>{b.mx.y, b.my.y}
	};
	res.my=Vec2t<T>{
		a.my _dot_ Vec2t<T>{b.mx.x, b.my.x},
		a.my _dot_ Vec2t<T>{b.mx.y, b.my.y}
	};
	res.a=a.a+a*b.a;
	return res;
}
template<class T>
struct Rectt{
	Vec2t<T> pos, size;
};
