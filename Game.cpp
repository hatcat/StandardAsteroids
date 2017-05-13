#include "Game.h"
#include "Input.h"
#include "Constants.h"

namespace
{
	static const asteroids::path_buffer* asteroid_vbs[] = { &asteroids::asteroid::a1, &asteroids::asteroid::a2, &asteroids::asteroid::a3, &asteroids::asteroid::a4 };
}

asteroids::game::game()
	: m_level{ 1 }
	, m_score {	0 }
	, m_ship(controllable_physics{
	physics{ vector_2d{ playing_field_width / 2, playing_field_height / 2 },{ vector_2d{ 0, 0 } } },
	vector_2d{ 0, 0 },
	0.0 })
	, m_ship_missile_count(0)
	, m_next_ship_missile(0)
	, m_ship_missiles(max_missiles)
	, m_rd()
	, m_gen(m_rd())
	, m_0_to_3(0, 3)
	, m_0_to_1(0, 1)
{
	generate_level();
}

void asteroids::game::update(display_surface& ds)
{
	using namespace std::experimental::io2d;

	get_key_states();
	update_asteroids();
	update_ship();
	update_missiles();

	ds.paint(brush{ bgra_color::black() });
	draw_asteroids(ds);
	draw_ship(ds);
	draw_missiles(ds);
}

void asteroids::game::generate_level()
{
	auto count = m_level * 2 + 2;
	std::uniform_int_distribution<> edge(1, 4);
	std::uniform_real_distribution<double> x(0.0, playing_field_width);
	std::uniform_real_distribution<double> y(0.0, playing_field_height);
	std::uniform_real_distribution<double> theta(0.0, tau);

	m_asteroids.clear();
	m_asteroids.reserve(count * 7);

	while (count-- != 0)
	{
		switch (edge(m_gen))
		{
		case 1:	m_asteroids.emplace_back(physics{ vector_2d{ 0.0, y(m_gen) },					pol_to_car(polar_2d{ initial_speed, theta(m_gen) }) }, path_from_prototype(*asteroid_vbs[m_0_to_3(m_gen)], big_asteroid_size), big_asteroid_size); break;
		case 2:	m_asteroids.emplace_back(physics{ vector_2d{ playing_field_width, y(m_gen) },	pol_to_car(polar_2d{ initial_speed, theta(m_gen) }) }, path_from_prototype(*asteroid_vbs[m_0_to_3(m_gen)], big_asteroid_size), big_asteroid_size); break;
		case 3:	m_asteroids.emplace_back(physics{ vector_2d{ x(m_gen), 0.0 },					pol_to_car(polar_2d{ initial_speed, theta(m_gen) }) }, path_from_prototype(*asteroid_vbs[m_0_to_3(m_gen)], big_asteroid_size), big_asteroid_size); break;
		case 4:	m_asteroids.emplace_back(physics{ vector_2d{ x(m_gen), playing_field_height },	pol_to_car(polar_2d{ initial_speed, theta(m_gen) }) }, path_from_prototype(*asteroid_vbs[m_0_to_3(m_gen)], big_asteroid_size), big_asteroid_size); break;
		}
	}
}

void asteroids::game::update_asteroids()
{
	for (auto& a : m_asteroids) { a.update(); }
}

void asteroids::game::draw_asteroids(display_surface& ds)
{
	for (auto& a : m_asteroids) { a.draw(ds); }
}

void asteroids::game::update_ship()
{
	auto missile = m_ship.update();
	if (missile.m_launch && m_ship_missile_count < max_missiles)
	{
		// launch a missile
		m_ship_missiles[m_next_ship_missile] = { missile.m_direction, missile.m_orientation, true };
		if (++m_next_ship_missile == max_missiles)
		{
			m_next_ship_missile = 0;
		}
		++m_ship_missile_count;
	}
}

void asteroids::game::update_missiles()
{
	auto ad = std::vector<asteroid_destruction>{};
	ad.reserve(max_missiles);
	for (auto& m : m_ship_missiles)
	{
		if (!m.active()) continue;
		if (!m.update())
		{
			m.destroy();
			--m_ship_missile_count;
		}
		for (auto& a : m_asteroids)
		{
			if (a.active() && collides(a.collision_data(), m.collision_data()))
			{
				ad.push_back(a.destroy());
				m.destroy();
				--m_ship_missile_count;
			}
		}
	}

	for (auto& next_asteroids : ad)
	{
		m_score += next_asteroids.m_score;
		if (!next_asteroids.m_physics) continue;

		auto new_physics = next_asteroids.m_physics->divide(m_gen, m_0_to_1);
		auto path1 = path_from_prototype(*asteroid_vbs[m_0_to_3(m_gen)], next_asteroids.m_size);
		m_asteroids.emplace_back(std::move(new_physics[0]), path1, next_asteroids.m_size);
		auto path2 = path_from_prototype(*asteroid_vbs[m_0_to_3(m_gen)], next_asteroids.m_size);
		m_asteroids.emplace_back(std::move(new_physics[1]), path2, next_asteroids.m_size);
	}
}

void asteroids::game::draw_ship(display_surface& ds)
{
	m_ship.draw(ds);
}

void asteroids::game::draw_missiles(display_surface& ds)
{
	for (auto& m : m_ship_missiles) { m.draw(ds); }
}

int asteroids::main()
{
	using namespace std::experimental::io2d;

	asteroids::game sd;
	auto ds = make_display_surface(640, 480, format::argb32, scaling::letterbox, refresh_rate::as_fast_as_possible, 30.0);
	ds.draw_callback([&](display_surface& ds) {sd.update(ds); });
	return ds.begin_show();
}