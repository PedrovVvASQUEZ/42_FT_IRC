/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:17:45 by pgrellie          #+#    #+#             */
/*   Updated: 2025/09/17 17:29:02 by pgrellie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// PARTIE SERVEUR:

// Variable			Rôle				Analogie
// _port			Port d'écoute		"Numéro de téléphone"
// _password		Authentification	"Code d'accès"
// _server_fd		Socket principal	"Standard téléphonique"
// _client_fd		Client connecté		"Ligne active"
// _server_addr		Adresse complète	"Adresse postale complète"

// 🔌 Fonction socket()
// Prototype : int socket(int domain, int type, int protocol);

// 1. domain (famille d'adresses) :
// AF_INET : IPv4 (le plus courant)
// AF_INET6 : IPv6
// AF_UNIX : Communication locale (pipes nommés)
// AF_UNSPEC : Non spécifié

// 2. type (type de socket) :
// SOCK_STREAM : TCP (fiable, orienté connexion, ordre garanti)
// SOCK_DGRAM : UDP (rapide, sans connexion, pas de garantie)
// SOCK_RAW : Accès direct aux protocoles (nécessite root)

// 3. protocol :
// 0 : Protocole par défaut (TCP pour STREAM, UDP pour DGRAM)
// IPPROTO_TCP : Force TCP explicitement
// IPPROTO_UDP : Force UDP explicitement

// Valeur de retour :
// Succès : Numéro du file descriptor (≥ 0)
// Échec : -1 (et errno est défini)

// ⚙️ Fonction setsockopt()
// Prototype : int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

// 1. sockfd :
// File descriptor du socket à configurer

// 2. level (niveau du protocole) :
// SOL_SOCKET : Options génériques du socket
// IPPROTO_TCP : Options spécifiques à TCP
// IPPROTO_IP : Options spécifiques à IP

// 3. optname (nom de l'option) :
// Pour SOL_SOCKET :
// SO_REUSEADDR : Réutiliser l'adresse locale
// SO_REUSEPORT : Réutiliser le port
// SO_KEEPALIVE : Maintenir la connexion vivante
// SO_RCVBUF : Taille du buffer de réception
// SO_SNDBUF : Taille du buffer d'envoi

// 4. optval :
// Pointeur vers la valeur de l'option
// Type dépend de l'option (int*, struct*, etc.)

// 5. optlen :
// Taille de la valeur pointée par optval

// Valeur de retour :
// Succès : 0
// Échec : -1 (et errno est défini)

// Appel socket() : _server_fd = socket(AF_INET, SOCK_STREAM, 0);
// Signification :
// AF_INET : "Je veux communiquer en IPv4"
// SOCK_STREAM : "Je veux une connexion TCP fiable"
// 0 : "Utilise le protocole par défaut (TCP)"

// Appel setsockopt() : int opt = 1; setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))
// Signification :
// _server_fd : "Configure ce socket"
// SOL_SOCKET : "Option générique de socket"
// SO_REUSEADDR : "Je veux réutiliser l'adresse"
// &opt : "La valeur est 1 (activé)"
// sizeof(opt) : "La valeur fait 4 octets"
