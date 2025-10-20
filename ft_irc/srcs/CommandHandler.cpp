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
	// Un nom de channel IRC valide :
	// - Commence par # ou &
	// - 1-50 caractères au total
	// - Pas d'espaces, virgules ou caractères de contrôle
	
	if (channel.empty() || channel.length() > 50)
		return false;
		
	if (channel[0] != '#' && channel[0] != '&')
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
	
	// Convertir la commande en majuscules pour la comparaison
	command = _toUpper(command);
	
	// Dispatcher vers les bonnes fonctions de commande
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
	{
		// Commande inconnue
		_sendNumericReply(client, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
	}
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
	
	// 001 RPL_WELCOME
	_sendNumericReply(client, RPL_WELCOME, ":Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
	
	// 002 RPL_YOURHOST  
	_sendNumericReply(client, RPL_YOURHOST, ":Your host is " + _server->getServerName() + ", running version 1.0");
	
	// 003 RPL_CREATED
	_sendNumericReply(client, RPL_CREATED, ":This server was created today");
	
	// 004 RPL_MYINFO
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
	
	// Si déjà authentifié, erreur
	if (client.getState() == AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return;
	}
	// Vérifier le mot de passe
	if (password == _server->getPassword())
	{
		// Mot de passe correct - le client peut maintenant s'authentifier
		client.setPasswordValid(true);
		std::cout << "🔐 Client " << client.getFd() << " provided correct password" << std::endl;
		// On ne change pas encore l'état, on attend NICK et USER
	}
	else
	{
		_sendNumericReply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		std::cout << "❌ Client " << client.getFd() << " failed authentication" << std::endl;
		// Marquer le client pour déconnexion
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
	// Valider le nickname
	if (!_isValidNickname(nickname))
	{
		_sendNumericReply(client, ERR_ERRONEUSNICKNAME, nickname + " :Erroneous nickname");
		return;
	}
	// Vérifier si le nickname est déjà utilisé
	if (_server->isNicknameInUse(nickname))
	{
		_sendNumericReply(client, ERR_NICKNAMEINUSE, nickname + " :Nickname is already in use");
		return;
	}
	std::string old_nick = client.getNickname();
	client.setNickname(nickname);
	if (old_nick.empty())
	{
		std::cout << "📛 Client " << client.getFd() << " set nickname to: " << nickname << std::endl;
	}
	else
	{
		std::cout << "📛 Client " << client.getFd() << " changed nickname from " << old_nick << " to " << nickname << std::endl;
		
		// Si le client est authentifié, notifier le changement de nickname
		if (client.getState() == AUTHENTIFICATED)
		{
			std::string nick_change = ":" + old_nick + "!" + client.getUsername() + "@" + client.getHostname() + " NICK " + nickname;
			
			// Envoyer la notification au client lui-même
			_server->sendMessage(client.getFd(), nick_change);
			
			// TODO: Envoyer aux autres clients dans les mêmes channels
			// Pour l'instant, on ne gère que la notification au client qui change
		}
	}
	// Si toutes les infos sont présentes, mot de passe valide et pas encore authentifié, envoyer la séquence de bienvenue
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
	
	// Si déjà authentifié, erreur
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
	
	// Si toutes les infos sont présentes, mot de passe valide et pas encore authentifié, envoyer la séquence de bienvenue
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
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins un paramètre (nom du channel)
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return;
	}
	
	std::string channel_name = message.getParam(0);
	
	// Vérifier que le nom du channel commence par # ou &
	if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	
	// Obtenir ou créer le channel
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		channel = _server->createChannel(channel_name);
		std::cout << "📺 Channel " << channel_name << " created by " << client.getNickname() << std::endl;
	}
	
	// Ajouter le client au channel
	if (channel->addMember(&client))
	{
		// Si c'est le premier membre du channel, en faire un opérateur
		if (channel->getMembersCount() == 1)
		{
			channel->addOperator(&client);
			std::cout << "👑 " << client.getNickname() << " became operator of " << channel_name << " (channel founder)" << std::endl;
		}
		
		std::cout << "📺 " << client.getNickname() << " joined channel " << channel_name << std::endl;
		
		// Envoyer la confirmation JOIN au client
		std::string join_message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " JOIN " + channel_name;
		_server->sendMessage(client.getFd(), join_message);
		
		// Broadcaster le JOIN aux autres membres du channel
		const std::set<Clients*>& members = channel->getMembers();
		std::set<Clients*>::const_iterator it;
		for (it = members.begin(); it != members.end(); ++it)
		{
			// Ne pas envoyer le message au client qui vient de rejoindre
			if (*it != &client && (*it)->isConnected())
			{
				_server->sendMessage((*it)->getFd(), join_message);
			}
		}
		
		// Envoyer le topic du channel (s'il existe)
		if (!channel->getTopic().empty())
		{
			_sendNumericReply(client, RPL_TOPIC, channel_name + " :" + channel->getTopic());
		}
		else
		{
			_sendNumericReply(client, RPL_NOTOPIC, channel_name + " :No topic is set");
		}
		
		// Envoyer la liste des membres du channel
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
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins un paramètre (nom du channel)
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return;
	}
	
	std::string channel_name = message.getParam(0);
	std::string part_message = message.getParamCount() > 1 ? message.getParam(1) : client.getNickname();
	
	// Obtenir le channel
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	
	// Vérifier que le client est membre du channel
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	
	// Construire le message PART à broadcaster
	std::string part_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PART " + channel_name;
	if (part_message != client.getNickname())
		part_broadcast += " :" + part_message;
	
	// Envoyer la confirmation PART au client
	_server->sendMessage(client.getFd(), part_broadcast);
	
	// Broadcaster le PART aux autres membres du channel
	const std::set<Clients*>& members = channel->getMembers();
	std::set<Clients*>::const_iterator it;
	for (it = members.begin(); it != members.end(); ++it)
	{
		if (*it != &client && (*it)->isConnected())
		{
			_server->sendMessage((*it)->getFd(), part_broadcast);
		}
	}
	
	// Retirer le client du channel
	if (channel->removeMember(&client))
	{
		std::cout << "📺 " << client.getNickname() << " left channel " << channel_name << std::endl;
		
		// Si le channel est vide, le supprimer
		if (channel->isEmpty())
		{
			_server->removeChannel(channel_name);
		}
	}
}

