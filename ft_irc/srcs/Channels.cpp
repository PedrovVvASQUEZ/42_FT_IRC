/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:08 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/08 16:19:33 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channels.hpp"

//==================== Contructors N Destructor ===================//

Channels::Channels(void) : _name("No name"), _topic("No topic")
{
	return ;
}

Channels::Channels(const std::string &name) : _name(name), _topic("No topic")
{
	return ;
}

Channels::Channels(const Channels &rhs) : _name(rhs._name), _topic(rhs._topic), _members(rhs._members)
{
	return ;
}

Channels::~Channels(void)
{
	return ;
}

//============= Surcharges d'operateur ===========//

Channels	&Channels::operator=(const Channels &rhs)
{
	if (&rhs != this)
	{
		_name = rhs._name;
		_topic = rhs._topic;
		_members = rhs._members;
	}
	return (*this);
}

//==================== Getters ===================//


const std::string	&Channels::getName(void) const
{
	return (_name);
}

const std::string	&Channels::getTopic(void) const
{
	return (_topic);
}

const std::vector<int>	&Channels::getMembers(void) const
{
	return (_members);
}

size_t				Channels::getMembersCount(void) const
{
	return (_members.size());
}

//==================== Setters ===================//

void	Channels::setTopic(const std::string &topic)
{
	_topic = topic;
}

//==================== Fonctions publics ===================//

bool	Channels::addMember(int clientFd)
{
	if (isMember(clientFd))
		return (false);
	_members.push_back(clientFd);
	return (true);
}

bool	Channels::removeMember(int clientFd)
{
	std::vector<int>::iterator it = std::find(_members.begin(), _members.end(), clientFd);
	if (it != _members.end())
	{
		_members.erase(it);
		return (true);
	}
	return (false);
}

bool	Channels::isMember(int clientFd) const
{
	return (std::find(_members.begin(), _members.end(), clientFd) != _members.end());
}

bool	Channels::isEmpty(void) const
{
	return (_members.empty());
}