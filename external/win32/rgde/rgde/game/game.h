#pragma once

#include <stack>


namespace game
{
  class Game;

  class BaseState
  {
  public:
    BaseState(const Game& s );
    virtual ~BaseState();

    // return value indicates break or not update chain.
    virtual bool update();

    const Game& getGame() const;

  protected:
    const Game& m_system;
  };

  class Game
  {
    typedef std::stack<BaseState*> GameStates;
  public:
    Game();
    ~Game();

    template<class T>
    void pushState()
    {
      BaseState *new_state = new T(*this);
      m_states.push(new_state);
    }

    void popState();

    template<class T>
    void setState()
    {
      popState();
      pushState<T>();
    }

    BaseState* getTopState();

  private:
    GameStates m_states;
  };

  namespace states
  {
    class Intro : public BaseState
    {
    public:
      Intro(const Game& s );
    };

    class Login : public BaseState
    {
    public:
      Login(const Game& s );
    };

    class PlayerSelection : public BaseState
    {
    public:
      PlayerSelection(const Game& s );
    };

    class ServerSelection : public BaseState
    {
    public:
      ServerSelection(const Game& s );
    };

    class TeamSelection : public BaseState
    {
    public:
      TeamSelection(const Game& s );
    };
  }
}