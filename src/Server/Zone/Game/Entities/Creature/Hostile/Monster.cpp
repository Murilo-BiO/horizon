/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 *
 * Copyright (c) 2019 Sagun K. (sagunxp@gmail.com).
 * Copyright (c) 2019 Horizon Dev Team.
 *
 * Base Author - Sagun K. (sagunxp@gmail.com)
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************/

#include "Monster.hpp"

#include "Core/Logging/Logger.hpp"
#include "Common/Definitions/EntityDefinitions.hpp"
#include "Server/Zone/Game/Entities/Traits/Status.hpp"
#include "Server/Zone/Game/Map/Map.hpp"

using namespace Horizon::Zone::Entities;

Monster::Monster(std::shared_ptr<Map> map, MapCoords mcoords, std::string const &name, uint16_t mob_id)
: Creature(_last_np_entity_guid++, ENTITY_MONSTER, map, mcoords)
{
	set_name(name);
	set_job_id(mob_id);
	set_direction(DIR_SOUTH);
}

Monster::~Monster()
{
	if (has_valid_grid_reference())
		remove_grid_reference();
}

void Monster::initialize()
{
	Entity::initialize();

	status()->initialize();
	map()->ensure_grid_for_entity(this, map_coords());
	
	set_monster_config(MonsterDB->get_monster_by_id(job_id()));
	if (monster_config() == nullptr) {
		HLog(error) << "Error finding monster by id " << job_id() << " in database.";
		return;
	}

    getScheduler().Schedule(Milliseconds(MOB_MIN_THINK_TIME_LAZY), [this] (TaskContext context) {
        if (is_spotted())
            perform_ai_lazy();
        context.Repeat(Milliseconds(MOB_MIN_THINK_TIME_LAZY));
    });
}

void Monster::perform_ai_lazy()
{
	std::srand(std::time(nullptr));

	if (monster_config()->mode & MONSTER_MODE_MASK_CANMOVE && is_spotted() && (_next_walktime - std::time(nullptr) < 0) && !is_walking()) {
		getScheduler().Schedule(Milliseconds(MIN_RANDOM_TRAVEL_TIME + (std::rand() % MOB_LAZY_MOVE_RATE)), ENTITY_SCHEDULE_AI_WALK, [this] (TaskContext context) {
			MapCoords mc = map()->get_random_coordinates_in_walkable_area(map_coords().x(), map_coords().y(), MAX_VIEW_RANGE, MAX_VIEW_RANGE);
			move_to_coordinates(mc.x(), mc.y());

			int total_movement_cost = 0;
			for (int i = 0; i < _walk_path.size(); i++) {
				MapCoords m = _walk_path.at(i);
				total_movement_cost += m.move_cost();
			}
			HLog(debug) << "Monster " << name() << " is set to travel from (" << map_coords().x() << "," << map_coords().y() << ") to (" << mc.x() << ", " << mc.y() << ") cost (" << total_movement_cost << ").";
            _next_walktime = std::time(nullptr) + ((std::rand() % MIN_RANDOM_TRAVEL_TIME) / 1000) + total_movement_cost;
		});
	}
}

void Monster::stop_movement()
{
}

void Monster::on_pathfinding_failure()
{
}

void Monster::on_movement_begin()
{

}

void Monster::on_movement_step()
{
	map()->ensure_grid_for_entity(this, map_coords());
}

void Monster::on_movement_end()
{
}

void Monster::sync_with_models()
{

}
