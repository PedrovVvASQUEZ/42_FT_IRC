/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:11:27 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/14 18:41:10 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include "Clients.hpp"
# include <string>
# include <set>
# include <algorithm>

class	Channels
{
	private:
		std::string			_name;		// Nom du canal (#general)
		std::string			_topic;		// Sujet du canal
		std::set<Clients*>	_members;	// Liste des pointeurs vers les clients membres
		std::set<Clients*>	_operators;	// Liste des pointeurs vers les opérateurs

	public:
		// Constructeurs/Destructeur
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

		// Setters
		void	setTopic(const std::string &topic);

		// Gestion des membres
		bool	addMember(Clients* client);
		bool	removeMember(Clients* client);
		bool	isMember(Clients* client) const;
		bool	isMember(int clientFd) const;  // Surcharge pour compatibilité
		bool	isEmpty(void) const;

		// Gestion des opérateurs
		bool	addOperator(Clients* client);
		bool	removeOperator(Clients* client);
		bool	isOperator(Clients* client) const;
		bool	isOperator(int clientFd) const;  // Surcharge pour compatibilité

		// Utilitaires
		std::string	getMembersList(void) const;
		void		broadcastToMembers(const std::string &message, Clients* excludeClient = NULL) const;
		Clients*	findMemberByFd(int fd) const;
};

#endif