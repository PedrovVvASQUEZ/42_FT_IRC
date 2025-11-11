/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:10:58 by pgrellie          #+#    #+#             */
/*   Updated: 2025/11/11 17:49:34 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <iostream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <map>
# include <algorithm>
# include <poll.h>
# include <fcntl.h>
# include <stdexcept>
# include "Clients.hpp"
# include "Channels.hpp"
# include "CommandHandler.hpp"
# include "Messages.hpp"

# define MAX_CLIENTS 100

class	Server
{
	private:

		int								_port;
		std::string						_password;
		int								_server_fd;
		struct sockaddr_in				_server_addr;
		bool							_running;
		CommandHandler					_commandHandler;
		std::vector<struct pollfd>		_pfds;
		std::map<int, Clients>			_clients;
		std::map<std::string, Channels>	_channels;

		void	_createSocket(void);
		void	_bindSocket(void);
		void	_listenSocket(void);
		void	_setNonBlocking(int fd);

		void	_acceptNewClient(void);
		void	_handleClientMessage(int client_fd);
		void	_clientRemover(int client_fd);
		void	_processMessage(Clients &Client, const Messages &message);
		bool	_processMessagesFromBuffer(Clients *client, int client_fd, const std::string &delimiter, int bytes_to_skip);

	public:

		Server(void);
		Server(int port, const std::string &password);
		Server(const Server &rhs);
		~Server(void);

		Server	&operator=(const Server &rhs);

		std::string		getServerName(void) const;
		std::string		getPassword(void) const;
		bool			isRunning(void) const;
		Clients			*getClient(int fd);
		const Clients	*getClient(int fd) const;
		Clients			*getClientByNickname(const std::string &nickname);
		const Clients	*getClientByNickname(const std::string &nickname) const;
		void			removeClient(int fd);

		void	start(void);
		void	run(void);
		void	stop(void);

		// CLIENT part
		void	sendMessage(int client_fd, const std::string& message);
		void	removeClientFromAllChannels(Clients* client);

		// CHANNELS part
		Channels		*getChannel(const std::string &name);
		const Channels	*getChannel(const std::string &name) const;
		Channels		*createChannel(const std::string &name);
		void			removeChannel(const std::string &name);

		bool	isNicknameInUse(const std::string& nickname) const;
};

#endif

