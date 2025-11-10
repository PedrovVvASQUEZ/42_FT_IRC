/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:08 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:17:16 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channels.hpp"
#include <iostream>

//==================== Contructors N Destructor ===================//

Channels::Channels(void) : _name("No name"), _topic("No topic")
{
	return ;
}

Channels::Channels(const std::string &name) : _name(name), _topic("No topic")
{
	return ;
}

Channels::Channels(const Channels &rhs) : _name(rhs._name), _topic(rhs._topic), _members(rhs._members), _operators(rhs._operators)
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
		_operators = rhs._operators;
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

const std::set<Clients*> &Channels::getMembers(void) const
{
	return (_members);
}

const std::set<Clients*> &Channels::getOperators(void) const
{
	return (_operators);
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

bool	Channels::addMember(Clients* client)
{
	if (!client || isMember(client))
		return (false);
	_members.insert(client);
	return (true);
}

bool	Channels::removeMember(Clients* client)
{
	if (!client)
		return (false);
	if (isOperator(client))
		_operators.erase(client);
	return (_members.erase(client) > 0);
}

bool	Channels::addOperator(Clients* client)
{
	if (!client || !isMember(client))
		return (false);
	_operators.insert(client);
	return (true);
}

bool	Channels::removeOperator(Clients* client)
{
	if (!client)
		return (false);
	if (_operators.size() == 1 && _members.size() > 1 && isOperator(client))
		return (false);
	return (_operators.erase(client) > 0);
}

bool	Channels::isMember(Clients* client) const
{
	if (!client)
		return (false);
	return (_members.find(client) != _members.end());
}

bool	Channels::isOperator(Clients* client) const
{
	if (!client)
		return (false);
	return (_operators.find(client) != _operators.end());
}

bool	Channels::isEmpty(void) const
{
	return (_members.empty());
}

std::string	Channels::getMembersList(void) const
{
	std::string members_list;
	std::set<Clients*>::const_iterator it;
	
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (!members_list.empty())
			members_list += " ";
		
		if (isOperator(*it))
			members_list += "@";
		
		members_list += (*it)->getNickname();
	}
	return members_list;
}