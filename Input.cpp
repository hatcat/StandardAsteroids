#include "Input.h"

#include <Windows.h>

namespace
{
	auto anti_clockwise_key = 'Q';
	auto clockwise_key = 'W';
	auto thrust_key = 'U';
	auto fire_key = 'I';
	auto hyperspace_key = 'O';

	auto anti_clockwise_key_press = false;
	auto clockwise_key_press = false;
	auto thrust_key_press = false;
	auto fire_key_press = false;
	auto fire_down = false;
	auto hyperspace_key_press = false;
	auto hyperspace_down = false;

	bool bounce_check(const bool& key, bool& down)
	{
		if (key && !down)
		{
			down = true;
			return true;
		}
		down = key;
		return false;
	}
}

void asteroids::get_key_states()
{
	anti_clockwise_key_press = GetKeyState(anti_clockwise_key) < 0;
	clockwise_key_press = GetKeyState(clockwise_key) < 0;
	thrust_key_press = GetKeyState(thrust_key) < 0;
	fire_key_press = GetKeyState(fire_key) < 0;
	hyperspace_key_press = GetKeyState(hyperspace_key) < 0;
}

bool asteroids::anti_clockwise()
{
	return anti_clockwise_key_press;
}

bool asteroids::clockwise()
{
	return clockwise_key_press;
}

bool asteroids::thrust()
{
	return thrust_key_press;
}

bool asteroids::fire()
{
	return bounce_check(fire_key_press, fire_down);
}

bool asteroids::hyperspace()
{
	return bounce_check(hyperspace_key_press, hyperspace_down);
}
