/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:21 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:17:16 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Clients.hpp"

//==================== Contructors N Destructor ===================//

Clients::Clients(void) : _cfd(-1), _cstate(CONNECTED), _passwordValid(false)
{
	return ;
}

Clients::Clients(int fd, const std::string &hostnameIP) : _cfd(fd), _hostnameIP(hostnameIP), _cstate(CONNECTED), _passwordValid(false)
{
	return ;
}

Clients::Clients(const Clients &rhs) : _cfd(rhs._cfd), _nickname(rhs._nickname),
										_username(rhs._username), _realname(rhs._realname),
										_hostnameIP(rhs._hostnameIP), _cstate(rhs._cstate),
										_buffer(rhs._buffer), _passwordValid(rhs._passwordValid)
{
	return ;
}

Clients::~Clients(void)
{
	return;
}

//============= Surcharges d'operateur ===========//

Clients	&Clients::operator=(const Clients &rhs)
{
	if (this != &rhs)
	{
		_cfd = rhs._cfd;
		_nickname = rhs._nickname;
		_username = rhs._username;
		_realname = rhs._realname;
		_hostnameIP = rhs._hostnameIP;
		_cstate = rhs._cstate;
		_buffer = rhs._buffer;
		_passwordValid = rhs._passwordValid;
	}
	return (*this);
}

//==================== Getters ===================//

int	Clients::getFd(void) const
{
	return (this->_cfd);
}

ClientState	Clients::getState(void) const
{
	return (this->_cstate);
}

const std::string	&Clients::getHostname(void) const
{
	return (this->_hostnameIP);
}

const std::string	&Clients::getNickname(void) const
{
	return (this->_nickname);
}

const std::string	&Clients::getUsername(void) const
{
	return (this->_username);
}

const std::string	&Clients::getBuffer(void) const
{
	return (this->_buffer);
}

std::string	&Clients::getBuffer(void)
{
	return (this->_buffer);
}

bool	Clients::isPasswordValid(void) const
{
	return (this->_passwordValid);
}

//==================== Setters ===================//

void	Clients::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void	Clients::setUsername(const std::string &username)
{
	_username = username;
}

void	Clients::setRealname(const std::string &realname)
{
	_realname = realname;
}

void	Clients::setState(ClientState state)
{
	_cstate = state;
}

void	Clients::appendToBuffer(const std::string &data)
{
	_buffer += data;
}

void	Clients::setPasswordValid(bool valid)
{
	_passwordValid = valid;
}

//==================== Fonctions publics ===================//

bool	Clients::messageComplete(void) const
{
	return (_buffer.find("\r\n") != std::string::npos || _buffer.find("\n") != std::string::npos);
}

std::string	Clients::extractMessage(void)
{
	size_t	pos = _buffer.find("\r\n");
	if (pos != std::string::npos)
	{
		std::string	message = _buffer.substr(0, pos);
		_buffer.erase(0, pos + 2);
		return (message);
	}
	pos = _buffer.find("\n");
	if (pos != std::string::npos)
	{
		std::string	message = _buffer.substr(0, pos);
		_buffer.erase(0, pos + 1);
		return (message);
	}
	return ("");
}

bool	Clients::isConnected(void) const
{
	return (this->_cstate == CONNECTED || this->_cstate == AUTHENTIFICATED);
}