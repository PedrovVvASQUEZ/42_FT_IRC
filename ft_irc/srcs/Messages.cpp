/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:45 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/14 18:37:30 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Messages.hpp"

// Constructeurs
Messages::Messages() : _isValid(false)
{
}

Messages::Messages(const std::string& rawMessage) : _rawMessage(rawMessage), _isValid(false)
{
	_parseMessage();
}

Messages::Messages(const Messages& other)
{
	*this = other;
}

// Destructeur
Messages::~Messages()
{
}

// Opérateur d'assignation
Messages& Messages::operator=(const Messages& other)
{
	if (this != &other)
	{
		_rawMessage = other._rawMessage;
		_prefix = other._prefix;
		_command = other._command;
		_params = other._params;
		_isValid = other._isValid;
	}
	return *this;
}

// Méthodes privées
void Messages::_parseMessage()
{
	if (_rawMessage.empty())
		return;

	std::string message = _rawMessage;
	
	// Enlever le CRLF de fin si présent
	if (message.size() >= 2 && message.substr(message.size() - 2) == "\r\n")
		message = message.substr(0, message.size() - 2);
	else if (message.size() >= 1 && (message[message.size() - 1] == '\r' || message[message.size() - 1] == '\n'))
		message = message.substr(0, message.size() - 1);

	size_t pos = 0;

	// 1. Parser le préfixe (optionnel, commence par ':')
	if (message[0] == ':')
	{
		pos = message.find(' ');
		if (pos == std::string::npos)
			return; // Message invalide
		_prefix = message.substr(1, pos - 1);
		pos++;
	}

	// 2. Parser la commande
	size_t commandStart = pos;
	pos = message.find(' ', commandStart);
	if (pos == std::string::npos)
	{
		// Pas de paramètres
		_command = message.substr(commandStart);
		_isValid = _validateCommand();
		return;
	}
	
	_command = message.substr(commandStart, pos - commandStart);
	pos++;

	// 3. Parser les paramètres
	while (pos < message.size())
	{
		if (message[pos] == ':')
		{
			// Paramètre trailing (le reste du message)
			_params.push_back(message.substr(pos + 1));
			break;
		}
		else
		{
			// Paramètre normal (jusqu'au prochain espace)
			size_t paramEnd = message.find(' ', pos);
			if (paramEnd == std::string::npos)
			{
				_params.push_back(message.substr(pos));
				break;
			}
			else
			{
				_params.push_back(message.substr(pos, paramEnd - pos));
				pos = paramEnd + 1;
			}
		}
	}

	_isValid = _validateCommand();
}

bool Messages::_validateCommand() const
{
	if (_command.empty())
		return false;

	// Commandes IRC standard
	if (_command == "PASS" || _command == "NICK" || _command == "USER" ||
		_command == "JOIN" || _command == "PART" || _command == "QUIT" ||
		_command == "PRIVMSG" || _command == "NOTICE" || _command == "KICK" ||
		_command == "MODE" || _command == "TOPIC" || _command == "INVITE" ||
		_command == "PING" || _command == "PONG" || _command == "WHO" ||
		_command == "WHOIS" || _command == "LIST" || _command == "NAMES")
		return true;

	// Réponses numériques (001-999)
	if (_command.size() == 3)
	{
		for (size_t i = 0; i < 3; i++)
		{
			if (!std::isdigit(_command[i]))
				return false;
		}
		return true;
	}

	return false;
}

std::string Messages::_trim(const std::string& str) const
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return "";
	
	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}

// Getters
std::string Messages::getRawMessage() const
{
	return _rawMessage;
}

std::string Messages::getPrefix() const
{
	return _prefix;
}

std::string Messages::getCommand() const
{
	return _command;
}

std::vector<std::string> Messages::getParams() const
{
	return _params;
}

std::string Messages::getParam(size_t index) const
{
	if (index < _params.size())
		return _params[index];
	return "";
}

size_t Messages::getParamCount() const
{
	return _params.size();
}

bool Messages::isValid() const
{
	return _isValid;
}

// Méthodes utilitaires
bool Messages::hasPrefix() const
{
	return !_prefix.empty();
}

bool Messages::isNumericReply() const
{
	return _command.size() == 3 && std::isdigit(_command[0]) && 
		   std::isdigit(_command[1]) && std::isdigit(_command[2]);
}

std::string Messages::toString() const
{
	std::string result;
	
	if (!_prefix.empty())
		result += ":" + _prefix + " ";
	
	result += _command;
	
	for (size_t i = 0; i < _params.size(); i++)
	{
		result += " ";
		if (i == _params.size() - 1 && _params[i].find(' ') != std::string::npos)
			result += ":" + _params[i];
		else
			result += _params[i];
	}
	
	return result;
}

// Méthodes statiques pour créer des messages
Messages Messages::createMessage(const std::string& command, const std::vector<std::string>& params)
{
	Messages msg;
	msg._command = command;
	msg._params = params;
	msg._isValid = msg._validateCommand();
	msg._rawMessage = msg.toString();
	return msg;
}

Messages Messages::createReply(const std::string& prefix, const std::string& numeric, 
							const std::string& target, const std::string& message)
{
	Messages msg;
	msg._prefix = prefix;
	msg._command = numeric;
	msg._params.push_back(target);
	if (!message.empty())
		msg._params.push_back(message);
	msg._isValid = msg._validateCommand();
	msg._rawMessage = msg.toString();
	return msg;
}
