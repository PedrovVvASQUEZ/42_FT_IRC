#!/bin/bash

# Test complet du serveur IRC ft_irc
# Tests séquentiels avec netcat

echo "🚀 Tests complets du serveur IRC ft_irc"
echo "========================================"

# Vérifier que le serveur fonctionne
if ! nc -z localhost 6667 2>/dev/null; then
    echo "❌ ERREUR: Serveur non accessible sur localhost:6667"
    echo "   Démarrez le serveur avec: ./ircserv 6667 mypassword"
    exit 1
fi

echo "✅ Serveur détecté sur localhost:6667"
echo

# Test 1: Connexion et authentification basique
echo "🔌 Test 1: Connexion et authentification basique"
result=$(echo -e "PASS mypassword\nNICK testuser\nUSER testuser 0 * :Test\nQUIT" | nc localhost 6667 | grep "001.*Welcome")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Authentification réussie"
else
    echo "❌ FAIL - Authentification échouée"
fi
echo

# Test 2: Mauvais mot de passe
echo "🚫 Test 2: Mauvais mot de passe"
result=$(echo -e "PASS wrongpass\nNICK testuser\nUSER testuser 0 * :Test\nQUIT" | nc localhost 6667 | grep "464")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Erreur 464 (mauvais mot de passe) détectée"
else
    echo "❌ FAIL - Erreur 464 non détectée"
fi
echo

# Test 3: Channel valide (#)
echo "📺 Test 3: Channel valide avec #"
result=$(echo -e "PASS mypassword\nNICK user1\nUSER user1 0 * :User1\nJOIN #test\nQUIT" | nc localhost 6667 | grep "JOIN #test")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Channel avec # accepté"
else
    echo "❌ FAIL - Channel avec # refusé"
fi
echo

# Test 4: Channel invalide (&)
echo "🔒 Test 4: Channel invalide avec &"
result=$(echo -e "PASS mypassword\nNICK user2\nUSER user2 0 * :User2\nJOIN &test\nQUIT" | nc localhost 6667 | grep "403")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Channel avec & correctement refusé (erreur 403)"
else
    echo "❌ FAIL - Channel avec & incorrectement accepté"
fi
echo

# Test 5: Channel sans préfixe
echo "🔒 Test 5: Channel sans préfixe"
result=$(echo -e "PASS mypassword\nNICK user3\nUSER user3 0 * :User3\nJOIN test\nQUIT" | nc localhost 6667 | grep "403")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Channel sans # correctement refusé (erreur 403)"
else
    echo "❌ FAIL - Channel sans # incorrectement accepté"
fi
echo

# Test 6: Mode invite-only (+i)
echo "⚙️ Test 6: Mode invite-only (+i)"
result=$(echo -e "PASS mypassword\nNICK moderator\nUSER moderator 0 * :Moderator\nJOIN #modetest\nMODE #modetest +i\nQUIT" | nc localhost 6667 | grep "MODE #modetest +i")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Mode +i activé"
else
    echo "❌ FAIL - Mode +i échoué"
fi
echo

# Test 7: Mode avec clé (+k)
echo "🗝️ Test 7: Mode avec clé (+k)"
result=$(echo -e "PASS mypassword\nNICK keyuser\nUSER keyuser 0 * :KeyUser\nJOIN #keytest\nMODE #keytest +k secret\nQUIT" | nc localhost 6667 | grep "MODE #keytest +k")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Mode +k activé"
else
    echo "❌ FAIL - Mode +k échoué"
fi
echo

# Test 8: Mode limite utilisateurs (+l)
echo "👥 Test 8: Mode limite utilisateurs (+l)"
result=$(echo -e "PASS mypassword\nNICK limituser\nUSER limituser 0 * :LimitUser\nJOIN #limittest\nMODE #limittest +l 5\nQUIT" | nc localhost 6667 | grep "MODE #limittest +l")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Mode +l activé"
else
    echo "❌ FAIL - Mode +l échoué"
fi
echo

# Test 9: Topic restriction (+t par défaut)
echo "📝 Test 9: Topic et mode +t"
result=$(echo -e "PASS mypassword\nNICK topicuser\nUSER topicuser 0 * :TopicUser\nJOIN #topictest\nTOPIC #topictest :New topic\nQUIT" | nc localhost 6667 | grep "TOPIC #topictest :New topic")
if [[ -n "$result" ]]; then
    echo "✅ PASS - TOPIC modifié par l'opérateur"
else
    echo "❌ FAIL - TOPIC échoué"
fi
echo

# Test 10: Affichage des modes
echo "📋 Test 10: Affichage des modes actifs"
result=$(echo -e "PASS mypassword\nNICK modeviewer\nUSER modeviewer 0 * :ModeViewer\nJOIN #modeview\nMODE #modeview +i\nMODE #modeview\nQUIT" | nc localhost 6667 | grep "324.*modeview.*+.*i")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Modes affichés correctement"
else
    echo "❌ FAIL - Modes non affichés"
fi
echo

# Test 11: PART (quitter un channel)
echo "🚪 Test 11: Quitter un channel (PART)"
result=$(echo -e "PASS mypassword\nNICK partuser\nUSER partuser 0 * :PartUser\nJOIN #parttest\nPART #parttest :Goodbye\nQUIT" | nc localhost 6667 | grep "PART #parttest")
if [[ -n "$result" ]]; then
    echo "✅ PASS - PART réussi"
else
    echo "❌ FAIL - PART échoué"
fi
echo

# Test 12: Pseudonyme trop long
echo "📛 Test 12: Pseudonyme trop long"
result=$(echo -e "PASS mypassword\nNICK verylongnickname\nUSER test 0 * :Test\nQUIT" | nc localhost 6667 | grep "432")
if [[ -n "$result" ]]; then
    echo "✅ PASS - Pseudonyme trop long refusé (erreur 432)"
else
    echo "❌ FAIL - Pseudonyme trop long accepté"
fi
echo

echo "📊 RÉSUMÉ DES TESTS"
echo "==================="
echo "✅ Tests de base du serveur IRC ft_irc terminés"
echo "📋 Pour des tests plus avancés (multi-utilisateurs), utilisez plusieurs terminaux avec netcat"
echo
echo "💡 Commandes de test manuel :"
echo "   Terminal 1: nc localhost 6667"
echo "   Terminal 2: nc localhost 6667"
echo "   Puis testez les interactions entre utilisateurs"
echo
echo "🎯 Fonctionnalités testées :"
echo "   - Authentification (PASS/NICK/USER)"
echo "   - Restriction des noms de channels (#)"
echo "   - Modes de channels (+i, +k, +l, +t)"
echo "   - Opérations de base (JOIN, PART, TOPIC)"
echo "   - Validation des pseudonymes"
echo "   - Gestion des erreurs"