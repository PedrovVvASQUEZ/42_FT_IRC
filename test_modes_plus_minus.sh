#!/bin/bash

# Test des modes + et - pour le serveur IRC
# Ce script teste l'activation et la désactivation des modes

echo "🚀 Test des modes + et - du serveur IRC"
echo "========================================"

# Démarrer le serveur en arrière-plan
cd /workspaces/FT_IRC/ft_irc
echo "📡 Démarrage du serveur IRC..."
timeout 60 ./ircserv 6667 password123 &
SERVER_PID=$!
sleep 2

# Fonction pour envoyer des commandes et attendre la réponse
send_command() {
    local cmd="$1"
    local desc="$2"
    echo ""
    echo "🔧 $desc"
    echo "   → Envoi: $cmd"
    echo "$cmd" | nc localhost 6667 -w 2 | head -10
    sleep 1
}

# Fonction pour créer une nouvelle connexion
connect_user() {
    local nick="$1"
    local desc="$2"
    echo ""
    echo "👤 $desc"
    (
        echo "PASS password123"
        echo "NICK $nick"
        echo "USER $nick 0 * :$nick User"
        sleep 1
        echo "JOIN #test"
        sleep 1
    ) | nc localhost 6667 -w 3 | grep -E "(376|001|366)" | tail -3
}

echo ""
echo "📋 PHASE 1: Connexion et création du channel"
echo "============================================="

# Connexion Admin (créateur du channel)
connect_user "Admin" "Connexion de l'administrateur"

echo ""
echo "🎛️ PHASE 2: Test des modes + (activation)"
echo "========================================="

# Test +i (invite only)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +i\r\n')" "Activation du mode +i (invite-only)"

# Test +t (topic restricted)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +t\r\n')" "Activation du mode +t (topic-restricted)"

# Test +k (key/password)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +k secret123\r\n')" "Activation du mode +k (password: secret123)"

# Test +l (user limit)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +l 5\r\n')" "Activation du mode +l (limite: 5 utilisateurs)"

# Test +o (operator)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +o Admin\r\n')" "Activation du mode +o (opérateur: Admin)"

echo ""
echo "🎛️ PHASE 3: Test des modes - (désactivation)"
echo "============================================"

# Test -i (désactiver invite only)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test -i\r\n')" "Désactivation du mode -i (plus d'invite-only)"

# Test -t (désactiver topic restricted)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test -t\r\n')" "Désactivation du mode -t (topic libre)"

# Test -k (désactiver password)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test -k\r\n')" "Désactivation du mode -k (plus de password)"

# Test -l (désactiver user limit)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test -l\r\n')" "Désactivation du mode -l (plus de limite)"

# Test -o (retirer operator)
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test -o Admin\r\n')" "Désactivation du mode -o (retirer opérateur)"

echo ""
echo "🎛️ PHASE 4: Test des combinaisons de modes"
echo "=========================================="

# Activer plusieurs modes en une fois
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +itk secret456\r\n')" "Activation multiple: +itk secret456"

# Désactiver plusieurs modes en une fois
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test -itk\r\n')" "Désactivation multiple: -itk"

# Mélange d'activation et désactivation
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test +tl-i 10\r\n')" "Mélange: +tl-i 10 (activer t et l, désactiver i)"

echo ""
echo "📊 PHASE 5: Vérification des modes actifs"
echo "========================================="

# Afficher les modes actuels du channel
send_command "$(echo -e 'PASS password123\r\nNICK Admin\r\nUSER Admin 0 * :Admin\r\nMODE #test\r\n')" "Affichage des modes actuels du channel #test"

echo ""
echo "🏁 Test terminé!"
echo "==============="

# Nettoyer
echo "🧹 Arrêt du serveur..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "✅ Tous les tests des modes + et - ont été exécutés!"
echo "📝 Vérifiez les logs du serveur pour voir les changements de modes en action."