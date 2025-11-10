/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:34 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/20 18:59:07 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"
#include "Server.hpp"

//==================== Constructeurs et Destructeur ===================//

CommandHandler::CommandHandler(void) : _server(NULL)
{
	return ;
}

CommandHandler::CommandHandler(Server* server) : _server(server)
{
	return ;
}

CommandHandler::CommandHandler(const CommandHandler &rhs)
{
	*this = rhs;
	return ;
}

CommandHandler::~CommandHandler(void)
{
	return ;
}

CommandHandler &CommandHandler::operator=(const CommandHandler &rhs)
{
	if (this != &rhs)
	{
		_server = rhs._server;
	}
	return *this;
}

void CommandHandler::setServer(Server* server)
{
	_server = server;
}

//==================== Fonctions utilitaires ===================//

std::vector<std::string> CommandHandler::_split(const std::string& str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	
	while (std::getline(tokenStream, token, delimiter))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	return tokens;
}

std::string CommandHandler::_toUpper(const std::string& str)
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
	{
		result[i] = std::toupper(result[i]);
	}
	return result;
}

bool CommandHandler::_isValidNickname(const std::string& nickname)
{
	// Un nickname IRC valide :
	// - 1-9 caractères
	// - Commence par une lettre ou caractères spéciaux: [ ] \ ` _ ^ { | }
	// - Peut contenir lettres, chiffres, - [ ] \ ` _ ^ { | }
	
	if (nickname.empty() || nickname.length() > 9)
		return false;
		
	// Premier caractère : lettre ou caractères spéciaux IRC
	char first = nickname[0];
	if (!std::isalpha(first) && first != '[' && first != ']' && first != '\\' && 
		first != '`' && first != '_' && first != '^' && first != '{' && 
		first != '|' && first != '}')
		return false;
	
	// Autres caractères : alphanumériques + caractères spéciaux IRC + -
	for (size_t i = 1; i < nickname.length(); ++i)
	{
		char c = nickname[i];
		if (!std::isalnum(c) && c != '-' && c != '[' && c != ']' && c != '\\' && 
			c != '`' && c != '_' && c != '^' && c != '{' && c != '|' && c != '}')
			return false;
	}
	
	return true;
}

bool CommandHandler::_isValidChannel(const std::string& channel)
{
	if (channel.empty() || channel.length() > 50)
		return false;
		
	if (channel[0] != '#')
		return false;
	for (size_t i = 1; i < channel.length(); ++i)
	{
		char c = channel[i];
		if (c == ' ' || c == ',' || c == '\007' || c < 32 || c > 126)
			return false;
	}
	return true;
}

//==================== Fonction principale ===================//

