/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Clients.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:11:13 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:17:16 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTS_HPP
# define CLIENTS_HPP

# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>

enum	ClientState
{
	DISCONNECTED,
	CONNECTED,
	AUTHENTIFICATED
};

class	Clients
{
	private:
		int			_cfd;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_hostnameIP;
		ClientState	_cstate;
		std::string	_buffer;
		bool		_passwordValid;

	public:
		Clients(void);
		Clients(int fd, const std::string& hostnameIP);
		Clients(const Clients &rhs);
		~Clients(void);

		Clients& operator=(const Clients &rhs);

		int					getFd(void) const;
		const std::string&	getNickname(void) const;
		const std::string&	getUsername(void) const;
		const std::string&	getHostname(void) const;
		ClientState			getState(void) const;
		const std::string&	getBuffer(void) const;
		std::string&		getBuffer(void);
		bool				isPasswordValid(void) const;

		void	setNickname(const std::string &nickname);
		void	setUsername(const std::string &username);
		void	setRealname(const std::string &realname);
		void	setState(ClientState state);
		void	appendToBuffer(const std::string &data);
		void	setPasswordValid(bool valid);

		bool		messageComplete(void) const;
		std::string	extractMessage(void);

		bool	isConnected(void) const;
};

#endif