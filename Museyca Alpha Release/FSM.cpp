#include <functional>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "FSM.h"
#include "Engine.h"
#include "Button.h"
#include "Utilities.h"
#include <ctime>
using namespace std;
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
#define BGSCROLL 2 // Could these scroll/speed values be handled in the class? Yes. Consider it!
// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::HandleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			Engine::Instance().QuitGame();
			break;
		case SDL_KEYDOWN: // Try SDL_KEYUP instead.
			if (event.key.keysym.sym == SDLK_ESCAPE)
				Engine::Instance().QuitGame();
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button >= 1 && event.button.button <= 3)
				Engine::Instance().SetMouseState(event.button.button - 1, true);
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button >= 1 && event.button.button <= 3)
				Engine::Instance().SetMouseState(event.button.button - 1, false);
			break;
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&Engine::Instance().GetMousePos().x, &Engine::Instance().GetMousePos().y);
			break;
		}
	}
}
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

// Begin TitleState.
TitleState::TitleState() {}

void TitleState::Enter()
{
	cout << "Entering Title..." << endl;
	Mix_HaltMusic();
	ifQuit = false;
	m_vButtons.push_back(new PlayButton("Img/playButton.png", { 0,0,400,100 }, { 312,100,400,100 }));
	// For the bind: what function, what instance, any parameters.
	m_vButtons.push_back(new ExitButton("Img/exitButton.png", { 0,0,400,100 }, { 312,300,400,100 }));
}

void TitleState::Update()
{
	if (!ifQuit) {
		for (int i = 0; i < (int)m_vButtons.size(); i++) {
			m_vButtons[i]->Update();
		}
	}
}

void TitleState::Render()
{
	cout << "Rendering Title..." << endl;
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	//The title page background
	Sprite title = { {0,0,1024,768}, {0,0,1024,768} };
	SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().GetTitleTexture(),
		title.GetSrcP(), title.GetDstP());

	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();
}
void TitleState::HandleEvents()
{
	State::HandleEvents(); // Don't need any events specific to TitleState.
}
void TitleState::Exit()
{
	cout << "Exiting Title..." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	ifQuit = true;
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}
// End TitleState.

//Begin Choose state
ChooseState::ChooseState() {}

void ChooseState::Enter() {
	cout << "Entering Title..." << endl;
	Mix_HaltMusic();
	ifQuit = false;
	m_vButtons.push_back(new Choice1Button("Img/ship1Button.png", { 0,0,100,100 }, { 100,300,100,100 }));
	// For the bind: what function, what instance, any parameters.
	m_vButtons.push_back(new Choice2Button("Img/ship2Button.png", { 0,0,100,100 }, { 462,300,100,100 }));

	m_vButtons.push_back(new Choice3Button("Img/ship3Button.png", { 0,0,100,100 }, { 824,300,100,100 }));
}
void ChooseState::Update() {
	if (!ifQuit) {
		for (int i = 0; i < (int)m_vButtons.size(); i++) {
			m_vButtons[i]->Update();
		}
	}
}
void ChooseState::HandleEvents() {
	State::HandleEvents(); // Don't need any events specific to TitleState.
}
void ChooseState::Render() {
	cout << "Rendering Title..." << endl;
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 0, 0, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	//The title page background
	Sprite choose = { {0,0,1024,768}, {0,0,1024,768} };
	SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().GetChooseTexture(),
		choose.GetSrcP(), choose.GetDstP());

	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();
}
void ChooseState::Exit() {
	cout << "Exiting Title..." << endl;
	ifQuit = true;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}
//End Choose state

// Begin GameState.
GameState::GameState() {}

void GameState::Enter()
{
	cout << "Entering Game..." << endl;
	Engine::Instance().PlayBGM();
	shotEnable = false;
	shotTime = 0;
}

void GameState::HandleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			Engine::Instance().SetRunning(false);
			break;
		case SDL_KEYDOWN: // Try SDL_KEYUP instead.
			if (event.key.keysym.sym == SDLK_ESCAPE)
				Engine::Instance().SetRunning(false);
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_SPACE)
				shotEnable = true;
			break;
		}
	}

	//shot counter, count for frames
	if (shotEnable) {
		switch (Engine::Instance().GetPlayerShip()) {
		case 1:
			shotTime++;
			if (shotTime >= 120) {
				Engine::Instance().SetCanshot(true);
				shotTime = 0;
				shotEnable = false;
			}
			break;
		case 2:
			shotTime++;
			if (shotTime >= 60) {
				Engine::Instance().SetCanshot(true);
				shotTime = 0;
				shotEnable = false;
			}
			break;
		case 3:
			shotTime++;
			if (shotTime >= 30) {
				Engine::Instance().SetCanshot(true);
				shotTime = 0;
				shotEnable = false;
			}
			break;
		default:
			break;
		}
	}

	// Now enemy bullets
	for (int i = 0; i < (int)Engine::Instance().GetVecEmyBullet().size(); i++)
	{
		Engine::Instance().GetVecEmyBullet()[i]->Update();
		if (Engine::Instance().GetVecEmyBullet()[i]->GetDstP()->y > HEIGHT + 10)
		{
			Engine::Instance().CleanSingleEB(i);
		}
	}
	if (Engine::Instance().GetEBNull()) Engine::Instance().CleanEBVec();
	Engine::Instance().CheckCollision();
}

