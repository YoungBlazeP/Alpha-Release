#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "Button.h"
#include "Sprites.h"
#include "Utilities.h"
using namespace std;

class State // This is the abstract base class for all specific states.
{
protected:
	State() {}
public:
	virtual void Enter() = 0;
	virtual void HandleEvents();
	virtual void Update() = 0;
	virtual void Render();
	virtual void Exit() = 0;
	virtual void Resume();
};

class TitleState : public State
{
private:
	bool ifQuit;
	vector<Button*> m_vButtons;
public:
	TitleState();
	void Enter();
	void HandleEvents();
	void Update();
	void Render();
	void Exit();
};

class ChooseState : public State
{
private:
	bool ifQuit;
	vector<Button*> m_vButtons;
public:
	ChooseState();
	void Enter();
	void Update();
	void HandleEvents();
	void Render();
	void Exit();
};

class GameState : public State
{
private:
	bool shotEnable;
	int shotTime;
public:
	GameState();
	void Enter();
	void Update();
	void HandleEvents();
	void Render();
	void Exit();
	void Resume();
};

class PauseState : public State
{
private:
	vector<Button*> m_vButtons;
public:
	PauseState();
	void Enter();
	void Update();
	void Render();
	void HandleEvents();
	void Exit();
	void Resume();
};

class LoseState : public State
{
private:
	bool ifQuit;
	vector<Button*> m_vButtons;
public:
	LoseState();
	void Enter();
	void Update();
	void Render();
	void HandleEvents();
	void Exit();
};

class FSM
{
private:
	vector<State*> m_vStates;
public:
	void Update();
	void Render();
	void HandleEvents();
	void ChangeState(State* pState); // Normal state change.
	void PushState(State* pState); // GameState to PauseState.
	void PopState(); // PauseState to GameState.
	void Clean();
	vector<State*>& GetStates();
};

