#include "stdafx.h"

#include "timer.h"

#include <string>
#include <iostream>


namespace utility
{
  namespace test
  {
    struct timer_holder
    {
      timer_holder()
      {
        m_timer1 = timer::create(3000, this, &timer_holder::time_over);
        m_timer2 = timer::create(6000, &timer_holder::time_over2);

        m_timer1_done = false;
      }

      ~timer_holder()
      {
        delete m_timer1;
        delete m_timer2;
      }

      void time_over()
      {
        m_timer1_done = true;
        std::cout << "timer1 done\n" ;
      }

      static void time_over2()
      {
        m_timer2_done = true;
        std::cout << "timer2 done\n" ;
      }

      void update()
      {
        m_timer1->update();
        m_timer2->update();
      }

      bool m_timer1_done;
      static bool m_timer2_done;

      timer* m_timer1;
      timer* m_timer2;
    };

    bool timer_holder::m_timer2_done = false;

    void timer_test()
    {

      timer_holder tester;

      while(!(tester.m_timer1_done && tester.m_timer2_done))
      {
        tester.update();
      }
    }
  }
}