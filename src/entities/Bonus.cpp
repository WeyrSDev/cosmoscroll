#include <typeinfo>
#include <cassert>
#include <SFML/System/Randomizer.hpp>

#include "Bonus.hpp"
#include "PlayerShip.hpp"
#include "../utils/MediaManager.hpp"

#define BONUS_SPEED 100


static inline const sf::Image& select_image(Bonus::Type type)
{
	static const char* keys[Bonus::BONUS_COUNT] = {
		"bonus-cooler",
		"bonus-health",
		"bonus-speed",
		"bonus-trishot"
	};
	return GET_IMG(keys[type]);
}


Bonus::Bonus(Type type, const sf::Vector2f& offset) :
	Entity(select_image(type), offset, 1)
{
	type_ = type;
}


Bonus* Bonus::MakeRandom(const sf::Vector2f& offset)
{
	Type type = (Type) sf::Randomizer::Random(0, BONUS_COUNT - 1);
	return new Bonus((Type) type, offset);
}


void Bonus::TakeDamage(int)
{
	// un bonus ne peut être détruit
}


void Bonus::OnCollide(Entity& entity)
{
	if (typeid (entity) == typeid (PlayerShip))
	{
		Kill(); // le bonus disparait
	}
}


void Bonus::Update(float frametime)
{
	sf::Sprite::Move(-BONUS_SPEED * frametime, 0);
	KillIfOut();
}


const wchar_t* Bonus::WhatItIs() const
{
	switch (type_)
	{
		case COOLER:
			return L"Glaçon";
		case HEALTH:
			return L"Health";
		case SPEED:
			return L"Vitesse";
		case TRISHOT:
			return L"Triple tir";
		default:
			assert(0);
	}
}

