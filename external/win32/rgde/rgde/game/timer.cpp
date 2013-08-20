#include "stdafx.h"
#include "timer.h"

int get_time()
{
  static int time = 0;
  return time++;
}

namespace utility
{

  timer::functor::functor(callback_type cb)
    : m_cb(cb)
  {
  }

  void timer::functor::call()
  {
    if (m_cb)
      m_cb();
  }

  timer* timer::create(int time, void (*func)())
  {
    return new timer(time, new functor(func));
  }

  timer* timer::create(int time)
  {
    return new timer(time, 0);
  }

  timer::timer(int time, timer::base_functor* callback)
    : m_callback(callback)
    , m_total_time(time)
    , m_start_time(get_time())
    , m_is_time_over(false)
  {
    m_cur_time = m_start_time;
  }

  timer::~timer()
  {
    if (m_callback)
      delete m_callback;
  }

  void timer::update()
  {
    if (m_is_time_over)
      return;

    m_cur_time = get_time();

    m_is_time_over = m_cur_time - m_start_time >= m_total_time;

    if (m_is_time_over && m_callback)
      m_callback->call();
  }
}