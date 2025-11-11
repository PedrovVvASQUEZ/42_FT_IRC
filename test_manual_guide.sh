#!/bin/bash

# Guide de test manuel multi-utilisateurs pour IRC ft_irc
# Ce script donne les instructions pour tester les fonctionnalités avancées

echo "🎭 Guide de test manuel multi-utilisateurs - IRC ft_irc"
echo "==========================================================="
echo
echo "Pour tester les fonctionnalités avancées nécessitant plusieurs utilisateurs,"
echo "suivez ce guide étape par étape :"
echo

echo "📋 PRÉPARATION :"
echo "1. Démarrez le serveur : ./ircserv 6667 mypassword"
echo "2. Ouvrez 3 terminaux supplémentaires"
echo

echo "🧪 TEST 1: Interaction multi-utilisateurs de base"
echo "=================================================="
echo
echo "Terminal 1 (Alice) :"
echo "  nc localhost 6667"
echo "  PASS mypassword"
echo "  NICK alice"
echo "  USER alice 0 * :Alice"
echo "  JOIN #general"
echo
echo "Terminal 2 (Bob) :"
echo "  nc localhost 6667"
echo "  PASS mypassword"
echo "  NICK bob"
echo "  USER bob 0 * :Bob"
echo "  JOIN #general"
echo
echo "✅ Vérification : Alice devrait voir Bob rejoindre"
echo "✅ Les deux devraient être dans la liste des membres"
echo

echo "🧪 TEST 2: Messages de channel"
echo "==============================="
echo
echo "Alice (Terminal 1) :"
echo "  PRIVMSG #general :Hello everyone!"
echo
echo "Bob (Terminal 2) :"
echo "  PRIVMSG #general :Hi Alice!"
echo
echo "✅ Vérification : Chaque utilisateur voit les messages de l'autre"
echo

echo "🧪 TEST 3: Messages privés"
echo "==========================="
echo
echo "Alice :"
echo "  PRIVMSG bob :This is a private message"
echo
echo "✅ Vérification : Seul Bob reçoit le message privé"
echo

echo "🧪 TEST 4: Gestion des privilèges d'opérateur"
echo "==============================================="
echo
echo "Alice (créatrice du channel, donc opérateur) :"
echo "  MODE #general +o bob"
echo
echo "✅ Vérification : Bob devient opérateur (@bob dans la liste)"
echo
echo "Bob (maintenant opérateur) :"
echo "  TOPIC #general :New topic set by Bob"
echo
echo "✅ Vérification : Le topic change et est diffusé à tous"
echo

echo "🧪 TEST 5: Test du KICK"
echo "========================"
echo
echo "Terminal 3 (Charlie) :"
echo "  nc localhost 6667"
echo "  PASS mypassword"
echo "  NICK charlie"
echo "  USER charlie 0 * :Charlie"
echo "  JOIN #general"
echo
echo "Alice ou Bob (opérateurs) :"
echo "  KICK #general charlie :Please behave"
echo
echo "✅ Vérification : Charlie est expulsé du channel"
echo

echo "🧪 TEST 6: Mode invitation uniquement (+i)"
echo "============================================"
echo
echo "Alice :"
echo "  MODE #general +i"
echo
echo "Charlie (Terminal 3) :"
echo "  JOIN #general"
echo
echo "✅ Vérification : Charlie ne peut pas rejoindre (erreur 473)"
echo
echo "Alice :"
echo "  INVITE charlie #general"
echo
echo "Charlie :"
echo "  JOIN #general"
echo
echo "✅ Vérification : Maintenant Charlie peut rejoindre"
echo

echo "🧪 TEST 7: Mode avec clé (+k)"
echo "=============================="
echo
echo "Alice :"
echo "  MODE #general +k secretpass"
echo
echo "Tous quittent le channel :"
echo "  PART #general"
echo
echo "Tentative de rejoindre sans clé :"
echo "  JOIN #general"
echo
echo "✅ Vérification : Accès refusé (erreur 475)"
echo
echo "Rejoindre avec la bonne clé :"
echo "  JOIN #general secretpass"
echo
echo "✅ Vérification : Accès autorisé"
echo

echo "🧪 TEST 8: Mode limite d'utilisateurs (+l)"
echo "============================================"
echo
echo "Alice :"
echo "  MODE #general +l 2"
echo
echo "✅ Vérification : Seuls 2 utilisateurs maximum dans le channel"
echo
echo "Troisième utilisateur tente de rejoindre :"
echo "✅ Vérification : Accès refusé (erreur 471)"
echo

echo "🧪 TEST 9: Combinaisons de modes"
echo "================================="
echo
echo "Alice :"
echo "  MODE #general +itl 3"
echo "  MODE #general"
echo
echo "✅ Vérification : Affichage des modes actifs (+itl)"
echo
echo "Retrait de modes :"
echo "  MODE #general -i"
echo "  MODE #general"
echo
echo "✅ Vérification : Mode +i retiré (+tl restants)"
echo

echo "🧪 TEST 10: Gestion des déconnexions"
echo "====================================="
echo
echo "Charlie fait Ctrl+C (déconnexion brutale)"
echo
echo "✅ Vérification : Le serveur nettoie automatiquement"
echo "✅ Charlie disparaît de la liste des membres"
echo

echo "🧪 TEST 11: Commande QUIT propre"
echo "================================="
echo
echo "Bob :"
echo "  QUIT :See you later!"
echo
echo "✅ Vérification : Message de départ diffusé à tous les channels"
echo "✅ Bob est retiré de tous les channels"
echo

echo
echo "📈 MÉTRIQUES À VÉRIFIER :"
echo "=========================="
echo "- Pas de fuites mémoire (tous les clients supprimés)"
echo "- Channels vides supprimés automatiquement"
echo "- Messages d'erreur corrects selon RFC IRC"
echo "- Logs serveur clairs et informatifs"
echo "- Arrêt propre du serveur avec Ctrl+C"
echo

echo "🎯 RÉSULTATS ATTENDUS :"
echo "========================"
echo "✅ Toutes les interactions multi-utilisateurs fonctionnent"
echo "✅ Tous les modes de channels sont opérationnels"
echo "✅ Gestion propre des connexions/déconnexions"
echo "✅ Respect des privilèges d'opérateur"
echo "✅ Messages et erreurs conformes au standard IRC"
echo

echo "🚀 Votre serveur IRC ft_irc est prêt pour l'évaluation !"