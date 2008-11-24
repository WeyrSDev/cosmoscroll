#include "PlayerShip.hpp"
#include "MediaManager.hpp"
#include "Window.hpp"
#include "ParticleSystem.hpp"
#include "Math.hpp"
#include "Settings.hpp"

#include <SFML/System.hpp>
#include <cassert>
#include <typeinfo>

#ifdef DEBUG
#include <iostream>
#endif

#define DEFAULT_SPEED	  200
#define GUN_OFFSET	  sf::Vector2f(52, 24)

// taux de regénération du bouclier en boules/secondes
#define SHIELD_RECOVERY_RATE 0.3
#define SHIELD_MAX		   6
#define SHIELD_DEFAULT	   3

#define COOLER_MAX		   3
#define COOLER_DEFAULT	   0

#define TIMED_BONUS_DURATION 10

#define HEAT_MAX	 100
#define COLDING_RATE 13

#define HP_DEFAULT   3
#define HP_MAX	   5


PlayerShip::PlayerShip(const sf::Vector2f& offset, const sf::Input& input) :
	Entity(GET_IMG("spaceship"), offset),
	panel_(ControlPanel::GetInstance()),
	input_(input),
	laserbeam_(Weapon::LASERBEAM, this),
	hellfire_(Weapon::HELLFIRE, this)
{
#ifdef DEBUG
	puts("PlayerShip()");
#endif
	hp_ = HP_DEFAULT;
	shield_ = SHIELD_DEFAULT;
	coolers_ = COOLER_DEFAULT;
	overheated_ = false;
	shield_timer_ = 0;
	heat_ = 0.0f;
	speed_ = DEFAULT_SPEED;
#ifndef NO_AUDIO
	shield_sfx_.SetBuffer(GET_SOUNDBUF("warp"));
#endif
	ParticleSystem::GetInstance().AddShield(SHIELD_DEFAULT, &sprite_);
	
	panel_.SetMaxShipHP(HP_MAX);
	panel_.SetMaxShield(SHIELD_MAX);
	panel_.SetMaxHeat(HEAT_MAX);
	
	panel_.SetShipHP(hp_);
	panel_.SetShield(shield_);
	panel_.SetHeat(static_cast<int>(heat_));
	panel_.SetCoolers(coolers_);
	panel_.SetInfo("");
	
	// init timed bonus
	for (int i = 0; i < TIMED_BONUS_COUNT; ++i)
	{
		bonus_[i] = 0;
	}
	
	// init bindings config
	const Settings& settings = Settings::GetInstance();
	binds_.up = settings.GetKey(Settings::UP);
	binds_.down = settings.GetKey(Settings::DOWN);
	binds_.left = settings.GetKey(Settings::LEFT);
	binds_.right = settings.GetKey(Settings::RIGHT);
	binds_.weapon_a = settings.GetKey(Settings::WEAPON1);
	binds_.weapon_b = settings.GetKey(Settings::WEAPON2);
	binds_.bonus_cooler = settings.GetKey(Settings::BONUS_COOLER);
}


PlayerShip::~PlayerShip()
{
#ifdef DEBUG
	puts("~PlayerShip()");
#endif
}


void PlayerShip::HandleKey(const sf::Event::KeyEvent& key)
{
	if (key.Code == binds_.bonus_cooler)
	{
		if (coolers_ > 0)
		{
			--coolers_;
			panel_.SetCoolers(coolers_);
			heat_ = 0.f;
			overheated_ = false;
			panel_.SetInfo("");
		}
	}
#ifdef DEBUG
	if (input_.IsKeyDown(sf::Key::H))
	{
		hp_ = 30;
		panel_.SetShipHP(hp_);
		shield_ = 30;
		panel_.SetShield(shield_);
		ParticleSystem::GetInstance().AddMessage(sprite_.GetPosition(), L"Secret Haxxx Of The Doom");
	}
#endif
}


void PlayerShip::Action()
{
	if (!overheated_)
	{
		float h = 0.0f;
		sf::Vector2f offset = sprite_.GetPosition() + GUN_OFFSET;
		if (input_.IsKeyDown(binds_.weapon_a))
		{ 
			h += laserbeam_.Shoot(offset);
		}
		if (input_.IsKeyDown(binds_.weapon_b))
		{
			h += hellfire_.Shoot(offset);
		}
		
		heat_ += h;
		if (heat_ >= HEAT_MAX)
		{
			overheated_ = true;
			panel_.SetInfo("Surchauffe !");
		}
	}
}