void GameState::Update()
{
	//player's choice
	cout << Engine::Instance().GetPlayerShip() << endl;

	//update
	Engine::Instance().UpdateFrameCounter();
	cout << Engine::Instance().GetFrameCounter() <<endl;

	//key event
	if (Engine::Instance().KeyDown(SDL_SCANCODE_P) == 1)
	{
		Engine::Instance().GetFSM().PushState(new PauseState());
		return;
	}
	else if (Engine::Instance().KeyDown(SDL_SCANCODE_X) == 1)
	{
		Engine::Instance().GetFSM().ChangeState(new TitleState());
		return;
	}

	// Scroll the backgrounds. Check if they need to snap back.
	for (int i = 0; i < 2; i++)
	{
		Engine::Instance().GetBgArray()[i].GetDstP()->y += BGSCROLL;
	}
	if (Engine::Instance().GetBgArray()[1].GetDstP()->y >= 768)
	{
		Engine::Instance().GetBgArray()[0].GetDstP()->y = -768;
		Engine::Instance().GetBgArray()[1].GetDstP()->y = 0;
	}

	Engine::Instance().PlayerAnimation();

	// Enemy animation/movement.
	for (int i = 0; i < (int)Engine::Instance().GetVecEnemies().size(); i++)
	{
		Engine::Instance().GetVecEnemies()[i]->Update(); // Oh, again! We're telling the enemies to update themselves. Good good!
		if (Engine::Instance().GetVecEnemies()[i]->GetDstP()->y > HEIGHT + 100)
		{
			Engine::Instance().CleanSingleEmy(i);
		}
	}
	if (Engine::Instance().GetEmyNull()) Engine::Instance().CleanEmyVec(); // Better to have a logic check (if) than a function call all the time!

	Engine::Instance().UpdateEmySpawn();

	// Update the bullets. Player's first.
	for (int i = 0; i < (int)Engine::Instance().GetVecPlyBullet().size(); i++)
	{
		Engine::Instance().GetVecPlyBullet()[i]->Update();
		if (Engine::Instance().GetVecPlyBullet()[i]->GetDstP()->y < 0)
		{
			Engine::Instance().CleanSinglePB(i);
		}
	}
	if (Engine::Instance().GetPBNull()) Engine::Instance().CleanPBVec();
}

void GameState::Render()
{
	cout << "Rendering Game..." << endl;

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 111, 122, 222, 255);
	SDL_RenderClear(Engine::Instance().GetRenderer()); // Clear the screen with the draw color.

	//background
	for (int i = 0; i < 2; i++) {
		SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().GetBGTexture(),
			Engine::Instance().GetBgArray()[i].GetSrcP(), Engine::Instance().GetBgArray()[i].GetDstP());
	}

	//player sprite
	if (Engine::Instance().GetIfAlive()) {
		switch (Engine::Instance().GetPlayerShip()) {
		case 1:
			SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().GetShip1Texture(),
				Engine::Instance().GetPlayer()->GetSrcP(), Engine::Instance().GetPlayer()->GetDstP(),
				Engine::Instance().GetPlayer()->GetAngle(), &Engine::Instance().GetPivot(), SDL_FLIP_NONE);
			break;
		case 2:
			SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().GetShip2Texture(),
				Engine::Instance().GetPlayer()->GetSrcP(), Engine::Instance().GetPlayer()->GetDstP(),
				Engine::Instance().GetPlayer()->GetAngle(), &Engine::Instance().GetPivot(), SDL_FLIP_NONE);
			break;
		case 3:
			SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().GetShip3Texture(),
				Engine::Instance().GetPlayer()->GetSrcP(), Engine::Instance().GetPlayer()->GetDstP(),
				Engine::Instance().GetPlayer()->GetAngle(), &Engine::Instance().GetPivot(), SDL_FLIP_NONE);
			break;
		default:
			break;
		}
	}
	else {
		Engine::Instance().RenderAndUpdateExplo();
	}

	//player bullet
	for (int i = 0; i < (int)Engine::Instance().GetVecPlyBullet().size(); i++)
	{
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().GetPlayerTexture(),
			Engine::Instance().GetVecPlyBullet()[i]->GetSrcP(), Engine::Instance().GetVecPlyBullet()[i]->GetDstP(),
			90, &Engine::Instance().GetPivot(), SDL_FLIP_NONE);
	}
	////enemy bullet
	for (int i = 0; i < (int)Engine::Instance().GetVecEmyBullet().size(); i++)
	{
		SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().GetPlayerTexture(),
			Engine::Instance().GetVecEmyBullet()[i]->GetSrcP(), Engine::Instance().GetVecEmyBullet()[i]->GetDstP());
	}

	//ememy
	for (int i = 0; i < (int)Engine::Instance().GetVecEnemies().size(); i++)
	{
		SDL_RenderCopyEx(Engine::Instance().GetRenderer(), Engine::Instance().GetEmy1Texture(),
			Engine::Instance().GetVecEnemies()[i]->GetSrcP(), Engine::Instance().GetVecEnemies()[i]->GetDstP(),
			-90, &Engine::Instance().GetPivot(), SDL_FLIP_NONE);
	}

	SDL_RenderPresent(Engine::Instance().GetRenderer());
	State::Render();
}

