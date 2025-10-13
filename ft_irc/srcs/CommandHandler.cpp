/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:34 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/08 18:29:02 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

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

void CommandHandler::processCommand(Clients& client, const std::string& message)
{
	if (!_server)
	{
		std::cerr << "Error: CommandHandler not properly initialized with server" << std::endl;
		return;
	}
	
	// Nettoyer le message (supprimer \r\n)
	std::string clean_message = message;
	if (!clean_message.empty() && clean_message[clean_message.length() - 1] == '\n')
		clean_message.erase(clean_message.length() - 1);
	if (!clean_message.empty() && clean_message[clean_message.length() - 1] == '\r')
		clean_message.erase(clean_message.length() - 1);
	
	if (clean_message.empty())
		return;
	
	std::cout << "📨 Processing command from client " << client.getFd() << ": " << clean_message << std::endl;
	
	// Parser la commande IRC : COMMAND param1 param2 :trailing param
	std::string command;
	std::string params;
	
	size_t space_pos = clean_message.find(' ');
	if (space_pos != std::string::npos)
	{
		command = _toUpper(clean_message.substr(0, space_pos));
		params = clean_message.substr(space_pos + 1);
	}
	else
	{
		command = _toUpper(clean_message);
		params = "";
	}
	
	// Dispatcher vers les bonnes fonctions de commande
	if (command == "PASS")
		_handlePass(client, params);
	else if (command == "NICK")
		_handleNick(client, params);
	else if (command == "USER")
		_handleUser(client, params);
	else if (command == "JOIN")
		_handleJoin(client, params);
	else if (command == "PART")
		_handlePart(client, params);
	else if (command == "PRIVMSG")
		_handlePrivmsg(client, params);
	else if (command == "QUIT")
		_handleQuit(client, params);
	else if (command == "KICK")
		_handleKick(client, params);
	else if (command == "INVITE")
		_handleInvite(client, params);
	else if (command == "MODE")
		_handleMode(client, params);
	else if (command == "TOPIC")
		_handleTopic(client, params);
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
	oss << ":irc.server.local " << std::setfill('0') << std::setw(3) << code << " " << target << " " << message << "\r\n";
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
	_sendNumericReply(client, RPL_YOURHOST, ":Your host is irc.server.local, running version 1.0");
	
	// 003 RPL_CREATED
	_sendNumericReply(client, RPL_CREATED, ":This server was created today");
	
	// 004 RPL_MYINFO
	_sendNumericReply(client, RPL_MYINFO, "irc.server.local 1.0 o o");
	
	std::cout << "✅ Welcome sequence sent to " << nick << std::endl;
}

//==================== Commandes d'authentification ===================//

void CommandHandler::_handlePass(Clients& client, const std::string& params)
{
	if (params.empty())
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
		return;
	}
	
	// Si déjà authentifié, erreur
	if (client.getState() == AUTHENTIFICATED)
	{
		_sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
		return;
	}
	
	// Vérifier le mot de passe
	if (params == _server->getPassword())
	{
		// Mot de passe correct - le client peut maintenant s'authentifier
		std::cout << "🔐 Client " << client.getFd() << " provided correct password" << std::endl;
		// On ne change pas encore l'état, on attend NICK et USER
	}
	else
	{
		_sendNumericReply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
		std::cout << "❌ Client " << client.getFd() << " failed authentication" << std::endl;
		// On pourrait fermer la connexion ici si on veut être strict
	}
}

