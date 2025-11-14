/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:11:27 by pgrellie          #+#    #+#             */
/*   Updated: 2025/11/11 19:00:46 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include "Clients.hpp"
# include <string>
# include <set>
# include <algorithm>

class Server;

class	Channels
{
	private:
		std::string			_name;
		std::string			_topic;
		std::set<Clients *>	_members;
		std::set<Clients *>	_operators;

		bool				_inviteOnly;
		bool				_topicRestricted;
		std::string			_key;
		size_t				_userLimit;
		std::set<Clients *>	_inviteList;

	public:

		public:
		Channels(void);
		Channels(const std::string &name);
		Channels(const Channels &rhs);
		~Channels(void);

		Channels	&operator=(const Channels &rhs);
	
		// Getters
		const std::string			&getName(void) const;
		const std::string			&getTopic(void) const;
		const std::set<Clients*>	&getMembers(void) const;
		const std::set<Clients*>	&getOperators(void) const;
		size_t						getMembersCount(void) const;

		void	setTopic(const std::string &topic);
		
		bool	isInviteOnly(void) const;
		void	setInviteOnly(bool invite);
		bool	isTopicRestricted(void) const;
		void	setTopicRestricted(bool restricted);
		const std::string	&getKey(void) const;
		void	setKey(const std::string &key);
		bool	hasKey(void) const;
		size_t	getUserLimit(void) const;
		void	setUserLimit(size_t limit);
		bool	hasUserLimit(void) const;
		
		void	addToInviteList(Clients* client);
		void	removeFromInviteList(Clients* client);
		bool	isInvited(Clients* client) const;
		void	clearInviteList(void);

		bool	addMember(Clients* client);
		bool	removeMember(Clients* client);
		bool	isMember(Clients* client) const;
		bool	isEmpty(void) const;

		bool	addOperator(Clients* client);
		bool	removeOperator(Clients* client);
		bool	isOperator(Clients* client) const;

		std::string	getMembersList(Server* server) const;
		std::string	getModeString(void) const;
};

#endif