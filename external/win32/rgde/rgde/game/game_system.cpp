// game_system.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "game.h"

//#include <boost/bind.hpp>

namespace events
{
  struct base_event
  {
    int foo(){return 1;}

  };

  class system
  {
  public:

  };
}

template<class Callee, typename RetType>
struct functor
{
  typedef RetType (Callee::*pfunc)();

  pfunc m_func;

  functor(pfunc f) : m_func(f) {}

  RetType call(Callee* obj)
  {
    return (obj->*m_func)();
  }
};

void  x(int, int) {}

#include <iostream>

template <typename P1, typename P2>
int Call(void (*func)(P1, P2))
{
  return 1;
}

//boost::function<int (X*, int)> f;



//template <class FuncType>
template <class P1>
class A {};

template <class Obj, class RetType, class P1>
class A<RetType (Obj*, P1) >  
{
  typedef Obj function_ownner;
  typedef RetType return_type;
  typedef P1 param1_type;
  enum {params_num = 1};
  enum {member_function = 1};
};



template<typename T>
struct test_type
{
  T* m_func;
  test_type(T* f) : m_func(f)
  {
    Call(m_func);
  }

};

test_type<void (int, int) > t(&x);


namespace utility
{
  namespace test
  {
    void timer_test();
  }
}

int _tmain(int argc, _TCHAR* argv[])
{
  typedef functor<events::base_event, int> fuu;

  fuu f(&events::base_event::foo);

  events::base_event be;
  f.call(&be);

  game::Game system;

  system.setState<game::BaseState>();
  system.pushState<game::BaseState>();
  system.popState();


  utility::test::timer_test();

	return 0;
}