void PlayerShip::Move(float frametime)
{
	static const float WIDTH = sprite_.GetSize().x;
	static const float HEIGHT = sprite_.GetSize().y;
	// déplacement
	const sf::Vector2f& offset = GetPosition();
	float x = offset.x;
	float y = offset.y;
	
	float dist = frametime * speed_;
	
	if (input_.IsKeyDown(binds_.up))
	{
		y = (y - dist < CONTROL_PANEL_HEIGHT) ? CONTROL_PANEL_HEIGHT : y - dist;
	}
	if (input_.IsKeyDown(binds_.down))
	{
		y = (y + HEIGHT + dist > WIN_HEIGHT) ? WIN_HEIGHT - HEIGHT : y + dist;
	}
	if (input_.IsKeyDown(binds_.left))
	{
		x = (x - dist < 0) ? 0 : x - dist;
	}
	if (input_.IsKeyDown(binds_.right))
	{
		x = (x + WIDTH + dist > WIN_WIDTH) ? WIN_WIDTH - WIDTH : x + dist;
	}
	sprite_.SetPosition(x, y);
	
	// regénération bouclier
	if (shield_ < SHIELD_MAX)
	{
		shield_timer_ -= frametime;
		if (shield_timer_ <= 0.f)
		{
			// ajout d'une boule supplémentaire
			++shield_;
			ParticleSystem& p = ParticleSystem::GetInstance();
			p.RemoveShield(&sprite_);
			p.AddShield(shield_, &sprite_);
			
			shield_timer_ = 1 / SHIELD_RECOVERY_RATE;
			panel_.SetShield(shield_);
		}
	}
	
	// refroidissement
	if (heat_ > 0.f)
	{
		heat_ -= COLDING_RATE * frametime;
		if (heat_ <= 0.f)
		{
			heat_ = 0.f;
			if (overheated_ )
			{
				overheated_ = false;
				panel_.SetInfo("");
			}
		}
	}
	panel_.SetHeat(static_cast<int>(heat_));
	
	// timer bonus
	for (int i = 0; i < TIMED_BONUS_COUNT; ++i)
	{
		if (bonus_[i] > 0)
		{
			bonus_[i] -= frametime;
			if (bonus_[i] <= 0)
			{
				bonus_[i] = 0;
				DisableTimedBonus((TimedBonus) i);
			}
		}
	}
	laserbeam_.Update(frametime);
	hellfire_.Update(frametime);
}


void PlayerShip::Hit(int damage)
{
	static ParticleSystem& p = ParticleSystem::GetInstance();
	if (shield_ > 0)
	{
		shield_ -= damage;
		p.RemoveShield(&sprite_);
		if (shield_ > 0)
		{
			p.AddShield(shield_, &sprite_);
		}
		else
		{
#ifndef NO_AUDIO
			shield_sfx_.Play();
#endif
			shield_ = 0;
		}
		panel_.SetShield(shield_);
	}
	else
	{
		Entity::Hit(damage);
		panel_.SetShipHP(hp_);
	}
}


void PlayerShip::Collide(Entity& ent)
{
	Bonus* bonus = dynamic_cast<Bonus*>(&ent);
	if (bonus != NULL)
	{
		HandleBonus(*bonus);
	}
	else
	{
		Hit(1);
	}
}


void PlayerShip::HandleBonus(const Bonus& bonus)
{
	switch (bonus.GetType())
	{
		case Bonus::TRISHOT:
			if (bonus_[T_TRISHOT] == 0)
			{
				hellfire_.SetTriple(true);
				laserbeam_.SetTriple(true);
				puts("bonus triple tir activé");
			}
			else
			{
				puts("bonus triple tir relancé");
			}
			bonus_[T_TRISHOT] += TIMED_BONUS_DURATION;
			break;
		case Bonus::SPEED:
			if (bonus_[T_SPEED] == 0)
			{
				speed_ *= 2;
				puts("bonus speed activé");
			}
			else
			{
				puts("bonus speed relancé");
			}
			bonus_[T_SPEED] += TIMED_BONUS_DURATION;
			break;
		case Bonus::HEALTH:
			if (hp_ < HP_MAX)
			{
				++hp_;
				panel_.SetShipHP(hp_);				
			}
			break;
		case Bonus::COOLER:
			if (coolers_ < COOLER_MAX)
			{
				++coolers_;
				panel_.SetCoolers(coolers_);
			}
			break;
		default:
			break;
	}
	ParticleSystem::GetInstance().AddMessage(bonus.GetPosition(),
		bonus.WhatItIs());
}


void PlayerShip::DisableTimedBonus(TimedBonus tbonus)
{
	switch (tbonus)
	{
		case T_TRISHOT:
			hellfire_.SetTriple(false);
			laserbeam_.SetTriple(false);
			puts("bonus triple tir désactivé");
			break;
		case T_SPEED:
			speed_ = DEFAULT_SPEED;
			puts("bonus speed désactivé");
			break;
		default:
			assert(0);
	}
	bonus_[tbonus] = 0;
}

