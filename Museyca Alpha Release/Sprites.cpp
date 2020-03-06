#include <algorithm> // For min/max.
#include <cmath>	 // For cos/sin.
#include "Sprites.h"
#include "Engine.h"
using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define FPS 60

Sprite::Sprite() {}
Sprite::Sprite(SDL_Rect s, SDL_Rect d) :m_rSrc(s), m_rDst(d) {}
SDL_Rect* Sprite::GetSrcP() { return &m_rSrc; }
SDL_Rect* Sprite::GetDstP() { return &m_rDst; }
double Sprite::GetAngle() { return m_angle; }


AnimatedSprite::AnimatedSprite(int a, int fm, int sm, SDL_Rect s, SDL_Rect d) :
	Sprite(s, d), m_iFrameMax(fm), m_iSpriteMax(sm)
{
	// Added this here (and not in header line) just because. Note: initializers ARE resolved before body statements.
	m_angle = a;
	m_iSprite = m_iFrame = 0; // Chaining assignments to the same value.
}

void AnimatedSprite::Animate()
{
	m_iFrame++;
	if (m_iFrame == m_iFrameMax)
	{
		m_iFrame = 0;
		m_iSprite++;
		if (m_iSprite == m_iSpriteMax)
		{
			m_iSprite = 0;
		}
	}
	m_rSrc.x = m_rSrc.w * m_iSprite;
}

Player::Player(SDL_Rect s, SDL_Rect d) : AnimatedSprite(0, 8, 4, s, d) {}

void Player::Render() {
}
void Player::Update() {
}

Bullet::Bullet(SDL_Rect s, SDL_Rect d, int spd) :
	Sprite(s, d), m_speed(spd), m_active(true) {}


void Bullet::Update()
{
	m_rDst.y -= m_speed;
}

Enemy::Enemy(SDL_Rect s, SDL_Rect d, vector<Bullet*>* bVec, Mix_Chunk* c, int fr) : AnimatedSprite(-90, 4, 4, s, d),
m_pBulletVec(bVec), m_pPew(c), m_bulletTimer(0), m_timerMax(fr) {}

void Enemy::Update()
{
	//Animate();
	m_rDst.y += 3;
	if (Engine::Instance().GetFrameCounter() >= FPS * 60 / 132)
	{
		m_pBulletVec->push_back(new Bullet( {160,100,14,14}, {m_rDst.x + 42,m_rDst.y,14,14}, -10 ));
		Mix_PlayChannel(-1, m_pPew, 0);
	}
	if (Engine::Instance().GetFrameCounter() > FPS * 60 / 133)
	{
		Engine::Instance().ResetFrameCounter();
	}
}




