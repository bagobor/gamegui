#include "stdafx.h"
#include "game.h"

namespace  game
{
//////////////////////////////////////////////////////////////////////////
  BaseState::BaseState(const Game& s ) : m_system(s) 
  {
  }

  BaseState::~BaseState() 
  {

  }

  // return value indicates break or not update chain.
  bool BaseState::update() 
  {
    return true;
  }

  const Game& BaseState::getGame() const 
  {
    return m_system;
  }
//////////////////////////////////////////////////////////////////////////

  Game::Game()
  {

  }

  Game::~Game()
  {
    while(BaseState* top_state = getTopState())
    {
      popState();
    }
  }

  void Game::popState()
  {
    BaseState* top_state = getTopState() ;
    if (top_state)
    {
      m_states.pop();
      delete top_state;
    }
  }

  BaseState* Game::getTopState() 
  {
    return m_states.empty() ? 0 :m_states.top();
  }
//////////////////////////////////////////////////////////////////////////
  namespace states
  {
    Intro::Intro(const Game& s ) : BaseState(s) 
    {
    }

    Login::Login(const Game& s ) : BaseState(s) 
    {
    }

    PlayerSelection::PlayerSelection(const Game& s ) : BaseState(s) 
    {
    }

    ServerSelection::ServerSelection(const Game& s ) : BaseState(s) 
    {
    }

    TeamSelection::TeamSelection(const Game& s ) : BaseState(s) 
    {
    }
  }
}