#include "Engine.h"
#include "Utilities.h"
#include <iostream>
#include <ctime>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
#define BGSCROLL 2 // Could these scroll/speed values be handled in the class? Yes. Consider it!
#define PSPEED 4
using namespace std;

Engine::Engine() :m_iESpawn(0), m_iESpawnMax(120), m_pivot({ 0,0 })
{	// Again showing a mix of either initializers or in-body initialization. Initializers happen first.
	m_bRunning = m_bEBNull = m_bENull = m_bPBNull = m_bDead = false; // Setting all to false.
	m_bCanShoot = true;
	cout << "Engine class constructed!" << endl;
}
Engine::~Engine() { delete m_pFSM; }

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game." << endl;
	srand((unsigned)time(NULL));
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				m_pBGText = IMG_LoadTexture(m_pRenderer, "Img/background.png");
				m_pSprText = IMG_LoadTexture(m_pRenderer, "Img/sprites.png");
				m_pTitleText = IMG_LoadTexture(m_pRenderer, "Img/titlepage.jpg");
				m_pChooseText = IMG_LoadTexture(m_pRenderer, "Img/choosePage.jpg");
				m_pLoseText = IMG_LoadTexture(m_pRenderer, "Img/losepage.jpg");
				m_pExploS1Text = IMG_LoadTexture(m_pRenderer, "Img/explosionShip1.png");
				m_pExploS2Text = IMG_LoadTexture(m_pRenderer, "Img/explosionShip2.png");
				m_pExploS3Text = IMG_LoadTexture(m_pRenderer, "Img/explosionShip3.png");
				m_pShip1Text = IMG_LoadTexture(m_pRenderer, "Img/ship1.png");
				m_pShip2Text = IMG_LoadTexture(m_pRenderer, "Img/ship2.png");
				m_pShip3Text = IMG_LoadTexture(m_pRenderer, "Img/ship3.png");
				m_pEmy1Text = IMG_LoadTexture(m_pRenderer, "Img/enemy1.png");
				m_pExploEmy1Text = IMG_LoadTexture(m_pRenderer, "Img/explosionEmy1.png");
				if (Mix_Init(MIX_INIT_MP3) != 0) // Mixer init success.
				{
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048); // Good for most games.
					Mix_AllocateChannels(16);
					m_pMusic = Mix_LoadMUS("Aud/Level1_soundtrack.mp3"); // Load the music track.
					// Load the chunks into the Mix_Chunk vector.
					m_vSounds.reserve(5); // Optional but good practice.
					m_vSounds.push_back(Mix_LoadWAV("Aud/stone.wav"));
					m_vSounds.push_back(Mix_LoadWAV("Aud/laser.wav"));
					m_vSounds.push_back(Mix_LoadWAV("Aud/explode.wav"));
					m_vSounds.push_back(Mix_LoadWAV("Aud/Menu_button_click_1.wav"));
					m_vSounds.push_back(Mix_LoadWAV("Aud/Menu_transition.wav"));
					/* By the way, you should check to see if any of these loads are failing and
					   you can use Mix_GetError() to print out the error message. Wavs can be finicky.*/
				}
				else return false;
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	m_iKeystates = SDL_GetKeyboardState(nullptr);
	playerShip = 0;
	explosion_src = { 0, 0, 100, 100 };
	emyExplo_dst = { 0, 0, 100, 100 };
	bgArray[0] = { {0,0,1024,768}, {0, -768, 1024, 768} };
	bgArray[1] = { {0,0,1024,768}, {0, 0, 1024, 768} };
	m_player = new Player({ 0,0,100,100 }, { WIDTH / 2 - 50, HEIGHT - 128, 100, 100 });
	m_pFSM = new FSM(); // Creates the state machine object/instance.
	m_pFSM->ChangeState(new TitleState()); // Invoking the ChangeState method to set the initial state, Title.

	Mix_VolumeMusic(16); // 0-MIX_MAX_VOLUME (128).
	m_bRunning = true; // Everything is okay, start the engine.
	cout << "Success!" << endl;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	GetFSM().HandleEvents();
}

