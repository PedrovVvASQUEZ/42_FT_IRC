/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:11:27 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/08 15:55:21 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include <string>
# include <vector>
# include <algorithm>

class	Channels
{
	private:
		std::string			_name;		// Nom du canal (#general)
		std::string			_topic;		// Sujet du canal
		std::vector<int>	_members;	// Liste des FDs des clients

	public:
		// Constructeurs/Destructeur
		Channels(void);
		Channels(const std::string &name);
		Channels(const Channels &rhs);
		~Channels(void);

		Channels	&operator=(const Channels &rhs);
	
		// Getters
		const std::string		&getName(void) const;
		const std::string		&getTopic(void) const;
		const std::vector<int>	&getMembers(void) const;
		size_t					getMembersCount(void) const;

		// Setters
		void	setTopic(const std::string &topic);

		// Gestion des membres
		bool	addMember(int clientFd);
		bool	removeMember(int clientFd);
		bool	isMember(int clientFd) const;
		bool	isEmpty(void) const;
};

#endif