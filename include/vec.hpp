#pragma once
#include <cmath>
template<class T1, class T2>
using mult=std::enable_if_t<std::is_arithmetic<T1>::value && std::is_arithmetic<T2>::value, decltype(std::declval<T1>()*std::declval<T2>())>;
template<class T1, class T2>
using divt=std::enable_if_t<std::is_arithmetic<T1>::value && std::is_arithmetic<T2>::value, decltype(std::declval<T1>()/std::declval<T2>())>;
template<class T>
struct Vec2t{
	T x, y;
	Vec2t(){}
	template<class T1, class T2>
	Vec2t(T1 a, T2 b):x(a),y(b){}
	Vec2t(const Vec2t&)=default;
	Vec2t(Vec2t&&)=default;
	Vec2t &operator=(const Vec2t&)=default;
	Vec2t &operator=(Vec2t&&)=default;
	template<class T2>
	explicit Vec2t(const Vec2t<T2> &a):x(a.x),y(a.y){}
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
Vec2t<T> operator-(Vec2t<T> a){
	return Vec2t<T>{-a.x, -a.y};
}
template<class T>
Vec2t<T> &operator-=(Vec2t<T> &a, Vec2t<T> b){
	a.x-=b.x;
	a.y-=b.y;
	return a;
}
template<class T, class T2>
Vec2t<mult<T2,T>> operator*(T2 a, Vec2t<T> b){
	return Vec2t<mult<T2,T>>{a*b.x, a*b.y};
}
template<class T, class T2>
Vec2t<mult<T,T2>> operator*(Vec2t<T> a, T2 b){
	return Vec2t<mult<T,T2>>{a.x*b, a.y*b};
}
template<class T>
Vec2t<T> &operator*=(Vec2t<T> &a, T b){
	a.x*=b;
	a.y*=b;
	return a;
}
template<class T, class T2>
Vec2t<divt<T,T2>> operator/(Vec2t<T> a, T2 b){
	return Vec2t<divt<T,T2>>{a.x/b, a.y/b};
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
	}} \
	template<class LHS> \
	wip_operator::helper_##oper<LHS> operator as(LHS &&lhs, wip_operator::tag_##oper){return wip_operator::helper_##oper<LHS>{std::forward<LHS>(lhs)};} \
	template<class LHS, class RHS> \
	decltype(call(std::declval<LHS>(), std::declval<RHS>())) operator as(wip_operator::helper_##oper<LHS> lhs, RHS &&rhs){ \
		return call(std::forward<LHS>(lhs.lhs), std::forward<RHS>(rhs)); \
	}
#define WIP__OPERATOR(oper, as) as wip_operator::tag_##oper() as

WIP__OPERATOR_DEF(dot, *, dot)
WIP__OPERATOR_DEF(cross, *, cross)
#define _dot_ WIP__OPERATOR(dot, *)
#define _cross_ WIP__OPERATOR(cross, *)


template<class T>
struct Mat2t{
	Vec2t<T> mx, my, a;
	Mat2t invert() const;
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
Mat2t<T> Mat2t<T>::invert() const{
	Mat2t<T> copy=*this, res={
		Vec2t<T>{1,0},
		Vec2t<T>{0,1},
		Vec2t<T>{0,0}
	};
	double m;

	m=copy.mx.x;
	copy.mx/=m;
	res.mx/=m;

	m=copy.my.x;
	copy.my-=m*copy.mx;
	res.my-=m*res.mx;

	m=copy.my.y;
	copy.my/=m;
	res.my/=m;

	m=copy.mx.y;
	copy.mx-=m*copy.my;
	res.mx-=m*res.my;

	res.a=-(res*copy.a);

	return res;
}
template<class T>
struct Rectt{
	Vec2t<T> pos, size;
};