void CommandHandler::processCommand(Clients& client, const Messages& message)
{
	if (!_server)
	{
		std::cerr << "Error: CommandHandler not properly initialized with server" << std::endl;
		return;
	}
	
	if (!message.isValid())
	{
		std::cerr << "Error: Invalid message format" << std::endl;
		return;
	}
	std::string command = message.getCommand();
	std::cout << "📨 Processing command from client " << client.getFd() << ": '" << command << "'" << std::endl;
	std::cout << "🔍 Message valid: " << (message.isValid() ? "YES" : "NO") << std::endl;
	command = _toUpper(command);
	if (command == "PASS")
		_handlePass(client, message);
	else if (command == "NICK")
		_handleNick(client, message);
	else if (command == "USER")
		_handleUser(client, message);
	else if (command == "JOIN")
		_handleJoin(client, message);
	else if (command == "PART")
		_handlePart(client, message);
	else if (command == "PRIVMSG")
		_handlePrivmsg(client, message);
	else if (command == "QUIT")
		_handleQuit(client, message);
	else if (command == "KICK")
		_handleKick(client, message);
	else if (command == "INVITE")
		_handleInvite(client, message);
	else if (command == "MODE")
		_handleMode(client, message);
	else if (command == "TOPIC")
		_handleTopic(client, message);
	else
		_sendNumericReply(client, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
}

//==================== Fonctions de réponse IRC ===================//

std::string CommandHandler::_formatNumericReply(int code, const std::string& target, const std::string& message)
{
	std::ostringstream oss;
	oss << ":" << _server->getServerName() << " " << std::setfill('0') << std::setw(3) << code << " " << target << " " << message << "\r\n";
	return oss.str();
}

void CommandHandler::_sendNumericReply(Clients& client, int code, const std::string& params)
{
	std::string target = client.getNickname().empty() ? "*" : client.getNickname();
	std::string formatted_reply = _formatNumericReply(code, target, params);
	_server->sendMessage(client.getFd(), formatted_reply);
}

void CommandHandler::_sendWelcomeSequence(Clients& client)
{
	std::string nick = client.getNickname();
	std::string user = client.getUsername();
	std::string host = client.getHostname();

	_sendNumericReply(client, RPL_WELCOME, ":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
	_sendNumericReply(client, RPL_YOURHOST, ":Your host is " + _server->getServerName() + ", running version 1.0");
	_sendNumericReply(client, RPL_CREATED, ":This server was created today");
	_sendNumericReply(client, RPL_MYINFO, _server->getServerName() + " 1.0 o o");
	std::cout << "✅ Welcome sequence sent to " << nick << std::endl;
}

//==================== Commandes d'authentification ===================//

void CommandHandler::_handlePass(Clients& client, const Messages& message)
{
	if (message.getParamCount() == 0)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		return;
	}
	std::string password = message.getParam(0);
	if (client.getState() == AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return;
	}
	if (password == _server->getPassword())
	{
		client.setPasswordValid(true);
		std::cout << "🔐 Client " << client.getFd() << " provided correct password" << std::endl;
	}
	else
	{
		_sendNumericReply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		std::cout << "❌ Client " << client.getFd() << " failed authentication" << std::endl;
		client.setState(DISCONNECTED);
	}
}

void CommandHandler::_handleNick(Clients& client, const Messages& message)
{
	if (message.getParamCount() == 0)
	{
		_sendNumericReply(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
		return;
	}
	std::string nickname = message.getParam(0);
	if (!_isValidNickname(nickname))
	{
		_sendNumericReply(client, ERR_ERRONEUSNICKNAME, nickname + " :Erroneous nickname");
		return;
	}

	if (_server->isNicknameInUse(nickname))
	{
		_sendNumericReply(client, ERR_NICKNAMEINUSE, nickname + " :Nickname is already in use");
		return;
	}
	std::string old_nick = client.getNickname();
	client.setNickname(nickname);
	if (old_nick.empty())
		std::cout << "📛 Client " << client.getFd() << " set nickname to: " << nickname << std::endl;
	else
	{
		std::cout << "📛 Client " << client.getFd() << " changed nickname from " << old_nick << " to " << nickname << std::endl;
		if (client.getState() == AUTHENTIFICATED)
		{
			std::string nick_change = ":" + old_nick + "!" + client.getUsername() + "@" + client.getHostname() + " NICK " + nickname;
			_server->sendMessage(client.getFd(), nick_change);
		}
	}
	if (!client.getNickname().empty() && !client.getUsername().empty() && 
		client.isPasswordValid() && client.getState() != AUTHENTIFICATED)
	{
		client.setState(AUTHENTIFICATED);
		_sendWelcomeSequence(client);
	}
}

void CommandHandler::_handleUser(Clients& client, const Messages& message)
{
	if (message.getParamCount() < 4)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return;
	}
	if (client.getState() == AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return;
	}
	std::string username = message.getParam(0);
	std::string realname = message.getParamCount() > 3 ? message.getParam(3) : "Unknown";
	client.setUsername(username);
	client.setRealname(realname);
	std::cout << "👤 Client " << client.getFd() << " set user info: " << username << " (" << realname << ")" << std::endl;
	if (!client.getNickname().empty() && !client.getUsername().empty() && 
		client.isPasswordValid() && client.getState() != AUTHENTIFICATED)
	{
		client.setState(AUTHENTIFICATED);
		_sendWelcomeSequence(client);
	}
}

void CommandHandler::_handleQuit(Clients& client, const Messages& message)
{
	std::string quit_message = "Quit";
	if (message.getParamCount() > 0)
		quit_message = message.getParam(0);
		
	std::cout << "👋 Client " << client.getNickname() << " (" << client.getFd() << ") is quitting: " << quit_message << std::endl;
	_server->removeClient(client.getFd());
}

//==================== Commandes de channels (stubs) ===================//

void CommandHandler::_handleJoin(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return;
	}
	std::string channel_name = message.getParam(0);
	
	// Vérifier que le nom du channel commence par #
	if (channel_name.empty() || channel_name[0] != '#')
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		channel = _server->createChannel(channel_name);
		std::cout << "📺 Channel " << channel_name << " created by " << client.getNickname() << std::endl;
	}
	if (channel->addMember(&client))
	{
		if (channel->getMembersCount() == 1)
		{
			channel->addOperator(&client);
			std::cout << "👑 " << client.getNickname() << " became operator of " << channel_name << " (channel founder)" << std::endl;
		}
		std::cout << "📺 " << client.getNickname() << " joined channel " << channel_name << std::endl;
		std::string join_message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN " + channel_name;
		_server->sendMessage(client.getFd(), join_message);
		
		const std::set<Clients*>& members = channel->getMembers();
		std::set<Clients*>::const_iterator it;
		for (it = members.begin(); it != members.end(); ++it)
		{
			if (*it != &client && (*it)->isConnected())
			{
				_server->sendMessage((*it)->getFd(), join_message);
			}
		}
		if (!channel->getTopic().empty())
		{
			_sendNumericReply(client, RPL_TOPIC, channel_name + " :" + channel->getTopic());
		}
		else
		{
			_sendNumericReply(client, RPL_NOTOPIC, channel_name + " :No topic is set");
		}
		std::string members_list = channel->getMembersList();
		_sendNumericReply(client, RPL_NAMREPLY, "= " + channel_name + " :" + members_list);
		_sendNumericReply(client, RPL_ENDOFNAMES, channel_name + " :End of /NAMES list");
	}
	else
	{
		std::cout << "⚠️  " << client.getNickname() << " already in channel " << channel_name << std::endl;
	}
}

