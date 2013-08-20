#pragma once

namespace utility
{
  class timer
  {
    struct base_functor
    {
      virtual ~base_functor() {}
      virtual void call() = 0;
    };

    template<class Callee>
    struct mem_functor : public base_functor
    {
      typedef void (Callee::*callback_type)();

    public:
      mem_functor(Callee* callee, callback_type cb)
        : m_cb(cb), m_callee(callee) {}

      virtual void call();

    private:
      Callee* m_callee;
      callback_type m_cb;
    };

    struct functor : public base_functor
    {
    public:
      typedef void (*callback_type)();

      functor(callback_type cb);
      virtual void call();

    private:
      callback_type m_cb;
    };


  public:
    template <typename Callee>
    static timer* create(int time,Callee* callee, void (Callee::*func)());
    static timer* create(int time, void (*func)());
    static timer* create(int time);

    ~timer();

    void update();

  private:
    timer(int time, base_functor* callback = 0);

  private:
    int m_total_time;
    int m_cur_time;
    int m_start_time;

    bool m_is_time_over;

    base_functor* m_callback;
  };

  template <typename Callee>
  timer* timer::create(int time,Callee* callee, void (Callee::*func)())
  {
    return new timer(time, new mem_functor<Callee> (callee, func));
  }

  template<class Callee>
  void timer::mem_functor<Callee>::call()
  {
    if (m_cb && m_callee)
      (m_callee->*m_cb)();
  }

  namespace test
  {
    void timer_test();
  }
}