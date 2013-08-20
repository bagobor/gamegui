#pragma once

namespace core
{
  struct call_helper
  {
	  template <typename Callee>
	  static int Call(Callee* callee, int (Callee::*func)())
	  {
		  return (callee->*func)();
	  }

	  template <typename Callee, typename P1>
	  static int Call(Callee* callee, int (Callee::*func)(P1), P1 p1)
	  {		
		  return  (callee->*func)( p1 );
	  }

	  template <typename Callee, typename P1, typename P2>
	  static int Call(Callee* callee, int (Callee::*func)(P1, P2), P1 p1, P2 p2 )
	  {
		  return (callee->*func)( p1,p2 );
	  }

	  template <typename Callee, typename P1, typename P2, typename P3>
	  static int Call(Callee* callee, int (Callee::*func)(P1, P2, P3),P1 p1,P2 p2,P3 p3)
	  {
		  return (callee->*func)( p1,p2,p3 );
	  }

	  template <typename Callee, typename P1, typename P2, typename P3,typename P4>
	  static int Call(Callee* callee, int (Callee::*func)(P1, P2, P3, P4), P1 p1,P2 p2,P3 p3, P4 p4)
	  {
		  return (callee->*func)( p1,p2,p3,p4 );
	  }
  };
}