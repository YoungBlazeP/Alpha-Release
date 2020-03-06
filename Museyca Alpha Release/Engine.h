#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include "Sprites.h"
#include "FSM.h"

class Engine
{
// I am avoiding in-class initialization.
private: // Private properties.
	bool m_bRunning, // Loop control flag.
		m_bENull, // These three flags check if we need to clear the respective vectors of nullptrs.
		m_bPBNull,
		m_bEBNull,
		m_bCanShoot, // This restricts the player from firing again unless they release the Spacebar.
		m_bDead;
	const Uint8* m_iKeystates; // Keyboard state container.
	Uint32 m_start, m_end, m_delta, m_fps; // Fixed timestep variables.
	SDL_Window* m_pWindow; // This represents the SDL window.
	SDL_Renderer* m_pRenderer; // This represents the buffer to draw to.

	SDL_Texture* m_pBGText; // For the bg.
	Sprite bgArray[2];

	SDL_Rect explosion_src;
	SDL_Rect emyExplo_dst;
	SDL_Texture* m_pSprText; // For the sprites.
	SDL_Texture* m_pTitleText; // for the title page.
	SDL_Texture* m_pChooseText;
	SDL_Texture* m_pLoseText; //for the lose page.
	SDL_Texture* m_pExploS1Text;//explosion animation
	SDL_Texture* m_pExploS2Text;//explosion animation
	SDL_Texture* m_pExploS3Text;//explosion animation

	SDL_Texture* m_pExploEmy1Text;//explosion animation

	SDL_Texture* m_pShip1Text;
	SDL_Texture* m_pShip2Text;
	SDL_Texture* m_pShip3Text;

	SDL_Texture* m_pEmy1Text;

	Player* m_player;

	int deathCounter = 0;

	int playerShip;//the player's choice for the ship, input from the choose state

	int m_iESpawn, // The enemy spawn frame timer properties.
		m_iESpawnMax;
	vector<Enemy*> m_vEnemies;
	vector<Bullet*> m_vPBullets;
	vector<Bullet*> m_vEBullets;
	SDL_Point m_pivot;

	Mix_Music* m_pMusic;
	vector<Mix_Chunk*> m_vSounds;

	int m_frameCounter = 0;
	FSM* m_pFSM; // Pointer to the StateMachine object created dynamically.
	SDL_Point m_MousePos;
	bool m_MouseState[3] = { 0,0,0 }; // Button up/down. Left, Middle, Right.


private: // Private methods.
	bool Init(const char* title, int xpos, int ypos, int width, int height, int flags);
	void Wake();
	void Sleep();
	void HandleEvents();
	
	void Update();
	void Render();
	void Clean();
public: // Public methods.
	Engine();
	~Engine();
	int Run();
	static Engine& Instance(); // This static method creates the static instance that can be accessed 'globally'
	
	void CheckCollision();

	bool KeyDown(SDL_Scancode c);
	SDL_Renderer* GetRenderer();
	SDL_Texture* GetBGTexture();
	SDL_Texture* GetPlayerTexture();
	SDL_Texture* GetTitleTexture();
	SDL_Texture* GetChooseTexture();
	SDL_Texture* GetLoseTexture();

	SDL_Texture* GetShip1Texture();
	SDL_Texture* GetShip2Texture();
	SDL_Texture* GetShip3Texture();

	SDL_Texture* GetEmy1Texture();

	SDL_Point GetPivot();
	Player* GetPlayer();
	Sprite* GetBgArray();
	vector<Enemy*> GetVecEnemies();
	vector<Bullet*> GetVecPlyBullet();
	vector<Bullet*> GetVecEmyBullet();;
	FSM& GetFSM();
	SDL_Point& GetMousePos();
	bool GetMouseState(int idx);
	void SetMouseState(int idx, bool b);

	int GetFrameCounter();
	void UpdateFrameCounter();
	void ResetFrameCounter();
	
	bool GetIfAlive();

	bool GetEmyNull();
	void SetEmyNull(bool emyNull);

	bool GetEBNull();
	void SetEBNull(bool eBltNull);

	bool GetPBNull();
	void SetPBNull(bool pBltNull);

	bool GetRunning();
	void SetRunning(bool ifRunning);

	bool GetCanshot();
	void SetCanshot(bool canshot);

	void QuitGame();

	void CleanSingleEB(int n);
	void CleanEBVec();
	void CleanSinglePB(int n);
	void CleanPBVec();
	void CleanSingleEmy(int n);
	void CleanEmyVec();

	void UpdateEmySpawn();
	void PlayerAnimation();
	void PlayBGM();
	void Reset();

	void PlayButtonSound();
	void PlayButtonClick();
	void RenderAndUpdateExplo();
	void RenderEmyExplo();
	void SetPlayerShip(int n);
	int GetPlayerShip();
	Mix_Chunk* GetSound(int i) { return m_vSounds[i]; }
};