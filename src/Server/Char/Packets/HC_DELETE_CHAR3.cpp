/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2020- Horizon Dev Team.
 *
 * Base Author - Sagun Khosla. (sagunxp@gmail.com)
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

#include "HC_DELETE_CHAR3.hpp"
#include "Server/Char/Session/CharSession.hpp"

using namespace Horizon::Char;
using namespace Horizon::Base;

HC_DELETE_CHAR3::HC_DELETE_CHAR3(std::shared_ptr<CharSession> s)
 : NetworkPacket<CharSession>(ID_HC_DELETE_CHAR3, s) { }

HC_DELETE_CHAR3::~HC_DELETE_CHAR3() { }

void HC_DELETE_CHAR3::deliver(uint32_t char_id, character_delete_accept_result result)
{
	_character_id = char_id;
	_result = result;
	
	serialize();
	transmit();
}

ByteBuffer &HC_DELETE_CHAR3::serialize()
{
	buf() << _packet_id;
	buf() << _character_id;
	buf() << _result;
	return buf();
}

void HC_DELETE_CHAR3::handle(ByteBuffer &&buf)
{
}

void HC_DELETE_CHAR3::deserialize(ByteBuffer &buf)
{
}