void CommandHandler::_handlePrivmsg(Clients& client, const Messages& message)
{
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins 2 paramètres (target et message)
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
	
	// Construire le message à envoyer
	std::string privmsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " PRIVMSG " + target + " :" + msg_text;
	
	// Déterminer si c'est un channel ou un utilisateur privé
	if (target[0] == '#' || target[0] == '&')
	{
		// Message vers un channel
		Channels* channel = _server->getChannel(target);
		if (!channel)
		{
			_sendNumericReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
			return;
		}
		
		// Vérifier que le client est membre du channel
		if (!channel->isMember(&client))
		{
			_sendNumericReply(client, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
			return;
		}
		
		// Envoyer le message à tous les membres du channel sauf l'expéditeur
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
		// Message privé vers un utilisateur
		Clients* target_client = _server->getClientByNickname(target);
		
		if (!target_client || !target_client->isConnected())
		{
			_sendNumericReply(client, ERR_NOSUCHNICK, target + " :No such nick/channel");
			return;
		}
		
		// Envoyer le message privé
		_server->sendMessage(target_client->getFd(), privmsg);
		std::cout << "💬 " << client.getNickname() << " sent private message to " << target << std::endl;
	}
}

// Stubs pour les autres commandes
void CommandHandler::_handleKick(Clients& client, const Messages& message)
{
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins 2 paramètres (channel et nickname)
	if (message.getParamCount() < 2)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
		return;
	}
	
	std::string channel_name = message.getParam(0);
	std::string target_nick = message.getParam(1);
	std::string kick_reason = message.getParamCount() > 2 ? message.getParam(2) : client.getNickname();
	
	// Obtenir le channel
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	
	// Vérifier que le client est membre du channel
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	
	// Vérifier que le client est opérateur du channel
	if (!channel->isOperator(&client))
	{
		_sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channel_name + " :You're not channel operator");
		return;
	}
	
	// Trouver l'utilisateur cible
	Clients* target_client = _server->getClientByNickname(target_nick);
	if (!target_client)
	{
		_sendNumericReply(client, ERR_NOSUCHNICK, target_nick + " :No such nick/channel");
		return;
	}
	
	// Vérifier que la cible est dans le channel
	if (!channel->isMember(target_client))
	{
		_sendNumericReply(client, ERR_USERNOTINCHANNEL, target_nick + " " + channel_name + " :They aren't on that channel");
		return;
	}
	
	// Construire le message KICK à broadcaster
	std::string kick_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " KICK " + channel_name + " " + target_nick + " :" + kick_reason;
	
	// Broadcaster le KICK à tous les membres du channel (y compris la cible)
	const std::set<Clients*>& members = channel->getMembers();
	std::set<Clients*>::const_iterator it;
	
	for (it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->isConnected())
		{
			_server->sendMessage((*it)->getFd(), kick_broadcast);
		}
	}
	
	// Retirer la cible du channel
	channel->removeMember(target_client);
	
	std::cout << "⚡ " << client.getNickname() << " kicked " << target_nick << " from " << channel_name << " (" << kick_reason << ")" << std::endl;
	
	// Si le channel est vide, le supprimer
	if (channel->isEmpty())
	{
		_server->removeChannel(channel_name);
	}
}