void CommandHandler::_handlePart(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return;
	}
	
	std::string channel_name = message.getParam(0);
	std::string part_message = message.getParamCount() > 1 ? message.getParam(1) : client.getNickname();
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	std::string part_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PART " + channel_name;
	if (part_message != client.getNickname())
		part_broadcast += " :" + part_message;
	_server->sendMessage(client.getFd(), part_broadcast);
	const std::set<Clients*>	&members = channel->getMembers();
	std::set<Clients*>::const_iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (*it != &client && (*it)->isConnected())
		{
			_server->sendMessage((*it)->getFd(), part_broadcast);
		}
	}
	if (channel->removeMember(&client))
	{
		std::cout << "📺 " << client.getNickname() << " left channel " << channel_name << std::endl;
		if (channel->isEmpty())
		{
			_server->removeChannel(channel_name);
		}
	}
}

void CommandHandler::_handlePrivmsg(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 2)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
		return;
	}
	std::string target = message.getParam(0);
	std::string msg_text = message.getParam(1);
	if (target.empty() || msg_text.empty())
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
		return;
	}
	std::string privmsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PRIVMSG " + target + " :" + msg_text;
	if (target[0] == '#')
	{
		Channels* channel = _server->getChannel(target);
		if (!channel)
		{
			_sendNumericReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
			return;
		}
		if (!channel->isMember(&client))
		{
			_sendNumericReply(client, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
			return;
		}
		const std::set<Clients*>& members = channel->getMembers();
		std::set<Clients*>::const_iterator it;
		int sent_count = 0;
		for (it = members.begin(); it != members.end(); ++it)
		{
			if (*it != &client && (*it)->isConnected())
			{
				_server->sendMessage((*it)->getFd(), privmsg);
				sent_count++;
			}
		}
		std::cout << "💬 " << client.getNickname() << " sent message to channel " << target << " (delivered to " << sent_count << " users)" << std::endl;
	}
	else
	{
		Clients* target_client = _server->getClientByNickname(target);
		if (!target_client || !target_client->isConnected())
		{
			_sendNumericReply(client, ERR_NOSUCHNICK, target + " :No such nick/channel");
			return;
		}
		_server->sendMessage(target_client->getFd(), privmsg);
		std::cout << "💬 " << client.getNickname() << " sent private message to " << target << std::endl;
	}
}

void CommandHandler::_handleKick(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 2)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return;
	}
	std::string channel_name = message.getParam(0);
	std::string target_nick = message.getParam(1);
	std::string kick_reason = message.getParamCount() > 2 ? message.getParam(2) : client.getNickname();
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	if (!channel->isOperator(&client))
	{
		_sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channel_name + " :You're not channel operator");
		return;
	}
	Clients* target_client = _server->getClientByNickname(target_nick);
	if (!target_client)
	{
		_sendNumericReply(client, ERR_NOSUCHNICK, target_nick + " :No such nick/channel");
		return;
	}
	if (!channel->isMember(target_client))
	{
		_sendNumericReply(client, ERR_USERNOTINCHANNEL, target_nick + " " + channel_name + " :They aren't on that channel");
		return;
	}
	std::string kick_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " KICK " + channel_name + " " + target_nick + " :" + kick_reason;
	const std::set<Clients*>& members = channel->getMembers();
	std::set<Clients*>::const_iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->isConnected())
		{
			_server->sendMessage((*it)->getFd(), kick_broadcast);
		}
	}
	channel->removeMember(target_client);
	std::cout << "⚡ " << client.getNickname() << " kicked " << target_nick << " from " << channel_name << " (" << kick_reason << ")" << std::endl;
	if (channel->isEmpty())
	{
		_server->removeChannel(channel_name);
	}
}