// Keyboard utility function.
bool Engine::KeyDown(SDL_Scancode c)
{
	if (m_iKeystates != nullptr)
	{
		if (m_iKeystates[c] == 1)
			return true;
		else
			return false;
	}
	return false;
}

void Engine::CheckCollision()
{
	//give the game 10 frames before it changes state
	if (m_bDead) {
		deathCounter++;
		cout << deathCounter << endl;
	}

	if (deathCounter == 36) {
		deathCounter = 0;
		m_bDead = false;
		Engine::Instance().GetFSM().ChangeState(new LoseState());
	}

	// Player vs. Enemy.
	SDL_Rect p = { m_player->GetDstP()->x, m_player->GetDstP()->y, 100, 100 };
	for (int i = 0; i < (int)m_vEnemies.size(); i++)
	{
		SDL_Rect e = { m_vEnemies[i]->GetDstP()->x, m_vEnemies[i]->GetDstP()->y - 100, 100, 100 };
		if (SDL_HasIntersection(&p, &e))
		{
			//set the player state to dead
			if (!m_bDead) {
				m_bDead = true;
			}
			// Game over!
			cout << "Player goes boom!" << endl;
			Mix_PlayChannel(-1, m_vSounds[2], 0);
			break;
		}
	}
	// Player bullets vs. Enemies.
	for (int i = 0; i < (int)m_vPBullets.size(); i++)
	{
		SDL_Rect b = { m_vPBullets[i]->GetDstP()->x - 100, m_vPBullets[i]->GetDstP()->y, 100, 10 };
		for (int j = 0; j < (int)m_vEnemies.size(); j++)
		{
			if (m_vEnemies[j] == nullptr) continue;
			SDL_Rect e = { m_vEnemies[j]->GetDstP()->x, m_vEnemies[j]->GetDstP()->y - 100, 100, 100 };
			if (SDL_HasIntersection(&b, &e))
			{
				Mix_PlayChannel(-1, m_vSounds[2], 0);
				//emyExplo_dst = m_vEnemies[j]->GetDstP;
				delete m_vEnemies[j];
				m_vEnemies[j] = nullptr;
				delete m_vPBullets[i];
				m_vPBullets[i] = nullptr;
				m_bENull = true;
				m_bPBNull = true;
				break;
			}
		}
	}
	if (m_bENull) CleanVector<Enemy*>(m_vEnemies, m_bENull);
	if (m_bPBNull) CleanVector<Bullet*>(m_vPBullets, m_bPBNull);
	// Enemy bullets vs. player.
	for (int i = 0; i < (int)m_vEBullets.size(); i++)
	{
		if (SDL_HasIntersection(&p, m_vEBullets[i]->GetDstP()))
		{
			// Game over!
			cout << "Player goes boom!" << endl;
			Mix_PlayChannel(-1, m_vSounds[2], 0);
			delete m_vEBullets[i];
			m_vEBullets[i] = nullptr;
			CleanVector<Bullet*>(m_vEBullets, m_bEBNull);
			//set the player state to dead
			if (!m_bDead) {
				m_bDead = true;
			}
			break;
		}
	}
}

/* Update */
void Engine::Update()
{
	GetFSM().Update(); // Invokes the update of the state machine.
}

/* render. */
void Engine::Render()
{
	GetFSM().Render();
}

