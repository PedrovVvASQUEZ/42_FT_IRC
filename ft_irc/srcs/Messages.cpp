/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:45 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:17:16 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Messages.hpp"

// Constructeurs
Messages::Messages() : _isValid(false)
{
	return ;
}

Messages::Messages(const std::string& rawMessage) : _rawMessage(rawMessage), _isValid(false)
{
	_parseMessage();
	return ;
}

Messages::Messages(const Messages& other)
{
	*this = other;
	return ;
}

// Destructeur
Messages::~Messages()
{
	return ;
}

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
	return (*this);
}

void Messages::_parseMessage()
{
	if (_rawMessage.empty())
		return ;
	std::string message = _rawMessage;
	if (message.size() >= 2 && message.substr(message.size() - 2) == "\r\n")
		message = message.substr(0, message.size() - 2);
	else if (message.size() >= 1 && (message[message.size() - 1] == '\r' || message[message.size() - 1] == '\n'))
		message = message.substr(0, message.size() - 1);
	size_t pos = 0;
	if (message[0] == ':')
	{
		pos = message.find(' ');
		if (pos == std::string::npos)
			return ;
		_prefix = message.substr(1, pos - 1);
		pos++;
	}
	size_t commandStart = pos;
	pos = message.find(' ', commandStart);
	if (pos == std::string::npos)
	{
		_command = message.substr(commandStart);
		_isValid = _validateCommand();
		return ;
	}
	_command = message.substr(commandStart, pos - commandStart);
	pos++;
	while (pos < message.size())
	{
		if (message[pos] == ':')
		{
			_params.push_back(message.substr(pos + 1));
			break ;
		}
		else
		{
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
		return (false);
	if (_command == "PASS" || _command == "NICK" || _command == "USER" ||
		_command == "JOIN" || _command == "PART" || _command == "QUIT" ||
		_command == "PRIVMSG" || _command == "NOTICE" || _command == "KICK" ||
		_command == "MODE" || _command == "TOPIC" || _command == "INVITE" ||
		_command == "PING")
		return (true);
	return (false);
}

std::string Messages::_trim(const std::string& str) const
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == std::string::npos)
		return ("");
	size_t end = str.find_last_not_of(" \t\r\n");
	return (str.substr(start, end - start + 1));
}

// Getters
std::string Messages::getRawMessage() const
{
	return (_rawMessage);
}

std::string Messages::getPrefix() const
{
	return (_prefix);
}

std::string Messages::getCommand() const
{
	return (_command);
}

std::vector<std::string> Messages::getParams() const
{
	return (_params);
}

std::string Messages::getParam(size_t index) const
{
	if (index < _params.size())
		return (_params[index]);
	return ("");
}

size_t Messages::getParamCount() const
{
	return (_params.size());
}

bool Messages::isValid() const
{
	return (_isValid);
}