void CommandHandler::_handleInvite(Clients& client, const Messages& message)
{
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins 2 paramètres (nickname et channel)
	if (message.getParamCount() < 2)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
		return;
	}
	
	std::string target_nick = message.getParam(0);
	std::string channel_name = message.getParam(1);
	
	// Trouver l'utilisateur cible
	Clients* target_client = _server->getClientByNickname(target_nick);
	if (!target_client)
	{
		_sendNumericReply(client, ERR_NOSUCHNICK, target_nick + " :No such nick/channel");
		return;
	}
	
	// Obtenir le channel
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	
	// Vérifier que le client est membre du channel
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	
	// Vérifier que la cible n'est pas déjà dans le channel
	if (channel->isMember(target_client))
	{
		_sendNumericReply(client, ERR_USERONCHANNEL, target_nick + " " + channel_name + " :is already on channel");
		return;
	}
	
	// Construire le message INVITE à envoyer à la cible
	std::string invite_message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " INVITE " + target_nick + " :" + channel_name;
	
	// Envoyer l'invitation à la cible
	_server->sendMessage(target_client->getFd(), invite_message);
	
	// Confirmer l'invitation à l'expéditeur
	_sendNumericReply(client, RPL_INVITING, target_nick + " " + channel_name);
	
	std::cout << "✉️ " << client.getNickname() << " invited " << target_nick << " to " << channel_name << std::endl;
}

void CommandHandler::_handleMode(Clients& client, const Messages& message)
{
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins un paramètre (target)
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
		return;
	}
	
	std::string target = message.getParam(0);
	
	// Si c'est un channel (commence par # ou &)
	if (target[0] == '#' || target[0] == '&')
	{
		// Obtenir le channel
		Channels* channel = _server->getChannel(target);
		if (!channel)
		{
			_sendNumericReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
			return;
		}
		
		// Vérifier que le client est membre du channel
		if (!channel->isMember(&client))
		{
			_sendNumericReply(client, ERR_NOTONCHANNEL, target + " :You're not on that channel");
			return;
		}
		
		// Si pas de mode spécifié, afficher les modes actuels
		if (message.getParamCount() == 1)
		{
			// Pour l'instant, pas de modes implémentés
			_sendNumericReply(client, RPL_CHANNELMODEIS, target + " +");
			return;
		}
		
		std::string mode_string = message.getParam(1);
		
		// Mode simple : +o/-o pour opérateur
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
			
			// Vérifier que le client est opérateur du channel
			if (!channel->isOperator(&client))
			{
				_sendNumericReply(client, ERR_CHANOPRIVSNEEDED, target + " :You're not channel operator");
				return;
			}
			
			if (mode_string == "+o")
			{
				if (channel->addOperator(target_client))
				{
					std::cout << "🔧 " << client.getNickname() << " gave operator to " << target_nick << " in " << target << std::endl;
				}
				else
				{
					std::cout << "⚠️  Failed to give operator to " << target_nick << " (already operator or not member)" << std::endl;
					return;
				}
			}
			else
			{
				if (channel->removeOperator(target_client))
				{
					std::cout << "🔧 " << client.getNickname() << " removed operator from " << target_nick << " in " << target << std::endl;
				}
				else
				{
					std::cout << "⚠️  Cannot remove last operator from " << target << std::endl;
					return;
				}
			}
			
			// Broadcaster le changement de mode
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
		{
			std::cout << "🔧 MODE " << mode_string << " not implemented yet" << std::endl;
			// Autres modes non implémentés pour l'instant
		}
	}
	else
	{
		// Mode utilisateur - pas implémenté pour l'instant
		std::cout << "🔧 User MODE not implemented yet" << std::endl;
	}
}

void CommandHandler::_handleTopic(Clients& client, const Messages& message)
{
	// Vérifier que le client est authentifié
	if (client.getState() != AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	// Vérifier qu'on a au moins un paramètre (nom du channel)
	if (message.getParamCount() < 1)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
		return;
	}
	
	std::string channel_name = message.getParam(0);
	
	// Obtenir le channel
	Channels* channel = _server->getChannel(channel_name);
	if (!channel)
	{
		_sendNumericReply(client, ERR_NOSUCHCHANNEL, channel_name + " :No such channel");
		return;
	}
	
	// Vérifier que le client est membre du channel
	if (!channel->isMember(&client))
	{
		_sendNumericReply(client, ERR_NOTONCHANNEL, channel_name + " :You're not on that channel");
		return;
	}
	
	// Si pas de deuxième paramètre, afficher le topic actuel
	if (message.getParamCount() == 1)
	{
		if (channel->getTopic().empty())
		{
			_sendNumericReply(client, RPL_NOTOPIC, channel_name + " :No topic is set");
		}
		else
		{
			_sendNumericReply(client, RPL_TOPIC, channel_name + " :" + channel->getTopic());
		}
		return;
	}
	
	// Sinon, changer le topic (vérifier les permissions)
	std::string new_topic = message.getParam(1);
	
	// Vérifier que le client est opérateur du channel pour changer le topic
	if (!channel->isOperator(&client))
	{
		_sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channel_name + " :You're not channel operator");
		return;
	}
	
	channel->setTopic(new_topic);
	
	// Construire le message TOPIC à broadcaster
	std::string topic_broadcast = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " TOPIC " + channel_name + " :" + new_topic;
	
	// Broadcaster le changement de topic à tous les membres du channel
	const std::set<Clients*>& members = channel->getMembers();
	std::set<Clients*>::const_iterator it;
	
	for (it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->isConnected())
		{
			_server->sendMessage((*it)->getFd(), topic_broadcast);
		}
	}
	
	std::cout << "📝 " << client.getNickname() << " changed topic of " << channel_name << " to: " << new_topic << std::endl;
}

