/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:17:45 by pgrellie          #+#    #+#             */
/*   Updated: 2025/11/13 17:10:23 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <signal.h>

// Client de reference irssi

Server	*g_server = NULL;

void signalHandler(int signal)
{
	if ((signal == SIGINT || signal == SIGQUIT || signal == SIGTERM) && g_server)
	{
		std::cout << "\nReceived signal " << signal << ". Shutting down server gracefully..." << std::endl;
		g_server->stop();
		delete g_server;
		g_server = NULL;
		exit (0);
	}
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}
	int port = std::atoi(argv[1]);
	std::string password = argv[2];
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
		return 1;
	}
	if (password.empty())
	{
		std::cerr << "Error: Password cannot be empty" << std::endl;
		return 1;
	}
	Server *server = new Server(port, password);
	g_server = server;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	try
	{
		server->start();
		server->run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		delete server;
		g_server = NULL;
		return 1;
	}
	delete server;
	g_server = NULL;
	return (0);
}

// Une socket est un point de communication réseau qui permet à deux programmes de communiquer, que ce soit sur la même machine ou à travers un réseau.

// Le format réseau universel est le big-endian pour l'adresse IP et les ports

// Analogie simple :
// Imaginez une prise électrique dans le mur :

// La socket = la prise
// Votre programme = l'appareil que vous branchez
// Le réseau = le système électrique
// Les données = l'électricité qui circule


// main.cpp
// Rôle : Point d'entrée du programme serveur IRC
// Fonction :
// Validation des arguments (port et mot de passe)
// Initialisation et lancement du serveur
// Gestion de la boucle principale du serveur
// Contient déjà les commentaires sur les fonctions réseau nécessaires (socket, bind, listen, accept, etc.)

// Server.cpp
// Rôle : Gestion du serveur IRC principal
// Fonction :
// Création et configuration des sockets
// Écoute des connexions entrantes
// Gestion de la boucle d'événements (probablement avec poll() ou select())
// Gestion des clients connectés
// Coordination entre les différents composants

// Client.cpp
// Rôle : Représentation et gestion des clients connectés
// Fonction :
// Stockage des informations client (nickname, username, channels rejoints, etc.)
// Gestion de l'authentification
// Envoi/réception de messages vers/depuis les clients
// Gestion de l'état des clients (connecté, authentifié, etc.)

// Channel.cpp
// Rôle : Gestion des canaux IRC
// Fonction :
// Création et suppression de canaux
// Gestion des membres du canal
// Gestion des permissions (opérateurs, modes du canal)
// Distribution des messages dans les canaux
// Gestion des modes de canal (+t, +i, +k, +o, +l)

// CommandHandler.cpp
// Rôle : Traitement des commandes IRC
// Fonction :
// Parsing des commandes IRC reçues des clients
// Implémentation des commandes IRC obligatoires : PASS, NICK, USER, JOIN, PART, PRIVMSG, KICK, INVITE, TOPIC, MODE
// Validation des paramètres des commandes
// Dispatch des commandes vers les bonnes méthodes

// Messages.cpp
// Rôle : Gestion des messages et réponses IRC
// Fonction :
// Définition des codes de réponse IRC numériques (001, 002, 403, 404, etc.)
// Formatage des messages de réponse selon le protocole IRC
// Messages d'erreur standardisés
// Messages de confirmation et d'information

// Utils.cpp
// Rôle : Fonctions utilitaires
// Fonction :
// Fonctions de parsing de chaînes
// Validation de formats (nicknames, noms de canaux)
// Fonctions de conversion et de formatage
// Utilitaires réseau et manipulation de données


// socket : crée un point de communication réseau (socket).

// close : ferme un descripteur de fichier (socket ou fichier).

// setsockopt : configure des options sur un socket (ex : réutilisation d’adresse).

// getsockname : récupère l’adresse locale d’un socket.

// getprotobyname : obtient des informations sur un protocole réseau (ex : "tcp").

// gethostbyname : résout un nom d’hôte en adresse IP (obsolète, remplacé par getaddrinfo).

// getaddrinfo : résout un nom d’hôte/service en adresses utilisables par socket.

// freeaddrinfo : libère la mémoire allouée par getaddrinfo.

// bind : associe un socket à une adresse locale (IP/port).

// connect : connecte un socket à une adresse distante.

// listen : met un socket en mode écoute (serveur).

// accept : accepte une connexion entrante sur un socket en écoute.

// htons, htonl : convertit des entiers (16/32 bits) de l’ordre hôte à l’ordre réseau.

// ntohs, ntohl : convertit des entiers de l’ordre réseau à l’ordre hôte.

// inet_addr : convertit une adresse IPv4 en chaîne en entier (obsolète).

// inet_ntoa : convertit une adresse IPv4 en entier en chaîne.

// send : envoie des données sur un socket.

// recv : reçoit des données sur un socket.

// signal, sigaction : gère les signaux (ex : interruption, arrêt).

// lseek : déplace le curseur de lecture/écriture dans un fichier.

// fstat : obtient des informations sur un fichier via son descripteur.

// fcntl : manipule des descripteurs de fichiers (ex : mode non bloquant).

// poll : surveille plusieurs descripteurs pour voir s’ils sont prêts (lecture/écriture).