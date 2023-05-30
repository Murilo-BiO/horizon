/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2023 Horizon Dev Team.
 *
 * Base Author - Sephus. (sagunxp@gmail.com)
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

#ifndef HORIZON_COMMON_CLIENT_PACKET_LENGTH_TABLE
#define HORIZON_COMMON_CLIENT_PACKET_LENGTH_TABLE

#include "PacketLengthTable.hpp"

namespace Horizon
{
namespace Common
{
/**
 * @brief Auto-generated with a python generator tool authored by Sephus (sagunxp@gmail.com).
 */
class ClientPacketLengthTable : public PacketLengthTable
{
public:
	ClientPacketLengthTable(std::shared_ptr<CommonSession> s)
	: PacketLengthTable(s)
	{
#define ADD_HPKT(i, j, k) _hpacket_length_table.insert(i, std::make_pair(j, std::make_shared<k>(s)))
#define ADD_TPKT(i, j, k) _tpacket_length_table.insert(i, std::make_pair(j, std::make_shared<k>(s)))
// Packet Version 20080624: 2 Packets
#if PACKET_VERSION == 20080624 // 0
		ADD_TPKT(0x03dd, 18, AHC_GAME_GUARD);
		ADD_TPKT(0x03de, 18, CAH_ACK_GAME_GUARD);
// Packet Version 20130605: 2 Packets
#elif PACKET_VERSION == 20130605 // 1
		ADD_HPKT(0x09c5, 1042, CS_LOGIN_QUERY);
		ADD_TPKT(0x09c6, -1, SC_LOGIN_ANSWER);
// Packet Version 20130612: 2 Packets
#elif PACKET_VERSION == 20130612 // 2
		ADD_TPKT(0x09c7, 18, SC_LOGIN_ERROR);
		ADD_TPKT(0x09c9, -1, SC_SOCT);
// Packet Version 20131030: 1 Packets
#elif PACKET_VERSION == 20131030 // 3
		ADD_TPKT(0x09e0, -1, SC_LOGIN_ANSWER_WITH_ID);
#endif
#undef ADD_TPKT
#undef ADD_HPKT
	}

	~ClientPacketLengthTable() { }

};
}
}
#endif /* HORIZON_COMMON_CLIENT_PACKET_LENGTH_TABLE */
