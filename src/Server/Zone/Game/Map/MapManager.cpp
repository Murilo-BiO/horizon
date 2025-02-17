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
 * Base Author - Sxyz <sagunxp@gmail.com>
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

#include "MapManager.hpp"
#include "MapContainerThread.hpp"

#include "Server/Common/Configuration/Horizon.hpp"
#include "Libraries/MapCache/MapCache.hpp"
#include "Server/Zone/Game/Map/Map.hpp"
#include "Server/Zone/Game/Entities/Player/Player.hpp"
#include "Server/Zone/Zone.hpp"

using namespace Horizon::Zone;

MapManager::~MapManager()
{

}

bool MapManager::initialize()
{
	return LoadMapCache();
}

bool MapManager::finalize()
{
	std::map<int32_t, std::shared_ptr<MapContainerThread>> container_map = _map_containers.get_map();
	for (auto &cont : container_map) {
		cont.second->finalize();
	}

	return true;
}

bool MapManager::LoadMapCache()
{
	Horizon::Libraries::MapCache m;
	std::string db_path = sZone->config().get_static_db_path().string();

	m.setMapListPath(db_path + "map_list.lua");
	m.setMapCachePath(sZone->config().get_mapcache_path().string());

	if (m.ReadMapListConfig() != MCACHE_CONFIG_OK) {
		HLog(error) <<"Could not read map config file '" << m.getMapListPath().string() << "'.";
		return false;
	}

	switch (m.ImportFromCacheFile())
	{
		default:
			break;
		case MCACHE_IMPORT_NONEXISTENT_FILE:
			HLog(error) << "Could not open file '" << m.getMapCachePath().string() << "'.";
			return false;
		case MCACHE_IMPORT_READ_ERROR:
			HLog(error) << "Could not read file '" << m.getMapCachePath().string() << "', rebuilding...";
			return false;
		case MCACHE_IMPORT_INVALID_CHECKSUM:
			HLog(error) << "File cache file '" << m.getMapCachePath().string() << "' is corrupted (invalid checksum), rebuilding...";
			return false;
		case MCACHE_IMPORT_DECOMPRESS_ERROR:
			HLog(error) << "File cache file '" << m.getMapCachePath().string() << "' could not be decompressed, rebuilding...";
			return false;
		case MCACHE_IMPORT_MAPINFO_ERROR:
			HLog(error) << "Could not read map information for a map while importing file '" << m.getMapCachePath().string() << "', rebuilding...";
			return false;
		case MCACHE_IMPORT_CELLINFO_ERROR:
			HLog(error) << "Could not read cell information for a map while importing file '" << m.getMapCachePath().string() << "', rebuilding...";
			return false;
	}
;
	int container_idx = 0, map_counter = 0, total_maps = 0;
	int mcache_size = m.getMCache()->maps.size();
	int container_max = std::ceil((double) mcache_size / MAX_MAP_CONTAINER_THREADS);

	HLog(info) << "Initializing " << MAX_MAP_CONTAINER_THREADS << " map containers with " << container_max << " maps per container for a total of " << mcache_size << " maps...";

	for (int i = 0; i < MAX_MAP_CONTAINER_THREADS; i++)
		_map_containers.insert(i, std::make_shared<MapContainerThread>());

	for (auto &i : m.getMCache()->maps) {
		std::shared_ptr<Map> map = std::make_shared<Map>(_map_containers.at(container_idx), i.second.name(), i.second.width(), i.second.height(), i.second.getCells());
		(_map_containers.at(container_idx))->add_map(std::move(map));
		map_counter++;
		total_maps++;

		if (container_max == map_counter || total_maps == mcache_size) {
			HLog(info) << "Initializing " << map_counter << " maps in map container " << (void *) _map_containers.at(container_idx).get() << "...";
			(_map_containers.at(container_idx))->initialize();
			(_map_containers.at(container_idx))->start();
			map_counter = 0;
			container_idx++;
		}
	}

	HLog(info) << "Done initializing " << total_maps << " maps in " << MAX_MAP_CONTAINER_THREADS << " containers.";

	return true;
}

std::shared_ptr<Map> MapManager::add_player_to_map(std::string map_name, std::shared_ptr<Entities::Player> p)
{
	std::map<int32_t, std::shared_ptr<MapContainerThread>> container_map = _map_containers.get_map();
	for (auto i = container_map.begin(); i != container_map.end(); i++) {
		std::shared_ptr<Map> map = i->second->get_map(map_name);
		if (map != nullptr) {
			i->second->add_player(p);
			return map;
		} else {
			return nullptr;
		}
	}

	return nullptr;
}

bool MapManager::remove_player_from_map(std::string map_name, std::shared_ptr<Entities::Player> p)
{
	std::map<int32_t, std::shared_ptr<MapContainerThread>> container_map = _map_containers.get_map();
	for (auto i = container_map.begin(); i != container_map.end(); i++) {
		if (i->second->get_map(map_name) != nullptr) {
			i->second->remove_player(p);
			return true;
		}
	}

	return false;
}
