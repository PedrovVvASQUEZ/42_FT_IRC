/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:11:54 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:17:16 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDHANDLER_HPP
# define COMMANDHANDLER_HPP

// === NUMERIC RESPONSES IRC ===

// INFOS N WELCOMING REPLIES
# define RPL_WELCOME			001
# define RPL_YOURHOST			002
# define RPL_CREATED			003
# define RPL_MYINFO				004
# define RPL_UMODEIS			221

// CHANNEL REPLIES
# define RPL_CHANNELMODEIS		324
# define RPL_NOTOPIC			331
# define RPL_TOPIC				332
# define RPL_INVITING			341
# define RPL_NAMREPLY			353
# define RPL_ENDOFNAMES			366

// INFORMATION REPLIES
# define RPL_MOTDSTART			375
# define RPL_MOTD				372
# define RPL_ENDOFMOTD			376

// Errors - USERS
# define ERR_NOSUCHNICK			401
# define ERR_NOSUCHCHANNEL		403
# define ERR_CANNOTSENDTOCHAN	404
# define ERR_TOOMANYCHANNELS	405
# define ERR_UNKNOWNCOMMAND		421
# define ERR_NONICKNAMEGIVEN	431
# define ERR_ERRONEUSNICKNAME	432
# define ERR_NICKNAMEINUSE		433

// Errors - CHANNELS
# define ERR_USERNOTINCHANNEL	441
# define ERR_NOTONCHANNEL		442
# define ERR_USERONCHANNEL		443
# define ERR_NOTREGISTERED		451
# define ERR_NEEDMOREPARAMS		461
# define ERR_ALREADYREGISTRED	462
# define ERR_PASSWDMISMATCH		464
# define ERR_NOORIGIN			409

// Errors - MODES N PERMISSIONS
# define ERR_KEYSET				467
# define ERR_CHANNELISFULL		471
# define ERR_UNKNOWNMODE		472
# define ERR_INVITEONLYCHAN		473
# define ERR_BANNEDFROMCHAN		474
# define ERR_BADCHANNELKEY		475
# define ERR_NOPRIVILEGES		481
# define ERR_CHANOPRIVSNEEDED	482

# include <string>
# include <vector>
# include "Clients.hpp"
# include "Messages.hpp"
# include "Channels.hpp"
# include <iostream>
# include <sstream>
# include <algorithm>
# include <cctype>
# include <iomanip>

class Server;

class	CommandHandler
{
	private:
		Server*		_server;
		
		std::string _toUpper(const std::string& str);
		bool		_isValidNickname(const std::string& nickname);
		
		void		_handlePass(Clients& client, const Messages& message);
		void		_handleNick(Clients& client, const Messages& message);
		void		_handleUser(Clients& client, const Messages& message);
		void		_handleJoin(Clients& client, const Messages& message);
		void		_handlePart(Clients& client, const Messages& message);
		void		_handlePrivmsg(Clients& client, const Messages& message);
		void		_handleQuit(Clients& client, const Messages& message);
		void		_handleKick(Clients& client, const Messages& message);
		void		_handleInvite(Clients& client, const Messages& message);
		void		_handleMode(Clients& client, const Messages& message);
		void		_handleTopic(Clients& client, const Messages& message);
		void		_handlePing(Clients& client, const Messages& message);
		void		_handleNotice(Clients& client, const Messages& message);
		void		_handleChannelModes(Clients& client, Channels* channel, const std::string& channelName, const std::string& modeString, const Messages& message);
		void		_broadcastModeChange(Clients& client, const std::string& channelName, const std::string& mode, const std::string& param);
		
		void		_sendNumericReply(Clients& client, int code, const std::string& params);
		void		_sendWelcomeSequence(Clients& client);
		std::string	_formatNumericReply(int code, const std::string& target, const std::string& message);
		
	public:
		CommandHandler(void);
		CommandHandler(Server* server);
		CommandHandler(const CommandHandler &rhs);
		~CommandHandler(void);

		CommandHandler	&operator=(const CommandHandler &rhs);

		void		processCommand(Clients& client, const Messages& message);
		void		setServer(Server* server);
};

#endif