void CommandHandler::_handleNick(Clients& client, const std::string& params)
{
	if (params.empty())
	{
		_sendNumericReply(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
		return;
	}
	
	// Extraire le nickname (premier paramètre)
	std::string nickname = params;
	size_t space_pos = nickname.find(' ');
	if (space_pos != std::string::npos)
		nickname = nickname.substr(0, space_pos);
	
	// Valider le nickname
	if (!_isValidNickname(nickname))
	{
		_sendNumericReply(client, ERR_ERRONEUSNICKNAME, nickname + " :Erroneous nickname");
		return;
	}
	
	// Vérifier si le nickname est déjà utilisé
	// TODO: Implémenter la vérification d'unicité via le serveur
	
	std::string old_nick = client.getNickname();
	client.setNickname(nickname);
	
	if (old_nick.empty())
	{
		std::cout << "📛 Client " << client.getFd() << " set nickname to: " << nickname << std::endl;
	}
	else
	{
		std::cout << "📛 Client " << client.getFd() << " changed nickname from " << old_nick << " to " << nickname << std::endl;
		// Notifier le changement de nick aux autres clients si nécessaire
	}
	
	// Si toutes les infos sont présentes et pas encore authentifié, envoyer la séquence de bienvenue
	if (!client.getNickname().empty() && !client.getUsername().empty() && client.getState() != AUTHENTIFICATED)
	{
		client.setState(AUTHENTIFICATED);
		_sendWelcomeSequence(client);
	}
}

void CommandHandler::_handleUser(Clients& client, const std::string& params)
{
	if (params.empty())
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
	
	// Parser: USER username hostname servername :realname
	std::vector<std::string> tokens = _split(params, ' ');
	if (tokens.size() < 4)
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
		return;
	}
	
	std::string username = tokens[0];
	std::string realname;
	
	// Chercher le realname après ':'
	size_t colon_pos = params.find(':');
	if (colon_pos != std::string::npos)
		realname = params.substr(colon_pos + 1);
	else
		realname = "Unknown";
	
	client.setUsername(username);
	client.setRealname(realname);
	
	std::cout << "👤 Client " << client.getFd() << " set user info: " << username << " (" << realname << ")" << std::endl;
	
	// Si toutes les infos sont présentes et pas encore authentifié, envoyer la séquence de bienvenue
	if (!client.getNickname().empty() && !client.getUsername().empty() && client.getState() != AUTHENTIFICATED)
	{
		client.setState(AUTHENTIFICATED);
		_sendWelcomeSequence(client);
	}
}

void CommandHandler::_handleQuit(Clients& client, const std::string& params)
{
	std::string quit_message = "Quit";
	
	// Chercher le message après ':'
	size_t colon_pos = params.find(':');
	if (colon_pos != std::string::npos)
		quit_message = params.substr(colon_pos + 1);
	
	std::cout << "👋 Client " << client.getNickname() << " (" << client.getFd() << ") is quitting: " << quit_message << std::endl;
	
	// Notifier les autres clients dans les channels
	// TODO: Implémenter la notification aux channels
	
	// Fermer la connexion
	_server->removeClient(client.getFd());
}

//==================== Commandes de channels (stubs) ===================//

void CommandHandler::_handleJoin(Clients& client, const std::string& params)
{
	if (!client.isAuthenticated())
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	if (params.empty())
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
		return;
	}
	
	// TODO: Implémenter JOIN avec la classe Channels
	std::cout << "📺 JOIN command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

void CommandHandler::_handlePart(Clients& client, const std::string& params)
{
	if (!client.isAuthenticated())
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	if (params.empty())
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
		return;
	}
	
	// TODO: Implémenter PART avec la classe Channels
	std::cout << "📺 PART command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

void CommandHandler::_handlePrivmsg(Clients& client, const std::string& params)
{
	if (!client.isAuthenticated())
	{
		_sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}
	
	if (params.empty())
	{
		_sendNumericReply(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
		return;
	}
	
	// TODO: Implémenter PRIVMSG avec la classe Channels
	std::cout << "💬 PRIVMSG command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

// Stubs pour les autres commandes
void CommandHandler::_handleKick(Clients& client, const std::string& params)
{
	std::cout << "⚡ KICK command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

void CommandHandler::_handleInvite(Clients& client, const std::string& params)
{
	std::cout << "✉️ INVITE command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

void CommandHandler::_handleMode(Clients& client, const std::string& params)
{
	std::cout << "🔧 MODE command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

void CommandHandler::_handleTopic(Clients& client, const std::string& params)
{
	std::cout << "📝 TOPIC command from " << client.getNickname() << ": " << params << " (not implemented yet)" << std::endl;
}

