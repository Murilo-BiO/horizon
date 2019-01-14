/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 * Copyright (c) 2018 Horizon Dev Team.
 *
 * Base Author - Sagun Khosla. (sagunxp@gmail.com)
 *
 * Under a proprietary license this file is not for use
 * or viewing without permission.
 **************************************************/

#ifndef HORIZON_INTERFACES_INTERSERVERAPI_HPP
#define HORIZON_INTERFACES_INTERSERVERAPI_HPP

#include "Server/Common/Base/PacketHandler/InterPackets.hpp"
#include "Server/Common/Models/GameAccount.hpp"
#include "Server/Common/Models/SessionData.hpp"

#include <stdio.h>
#include <memory>

namespace Horizon
{
namespace Interface
{
template <class SessionType>
class InterServerAPI
{
public:
	InterServerAPI()
	{
		//
	}

	virtual ~InterServerAPI()
	{
		//
	}

	/**
	 * @brief Obtain session data from the inter-server.
	 *        - Does not check if the inter-server is online
	 *          (but returns nullptr if the session happens to be null).
	 * @param[in] auth_code   session auth code to check against.
	 * @return shared_ptr to the session data or nullptr if none found.
	 */
	std::shared_ptr<SessionData> get_session_data(uint32_t auth_code)
	{
		std::shared_ptr<SessionData> session_data = std::make_shared<SessionData>();
		PACKET_INTER_SESSION_REQ send_pkt;
		PACKET_INTER_SESSION_GET recv_pkt;

		send_pkt.auth_code = auth_code;

		if (_inter_socket != nullptr
			&& _inter_socket->get_session()->get_packet_handler()->send_and_receive_packet(send_pkt, &recv_pkt) == Horizon::Base::inter_packets::INTER_SESSION_GET) {
			*session_data << recv_pkt.s;
			return session_data;
		}

		return nullptr;
	}

	/**
	 * @brief Request inter to delete a session from the server's storage.
	 *        - Use carefully, deletions performed will affect all other services.
	 * @param[in] auth_code  session auth code to check against.
	 */
	void delete_session(uint32_t auth_code)
	{
		if (_inter_socket != nullptr)
			_inter_socket->get_session()->get_packet_handler()->Send_INTER_SESSION_DEL(auth_code);
	}

	/**
	 * @brief Insert or Update session data into the inter server storage.
	 * @param[in|out] session_data   shared_ptr to the session data object.
	 */
	void store_session_data(std::shared_ptr<SessionData> const &session_data)
	{
		if (_inter_socket != nullptr)
			_inter_socket->get_session()->get_packet_handler()->Send_INTER_SESSION_SET(*session_data);
	}

	/**
	 * @brief Obtain game account data from the inter-server.
	 *        - Does not check if the inter-server is online
	 *          (but returns nullptr if the session happens to be null).
	 * @param[in] account_id   game account ID to check against.
	 * @return shared_ptr to the game account data or nullptr if none found.
	 */
	std::shared_ptr<GameAccount> get_game_account(uint32_t account_id)
	{
		std::shared_ptr<GameAccount> game_account = std::make_shared<GameAccount>();
		PACKET_INTER_GAME_ACCOUNT_REQ send_pkt;
		PACKET_INTER_GAME_ACCOUNT_GET recv_pkt;

		send_pkt.account_id = account_id;

		if (_inter_socket != nullptr
			&& _inter_socket->get_session()->get_packet_handler()->send_and_receive_packet(send_pkt, &recv_pkt) == Horizon::Base::inter_packets::INTER_GAME_ACCOUNT_GET) {
			*game_account << recv_pkt.s;
			return game_account;
		}

		return nullptr;
	}

	/**
	 * @brief Request inter to delete a game account from the server's storage.
	 *        - Use carefully, deletions performed will affect all other services.
	 * @param[in] account_id  game account ID to check against.
	 */
	void delete_game_account(uint32_t account_id)
	{
		if (_inter_socket != nullptr)
			_inter_socket->get_session()->get_packet_handler()->Send_INTER_GAME_ACCOUNT_DEL(account_id);
	}

	/**
	 * @brief Insert or Update game account data into the inter server storage.
	 * @param[in|out] game_account   shared_ptr to the game account data object.
	 */
	void store_game_account(std::shared_ptr<GameAccount> game_account)
	{
		if (_inter_socket != nullptr)
			_inter_socket->get_session()->get_packet_handler()->Send_INTER_GAME_ACCOUNT_SET(*game_account);
	}

	bool pingInterServer()
	{
		PACKET_INTER_PING send_pkt;
		PACKET_INTER_PONG recv_pkt;

		if (_inter_socket != nullptr
			&& _inter_socket->get_session()->get_packet_handler()->send_and_receive_packet(send_pkt, &recv_pkt) == Horizon::Base::inter_packets::INTER_PONG)
			return true;

		return false;
	}

	void set_inter_socket(std::shared_ptr<SessionType> socket) { _inter_socket.swap(socket); }

private:
	std::shared_ptr<SessionType> _inter_socket;
};
}
}

#endif /* HORIZON_INTERFACES_INTER_SERVER_HPP */