void GameState::Exit()
{
	cout << "Exiting Game..." << endl;
}

void GameState::Resume() { cout << "Resuming Game..." << endl; }
// End GameState.

// Begin PauseState.
PauseState::PauseState() {}

void PauseState::Enter()
{
	cout << "Entering Pause..." << endl;
	m_vButtons.push_back(new ResumeButton("Img/resumeButton.png", { 0,0,400,100 }, { 312,200,400,100 }));
	// This exit button has a different size but SAME function as the one in title.
	m_vButtons.push_back(new ExitButton("Img/exitButton.png", { 0,0,400,100 }, { 312,400,400,100 }));
	Mix_HaltChannel(-1);
}
void PauseState::HandleEvents()
{
	State::HandleEvents();
}

void PauseState::Update()
{
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Update();
}

void PauseState::Render()
{
	cout << "Rendering Pause..." << endl;
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 88, 88, 88, 8);
	//SDL_Rect rect = { 256, 128, 512, 512 }; //window size
	SDL_Rect rect = { 0, 0, 1024, 768 };
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();
}

void PauseState::Exit()
{
	cout << "Exiting Pause..." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}

void PauseState::Resume()
{
	cout << "Exiting Pause..." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
}
// End PauseState.

// Begin LoseState.
LoseState::LoseState() {}

void LoseState::Enter()
{
	cout << "Entering Lose..." << endl;
	ifQuit = false;
	m_vButtons.push_back(new RestartButton("Img/restartButton.png", { 0,0,400,100 }, { 312,200,400,100 }));
	// This exit button has a different size but SAME function as the one in title.
	m_vButtons.push_back(new ExitButton("Img/exitButton.png", { 0,0,400,100 }, { 312,400,400,100 }));
	Mix_HaltChannel(-1);
}

void LoseState::Update()
{
	if (!ifQuit) {
		for (int i = 0; i < (int)m_vButtons.size(); i++) {
			m_vButtons[i]->Update();
		}
	}
}

void LoseState::HandleEvents()
{
	State::HandleEvents();
}

void LoseState::Render()
{
	cout << "Rendering Lose..." << endl;
	SDL_SetRenderDrawBlendMode(Engine::Instance().GetRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 16, 32, 64, 64);

	Sprite lose = { {0, 0, 512, 512}, {256, 128, 512, 512} };
	SDL_RenderCopy(Engine::Instance().GetRenderer(), Engine::Instance().GetLoseTexture(),
		lose.GetSrcP(), lose.GetDstP());


	//SDL_Rect rect = { 256, 128, 512, 512 };
	//SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	for (int i = 0; i < (int)m_vButtons.size(); i++)
		m_vButtons[i]->Render();
	State::Render();
}

void LoseState::Exit()
{
	cout << "Exiting Lose..." << endl;
	for (int i = 0; i < (int)m_vButtons.size(); i++)
	{
		delete m_vButtons[i];
		m_vButtons[i] = nullptr;
	}
	ifQuit = true;
	m_vButtons.clear();
	m_vButtons.shrink_to_fit();
	Engine::Instance().Reset();
}
// End PauseState.


// Begin StateMachine.
void FSM::Update()
{
	if (!m_vStates.empty()) // empty() and back() are methods of the vector type.
		m_vStates.back()->Update();
}

void FSM::Render()
{
	if (!m_vStates.empty())
		m_vStates.back()->Render();
}
void FSM::HandleEvents()
{
	if (!m_vStates.empty())
		m_vStates.back()->HandleEvents();
}
void FSM::ChangeState(State* pState)
{
	if (!m_vStates.empty())
	{
		m_vStates.back()->Exit();
		delete m_vStates.back();	// De-allocating the state in the heap.
		m_vStates.back() = nullptr; // Nullifying pointer to the de-allocated state.
		m_vStates.pop_back();		// Removes the now-null pointer from the vector.
	}
	PushState(pState); // Invokes method below.
}

void FSM::PushState(State* pState)
{
	m_vStates.push_back(pState);
	m_vStates.back()->Enter();
}

void FSM::PopState()
{
	if (!m_vStates.empty())
	{
		m_vStates.back()->Exit();
		delete m_vStates.back();
		m_vStates.back() = nullptr;
		m_vStates.pop_back();
	}
	m_vStates.back()->Resume();
}

void FSM::Clean()
{
	while (!m_vStates.empty()) // Because we can exit the game in the pause state with the window's 'X'.
	{						   // Ensures that ALL states left in the vector are cleaned up.
		m_vStates.back()->Exit();
		delete m_vStates.back();
		m_vStates.back() = nullptr;
		m_vStates.pop_back();
	}
}

vector<State*>& FSM::GetStates() { return m_vStates; }
// End StateMachine.