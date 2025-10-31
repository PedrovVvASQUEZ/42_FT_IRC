/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 18:41:53 by pgrellie          #+#    #+#             */
/*   Updated: 2025/10/23 18:35:29 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include "Server.hpp"

//==================== Contructors N Destructor ===================//

Server::Server(void) : _port(0), _password("Password"), _server_fd(-1), _running(false), _commandHandler(this)
{
	return ;
}

Server::Server(int port, const std::string &password) : _port(port), _password(password), _server_fd(-1), _running(false), _commandHandler(this)
{
	std::memset(&_server_addr, 0, sizeof(_server_addr));
	std::cout << "Server created on port " << _port << std::endl;
}

Server::Server(const Server &rhs) : _port(rhs._port), _password(rhs._password), _server_fd(-1), _running(false), _commandHandler(this)
{
	std::memset(&_server_addr, 0, sizeof(_server_addr));
	std::cout << "Server copied (port " << _port << ")" << std::endl;
}

Server::~Server(void)
{
	if (_running)
		stop();
	std::cout << "Server destroyed" << std::endl;
}

//============= Surcharges d'operateur ===========//

Server	&Server::operator=(const Server &rhs)
{
	if (&rhs != this)
	{
		// Fermer les anciennes connexions avant de copier
		stop();
		_port = rhs._port;
		_password = rhs._password;
		_server_fd = -1;  // Reset - il faudra redémarrer le serveur
		_running = false;  // Reset
		_commandHandler.setServer(this);  // Réinitialiser le CommandHandler
		std::memset(&_server_addr, 0, sizeof(_server_addr));
	}
	return (*this);
}

//==================== Getters  ===================//

int	Server::getServerFd(void) const
{
	return (_server_fd);
}

int	Server::getPort(void) const
{
	return (_port);
}

std::string	Server::getPassword(void) const
{
	return (_password);
}

bool	Server::isRunning(void) const
{
	return (_running);
}

Clients* Server::getClient(int fd)
{
	std::map<int, Clients>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return &(it->second);
	return (NULL);
}

const Clients* Server::getClient(int fd) const
{
	std::map<int, Clients>::const_iterator it = _clients.find(fd);
	if (it != _clients.end())
		return &(it->second);
	return (NULL);
}

Clients* Server::getClientByNickname(const std::string& nickname)
{
	std::map<int, Clients>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return &(it->second);
	}
	return (NULL);
}

const Clients* Server::getClientByNickname(const std::string& nickname) const
{
	std::map<int, Clients>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return &(it->second);
	}
	return (NULL);
}

void	Server::removeClient(int fd)
{
	_clientRemover(fd);
}

void	Server::removeClient(Clients* client)
{
	if (!client)
		return;
	_clientRemover(client->getFd());
}

size_t Server::getClientCount(void) const
{
	return _clients.size();
}

//==================== PRIVATE member functions ===================//

void	Server::_createSocket()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Server FD: " << _server_fd << std::endl;
	if (_server_fd == -1)
	{
		std::cerr << "Error: Failed to create socket" << std::endl;
		exit(1);
	}
	// Option pour réutiliser l'adresse
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error: Failed to set socket options" << std::endl;
		close(_server_fd);
		exit(1);
	}
	// Configurer le socket en mode non-bloquant
	_setNonBlocking(_server_fd);
	std::cout << "Socket created successfully" << std::endl;
}

void	Server::_bindSocket()
{
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces
	_server_addr.sin_port = htons(_port);

	if (bind(_server_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << "Error: Failed to bind socket to port " << _port << std::endl;
		close(_server_fd);
		exit(1);
	}
	std::cout << "Socket bound to port " << _port << std::endl;
}

void	Server::_listenSocket()
{
	if (listen(_server_fd, 5) < 0) // Max 5 connexions en attente
	{
		std::cerr << "Error: Failed to listen on socket" << std::endl;
		close(_server_fd);
		exit(1);
	}
	std::cout << "Server listening on port " << _port << std::endl;
}

void	Server::_setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: Failed to set non-blocking mode for FD " << fd << std::endl;
	}
	else
	{
		std::cout << "FD " << fd << " set to non-blocking mode" << std::endl;
	}
}

void	Server::_acceptNewClient()
{
	struct sockaddr_in	client_addr;
	socklen_t 			client_len = sizeof(client_addr);

	int	new_client_fd = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
	std::cout << "Client FD: " << new_client_fd << std::endl;
	if (new_client_fd < 0)
	{
		std::cerr << "Error: Failed to accept client connection" << std::endl;
		return;
	}
	if (_clients.size() >= MAX_CLIENTS)
	{
		std::cout << "Server is full, new client rejected" << std::endl;
		close(new_client_fd);
		return ;
	}
	// Configurer le socket client en mode non-bloquant
	_setNonBlocking(new_client_fd);
	// Creer nouveau Client
	std::string			hostname = inet_ntoa(client_addr.sin_addr);
	Clients				new_client(new_client_fd, hostname);
	_clients[new_client_fd] = new_client;
	// Ajouteer nouveau Client
	struct pollfd new_client_pfd;
	new_client_pfd.fd = new_client_fd;
	new_client_pfd.events = POLLIN;
	new_client_pfd.revents = 0;
	this->_pfds.push_back(new_client_pfd);
	std::cout << "New client connected! FD: " << new_client_fd << " from " << hostname << std::endl;
	std::cout << "Actual clients: " << _clients.size() << std::endl;
}

void Server::_clientRemover(int client_fd)
{
	// Supprimer de la map des clients
	_clients.erase(client_fd);
	std::vector<struct pollfd>::iterator it;
	for (it = _pfds.begin(); it != _pfds.end(); ++it)
	{
		if (it->fd == client_fd)
		{
			_pfds.erase(it);
			break ;
		}
	}
	close(client_fd);
	std::cout << "Client " << client_fd << " removed. Total clients: " << _clients.size() << std::endl;
}

void Server::_processMessage(Clients& client, const Messages &message)
{
	// Utiliser le CommandHandler pour traiter les commandes IRC
	_commandHandler.processCommand(client, message);
}

void	Server::_handleClientMessage(int client_fd)
{
	char		buffer[512];
	std::memset(buffer, 0, 512);
	
	// Sur Linux : recv() peut retourner EAGAIN en mode non-bloquant
	// Mais avec poll(), on sait déjà que des données sont disponibles
	ssize_t		bytes_received = recv(client_fd, buffer, 511, 0);

	if (bytes_received < 0)
	{
		// Gestion spéciale pour mode non-bloquant
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			// Pas de données disponibles maintenant, on reviendra plus tard
			return;
		}
		std::cerr << "Error receiving from client " << client_fd << ": " << strerror(errno) << std::endl;
		_clientRemover(client_fd);
		return;
	}
	if (bytes_received == 0)
	{
		std::cout << "Client " << client_fd << " disconnected cleanly" << std::endl;
		_clientRemover(client_fd);
		return;
	}
	Clients		*client = getClient(client_fd);
	if (!client)
	{
		std::cerr << "Error: Client not found for fd " << client_fd << std::endl;
		return;
	}
	client->appendToBuffer(std::string(buffer, bytes_received));
	
	// Essayer d'abord \r\n (standard IRC)
	bool found_crlf = _processMessagesFromBuffer(client, client_fd, "\r\n", 2);
	
	// Si aucun \r\n trouvé, essayer \n seul (compatibilité netcat/telnet)
	if (!found_crlf)
		_processMessagesFromBuffer(client, client_fd, "\n", 1);
}

bool	Server::_processMessagesFromBuffer(Clients *client, int client_fd, const std::string &delimiter, int bytes_to_skip)
{
	std::string	&client_buffer = client->getBuffer();
	size_t pos;
	bool found_any = false;
	
	while ((pos = client_buffer.find(delimiter)) != std::string::npos)
	{
		found_any = true;
		std::string raw_message = client_buffer.substr(0, pos);
		client_buffer.erase(0, pos + bytes_to_skip);
		
		// Nettoyer le message (enlever \r final si présent)
		if (!raw_message.empty() && raw_message[raw_message.length() - 1] == '\r')
			raw_message.erase(raw_message.length() - 1);
			
		if (!raw_message.empty())
		{
			std::cout << "Received from client " << client_fd << ": " << raw_message << std::endl;
			
			// Créer un objet Messages pour parser le message IRC
			Messages msg(raw_message);
			if (msg.isValid())
			{
				_processMessage(*client, msg);
			}
			else
			{
				std::cout << "Invalid message format from client " << client_fd << ": " << raw_message << std::endl;
			}
		}
	}
	return found_any;
}

//==================== CLIENT PART ===================//

void	Server::sendMessage(int client_fd, const std::string &message)
{
	Clients	*client = getClient(client_fd);
	if (!client || !client->isConnected())
	{
		std::cerr << "Client: " << client_fd << " not found or disconnected" << std::endl;
		return;
	}
	std::string full_message = message + "\r\n";
	ssize_t bytes_sent = send(client_fd, full_message.c_str(), full_message.length(), 0);
	
	if (bytes_sent < 0)
	{
		// Gestion pour mode non-bloquant
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			// Buffer de sortie plein, on devrait stocker le message pour plus tard
			// Pour l'instant, on ignore (simplifié)
			std::cout << "Send buffer full for client " << client_fd << std::endl;
			return;
		}
		std::cerr << "Error: Failed to send message to client " << client_fd << std::endl;
		_clientRemover(client_fd);
		return;
	}
	else if (bytes_sent < (ssize_t)full_message.length())
	{
		// Envoi partiel - en mode non-bloquant, on devrait gérer ça
		std::cout << "Partial send to client " << client_fd << ": " << bytes_sent << "/" << full_message.length() << std::endl;
	}
	
	std::cout << "Sent to client: " << client_fd << ": " << message << std::endl;
}

void	Server::removeClientFromAllChannels(Clients* client)
{
	if (!client)
		return;
	
	std::map<std::string, Channels>::iterator	it;
	for (it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->second.isMember(client))
		{
			it->second.removeMember(client);
			std::cout << "Client " << client->getNickname() << " removed from Channel " << it->first << std::endl;
		}
	}
}
//==================== SERVER activation N deactivation ===================//


void	Server::start(void)
{
	std::cout << "Starting IRC server..." << std::endl;
	_createSocket();
	_bindSocket();
	_listenSocket();
	// Ajouter le server socket à la liste de surveillance
	struct pollfd server_pollfd;
	server_pollfd.fd = this->_server_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	this->_pfds.push_back(server_pollfd);

	this->_running = true;
	std::cout << "Server ready! Waiting for connections..." << std::endl;
}

void Server::run(void)
{
	while (_running)
	{
		int poll_result = poll(_pfds.data(), _pfds.size(), 100);  // Timeout de 100ms - bon compromis !
		if (poll_result < 0)
		{
			// Si poll() est interrompu par un signal, c'est normal
			if (errno == EINTR)
				continue;
			// Si le serveur est en cours d'arrêt, c'est normal aussi
			if (!_running)
				break;
			std::cerr << "Error: poll() failed" << std::endl;
			break;
		}
		if (poll_result == 0)
		{
			continue;
		}
		// Vérifier chaque file descriptor
		for (size_t i = 0; i < _pfds.size(); ++i)
		{
			if (_pfds[i].revents & POLLIN)
			{
				if (_pfds[i].fd == _server_fd)
				{
					std::cout << "🆕 New connection incoming..." << std::endl;
					_acceptNewClient();
				}
				else
				{
					std::cout << "📨 Message from client FD: " << _pfds[i].fd << std::endl;
					_handleClientMessage(_pfds[i].fd);
				}
			}
			else if (_pfds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
			{
				std::cout << "🚪 Client FD " << _pfds[i].fd << " disconnected (revents: " << _pfds[i].revents << ")" << std::endl;
				if (_pfds[i].fd != _server_fd)
				{
					_clientRemover(_pfds[i].fd);
					--i; // Ajuster l'index car le vecteur a changé
				}
			}
			_pfds[i].revents = 0;  // Nettoyer les events
		}
	}
}

void Server::stop(void)
{
	_running = false;
	
	std::cout << "Cleaning up server resources..." << std::endl;
	
	// Fermer toutes les connexions clients
	std::map<int, Clients>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
	}
	_clients.clear();
	
	// Nettoyer tous les channels
	_channels.clear();
	
	// Nettoyer les descripteurs de poll
	_pfds.clear();
	
	// Fermer le socket serveur
	if (_server_fd != -1)
	{
		close(_server_fd);
		_server_fd = -1;
	}
	
	std::cout << "Server stopped cleanly" << std::endl;
}

//==================== CHANNELS PART ===================//

Channels	*Server::getChannel(const std::string &name)
{
	std::map<std::string, Channels>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return &(it->second);
	return (NULL);
}

const Channels	*Server::getChannel(const std::string &name) const
{
	std::map<std::string, Channels>::const_iterator it = _channels.find(name);
	if (it != _channels.end())
		return &(it->second);
	return (NULL);
}

Channels	*Server::createChannel(const std::string &name)
{
	if (getChannel(name) != NULL)
		return (getChannel(name));
	_channels[name] = Channels(name);
	std::cout << "Channel " << name << " created" << std::endl;
	return (&_channels[name]);
}

void		Server::removeChannel(const std::string &name)
{
	Channels	*to_remove = getChannel(name);
	if (to_remove == NULL)
		return ;
	if (to_remove->isEmpty())
	{
		_channels.erase(name);
		std::cout << "Channel " << name << " removed" << std::endl;
	}
}

//==================== SERVER UTILITIES ===================//

std::string	Server::getServerName(void) const
{
	return ("ft_irc.42.fr");
}

bool		Server::isNicknameInUse(const std::string& nickname) const
{
	std::map<int, Clients>::const_iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return (true);
	}
	return (false);
}