void CommandHandler::_handleInvite(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 2)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		return;
	}
	std::string target_nick = message.getParam(0);
	std::string channel_name = message.getParam(1);
	Clients* target_client = _server->getClientByNickname(target_nick);
	if (!target_client)
	{
		_sendNumericReply(client, ERR_NOSUCHNICK, target_nick + " :No such nick/channel");
		return;
	}
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	if (channel->isMember(target_client))
	{
		_sendNumericReply(client, ERR_USERONCHANNEL, target_nick + " " + channel_name + " :is already on channel");
		return;
	}
	std::string invite_message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " INVITE " + target_nick + " :" + channel_name;
	_server->sendMessage(target_client->getFd(), invite_message);
	_sendNumericReply(client, RPL_INVITING, target_nick + " " + channel_name);
	std::cout << "✉️ " << client.getNickname() << " invited " << target_nick << " to " << channel_name << std::endl;
}

void CommandHandler::_handleMode(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return;
	}
	std::string target = message.getParam(0);
	if (target[0] == '#')
	{
		Channels* channel = _server->getChannel(target);
		if (!channel)
		{
			_sendNumericReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
			return;
		}
		if (!channel->isMember(&client))
		{
			_sendNumericReply(client, ERR_NOTONCHANNEL, target + " :You're not on that channel");
			return;
		}
		if (message.getParamCount() == 1)
		{
			_sendNumericReply(client, RPL_CHANNELMODEIS, target + " +");
			return;
		}
		std::string mode_string = message.getParam(1);
		if (mode_string == "+o" || mode_string == "-o")
		{
			if (message.getParamCount() < 3)
			{
				_sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
				return;
			}
			std::string target_nick = message.getParam(2);
			Clients* target_client = _server->getClientByNickname(target_nick);
			if (!target_client)
			{
				_sendNumericReply(client, ERR_NOSUCHNICK, target_nick + " :No such nick/channel");
				return;
			}
			if (!channel->isMember(target_client))
			{
				_sendNumericReply(client, ERR_USERNOTINCHANNEL, target_nick + " " + target + " :They aren't on that channel");
				return;
			}
			if (!channel->isOperator(&client))
			{
				_sendNumericReply(client, ERR_CHANOPRIVSNEEDED, target + " :You're not channel operator");
				return;
			}
			if (mode_string == "+o")
			{
				if (channel->addOperator(target_client))
					std::cout << "🔧 " << client.getNickname() << " gave operator to " << target_nick << " in " << target << std::endl;
				else
				{
					std::cout << "⚠️  Failed to give operator to " << target_nick << " (already operator or not member)" << std::endl;
					return;
				}
			}
			else
			{
				if (channel->removeOperator(target_client))
					std::cout << "🔧 " << client.getNickname() << " removed operator from " << target_nick << " in " << target << std::endl;
				else
				{
					std::cout << "⚠️  Cannot remove last operator from " << target << std::endl;
					return;
				}
			}
			std::string mode_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " MODE " + target + " " + mode_string + " " + target_nick;
			const std::set<Clients*>& members = channel->getMembers();
			std::set<Clients*>::const_iterator it;
			for (it = members.begin(); it != members.end(); ++it)
			{
				if ((*it)->isConnected())
				{
					_server->sendMessage((*it)->getFd(), mode_broadcast);
				}
			}
		}
		else
			std::cout << "🔧 MODE " << mode_string << " not implemented yet" << std::endl;
	}
	else
		std::cout << "🔧 User MODE not implemented yet" << std::endl;
}

void CommandHandler::_handleTopic(Clients& client, const Messages& message)
{
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return;
	}
	std::string channel_name = message.getParam(0);
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	if (message.getParamCount() == 1)
	{
		if (channel->getTopic().empty())
			_sendNumericReply(client, RPL_NOTOPIC, channel_name + " :No topic is set");
		else
			_sendNumericReply(client, RPL_TOPIC, channel_name + " :" + channel->getTopic());
		return;
	}
	std::string new_topic = message.getParam(1);
	if (!channel->isOperator(&client))
	{
		_sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channel_name + " :You're not channel operator");
		return;
	}
	channel->setTopic(new_topic);
	std::string topic_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " TOPIC " + channel_name + " :" + new_topic;
	const std::set<Clients*>& members = channel->getMembers();
	std::set<Clients*>::const_iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->isConnected())
			_server->sendMessage((*it)->getFd(), topic_broadcast);
	}
	
	std::cout << "📝 " << client.getNickname() << " changed topic of " << channel_name << " to: " << new_topic << std::endl;
}