void Engine::Clean()
{
	cout << "Cleaning game." << endl;
	delete m_player;
	m_player = nullptr;
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	for (int i = 0; i < (int)m_vSounds.size(); i++)
		Mix_FreeChunk(m_vSounds[i]);
	m_vSounds.clear();
	Mix_FreeMusic(m_pMusic);
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

int Engine::Run()
{
	if (Init("Museyca", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (m_bRunning)
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_bRunning)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; // C++11 will prevent this line from running more than once. Magic statics.
	return instance;
}

SDL_Renderer* Engine::GetRenderer() { return m_pRenderer; }

SDL_Texture* Engine::GetBGTexture() { return m_pBGText; }

SDL_Texture* Engine::GetTitleTexture() { return m_pTitleText; }

SDL_Texture* Engine::GetPlayerTexture() { return m_pSprText; }

SDL_Texture* Engine::GetChooseTexture() { return m_pChooseText; }

SDL_Texture* Engine::GetLoseTexture() { return m_pLoseText; }

SDL_Texture* Engine::GetShip1Texture() { return m_pShip1Text; }

SDL_Texture* Engine::GetShip2Texture() { return m_pShip2Text; }

SDL_Texture* Engine::GetShip3Texture() { return m_pShip3Text; }

SDL_Texture* Engine::GetEmy1Texture() { return m_pEmy1Text; }

SDL_Point Engine::GetPivot() { return m_pivot; }

FSM& Engine::GetFSM() { return *m_pFSM; }

SDL_Point& Engine::GetMousePos() { return m_MousePos; }

bool Engine::GetMouseState(int idx) { return m_MouseState[idx]; }

void Engine::SetMouseState(int idx, bool b) { m_MouseState[idx] = b; }

void Engine::QuitGame() { m_bRunning = false; }

Player* Engine::GetPlayer() { return m_player; }

Sprite* Engine::GetBgArray() { return bgArray; }


//getter for vectors
vector<Enemy*> Engine::GetVecEnemies() { return m_vEnemies; }
vector<Bullet*> Engine::GetVecPlyBullet() { return m_vPBullets; }
vector<Bullet*> Engine::GetVecEmyBullet() { return m_vEBullets; }

//getter and setters for booleans
//enemy
bool Engine::GetEmyNull() { return m_bENull; }
void Engine::SetEmyNull(bool emyNull) { m_bENull = emyNull; }

//enemy bullet
bool Engine::GetEBNull() { return m_bEBNull; }
void Engine::SetEBNull(bool eBltNull) { m_bEBNull = eBltNull; }

//player bullet
bool Engine::GetPBNull() { return m_bPBNull; }
void Engine::SetPBNull(bool pBltNull) { m_bPBNull = pBltNull; }

bool Engine::GetRunning() { return m_bRunning; }
void Engine::SetRunning(bool ifRunning) { m_bRunning = ifRunning; }

bool Engine::GetCanshot() { return m_bCanShoot; }
void Engine::SetCanshot(bool canshot) { m_bCanShoot = canshot; }


//cleaning methods
//enemy bullet
void Engine::CleanSingleEB(int n) {
	delete m_vEBullets[n];
	m_vEBullets[n] = nullptr;
	m_bEBNull = true;
}
void Engine::CleanEBVec() { CleanVector<Bullet*>(m_vEBullets, m_bEBNull); }

//player bullet
void Engine::CleanSinglePB(int n) {
	delete m_vPBullets[n];
	m_vPBullets[n] = nullptr;
	m_bPBNull = true;
}
void Engine::CleanPBVec() { CleanVector<Bullet*>(m_vPBullets, m_bPBNull); }

//enemy
void Engine::CleanSingleEmy(int n) {
	delete m_vEnemies[n];
	m_vEnemies[n] = nullptr;
	m_bENull = true;
}
void Engine::CleanEmyVec() { CleanVector<Enemy*>(m_vEnemies, m_bENull); }


void Engine::UpdateEmySpawn() {
	// Update enemy spawns.
	if (m_iESpawn++ == m_iESpawnMax)
	{
		m_vEnemies.push_back(new Enemy( { 0,0,100,100 }, { 100 + rand() % (WIDTH - 200),0,100,100 }, &m_vEBullets, m_vSounds[0],
			30 + rand() % 91)); // Randomizing enemy bullet spawn to every 30-120 frames.
		m_iESpawn = 0;
	}
}

void Engine::PlayerAnimation() {

	// Player animation/movement.
	//m_player->Animate(); // Oh! We're telling the player to animate itself. This is good! Hint hint.
	double shipFactor;
	switch (playerShip) {
	case 1:
		shipFactor = 1;
		break;
	case 2:
		shipFactor = 2;
		break;
	case 3:
		shipFactor = 3;
		break;
	default:
		shipFactor = 0;
		break;
	}

	if (KeyDown(SDL_SCANCODE_A) && m_player->GetDstP()->x > 0)
		m_player->GetDstP()->x -= PSPEED * shipFactor;
	else if (KeyDown(SDL_SCANCODE_D) && m_player->GetDstP()->x < WIDTH - 100)
		m_player->GetDstP()->x += PSPEED * shipFactor;
	if (KeyDown(SDL_SCANCODE_W) && m_player->GetDstP()->y > HEIGHT / 2)
		m_player->GetDstP()->y -= PSPEED * shipFactor;
	else if (KeyDown(SDL_SCANCODE_S) && m_player->GetDstP()->y < HEIGHT - m_player->GetDstP()->w)
		m_player->GetDstP()->y += PSPEED * shipFactor;
	if (KeyDown(SDL_SCANCODE_SPACE) && m_bCanShoot)
	{
		m_bCanShoot = false;
		m_vPBullets.push_back(new Bullet({ 376,0,10,100 }, { m_player->GetDstP()->x + 100,m_player->GetDstP()->y,10,100 }, 30));
		Mix_PlayChannel(-1, m_vSounds[1], 0);
	}
}

void Engine::PlayBGM() { Mix_PlayMusic(m_pMusic, -1); }

//the reset funciton that resets everything in the game
void Engine::Reset() {
	playerShip = 0;
	bgArray[0] = { {0,0,1024,768}, {0, -768, 1024, 768} };
	bgArray[1] = { {0,0,1024,768}, {0, 0, 1024, 768} };
	explosion_src = { 0, 0, 100, 100 };
	m_player = new Player({ 0,0,100,100 }, { WIDTH / 2 - 50, HEIGHT - 128, 100, 100 });
	m_bEBNull = m_bENull = m_bPBNull = m_bDead = false; // Setting all to false.
	m_bCanShoot = true;
	m_iESpawn = 0;
	m_iESpawnMax = 120; 
	m_pivot = { 0,0 };

	for (int i = 0; i < (int)GetVecEmyBullet().size(); i++)
	{
		CleanSingleEB(i);
	}
	CleanEBVec();
	for (int i = 0; i < (int)GetVecPlyBullet().size(); i++)
	{
		CleanSinglePB(i);
	}
	CleanPBVec();
	for (int i = 0; i < (int)GetVecEnemies().size(); i++)
	{
		CleanSingleEmy(i);
	}
	CleanEmyVec();
}

int  Engine::GetFrameCounter() { return m_frameCounter; }

void Engine::UpdateFrameCounter() { m_frameCounter++; }

void Engine::ResetFrameCounter() { m_frameCounter = -2; }

void Engine::SetPlayerShip(int n) { playerShip = n; }

int Engine::GetPlayerShip() { return playerShip; }

bool Engine::GetIfAlive() { return !m_bDead; }

void Engine::RenderAndUpdateExplo() {
	//explosion_src.x = explosion_src.w * static_cast<int>((SDL_GetTicks() / FPS) % 11);
	explosion_src.x = explosion_src.w * (deathCounter / 3);
	switch (GetPlayerShip()) {
	case 1:
		SDL_RenderCopy(m_pRenderer, m_pExploS1Text, &explosion_src, GetPlayer()->GetDstP());
		break;
	case 2:
		SDL_RenderCopy(m_pRenderer, m_pExploS2Text, &explosion_src, GetPlayer()->GetDstP());
		break;
	case 3:
		SDL_RenderCopy(m_pRenderer, m_pExploS3Text, &explosion_src, GetPlayer()->GetDstP());
		break;
	}
}

void Engine::PlayButtonSound() { Mix_PlayChannel(-1, m_vSounds[3], 0); }

void Engine::PlayButtonClick() { Mix_PlayChannel(-1, m_vSounds[4], 0); }

void Engine::RenderEmyExplo() { }