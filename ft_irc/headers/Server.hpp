/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:10:58 by pgrellie          #+#    #+#             */
/*   Updated: 2025/09/19 18:06:59 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

// Includes essentiels pour un serveur simple
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <errno.h>
# include <iostream>
# include <string>
# include <cstring>
# include <cstdlib>
# include <vector>
# include <map>
# include <algorithm>
# include <poll.h>
# include "Clients.hpp"

# define MAX_CLIENTS 100

class	Server
{
	private:
		// Configuration de base
		int							_port;
		std::string					_password;
		int							_server_fd;
		struct sockaddr_in			_server_addr;
		bool						_running;
		std::vector<struct pollfd>	_pfds;
		std::map<int, Clients>		_clients;

		// Méthodes privées - étapes de setup
		void	_createSocket(void);
		void	_bindSocket(void);
		void	_listenSocket(void);

		void	_acceptNewClient(void);	// Accept UNE connexion
		void	_handleClientMessage(int client_fd);	// Lit et traite UN message
		void	_clientRemover(int client_fd);
		void	_processMessage(Clients &Client, const std::string &message);

		public:
		// Constructeurs / Destructeur
		Server(void);
		Server(int port, const std::string &password);
		Server(const Server &rhs);
		~Server(void);

		// Sucharges d'operateur
		Server	&operator=(const Server &rhs);

		// Getters de base
		int			getServerFd(void) const;
		int			getPort(void) const;
		std::string	getPassword(void) const;
		bool		isRunning(void) const;
		Clients		*getClient(int fd);
		void		removeClient(int fd);
		size_t		getClientCount(void) const;

		// Méthodes principales - version simple
		void	start(void);		// Setup complet du serveur
		void	run(void);
		void	sendMessage(int client_fd, const std::string& message);	// Envoie UN message
		void	broadcastMessage(const std::string &message);
		void	stop(void);			// Ferme les connexions
		
};

#endif

