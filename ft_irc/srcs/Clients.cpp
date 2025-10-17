/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:21 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/03 18:42:17 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Clients.hpp"

//==================== Contructors N Destructor ===================//

Clients::Clients(void) : _cfd(-1), _cstate(CONNECTED)
{
	return ;
}

Clients::Clients(int fd, const std::string &hostnameIP) : _cfd(fd), _hostnameIP(hostnameIP), _cstate(CONNECTED)
{
	return ;
}

Clients::Clients(const Clients &rhs) : _cfd(rhs._cfd), _nickname(rhs._nickname),
										_username(rhs._username), _realname(rhs._realname),
										_hostnameIP(rhs._hostnameIP), _cstate(rhs._cstate),
										_buffer(rhs._buffer)
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

const std::string	&Clients::getRealname(void) const
{
	return (this->_realname);
}

const std::string	&Clients::getBuffer(void) const
{
	return (this->_buffer);
}

std::string	&Clients::getBuffer(void)
{
	return (this->_buffer);
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

void	Clients::clearBuffer(void)
{
	_buffer.clear();
}

//==================== Fonctions publics ===================//

bool	Clients::messageComplete(void) const
{
	return (_buffer.find("\r\n") != std::string::npos);
}

std::string	Clients::extractMessage(void)
{
	size_t	pos = _buffer.find("\r\n");
	if (pos == std::string::npos)
		return ("");
	std::string	message = _buffer.substr(0, pos);
	_buffer.erase(0, pos + 2);
	return (message);
}

bool	Clients::isAuthenticated(void) const
{
	return (this->_cstate == AUTHENTIFICATED);
}

bool	Clients::isConnected(void) const
{
	return (this->_cstate == CONNECTED || this->_cstate == AUTHENTIFICATED);
}