#!/bin/bash

# Script de test complet pour ft_irc
# Test avec netcat et irssi

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

SERVER_PORT=6667
SERVER_PASSWORD="motdepasse"

echo -e "${BLUE}=== Test complet du serveur IRC ===${NC}"

# Fonction pour attendre que le serveur soit prêt
wait_for_server() {
    echo -e "${YELLOW}Attente que le serveur soit prêt...${NC}"
    for i in {1..10}; do
        if nc -z localhost $SERVER_PORT 2>/dev/null; then
            echo -e "${GREEN}Serveur détecté sur le port $SERVER_PORT${NC}"
            sleep 1
            return 0
        fi
        sleep 1
    done
    echo -e "${RED}Erreur: Le serveur n'est pas accessible sur le port $SERVER_PORT${NC}"
    return 1
}

# Test 1: Connexion simple avec netcat
test_netcat_simple() {
    echo -e "\n${BLUE}=== Test 1: Connexion simple avec netcat ===${NC}"
    
    (
        echo -e "${YELLOW}Envoi des commandes de base...${NC}"
        printf 'PASS %s\r\nNICK TestUser\r\nUSER test 0 * :Test User\r\nQUIT :Test terminé\r\n' "$SERVER_PASSWORD"
        sleep 2
    ) | nc localhost $SERVER_PORT > /tmp/irc_test1.log 2>&1 &
    
    sleep 3
    
    if [ -s /tmp/irc_test1.log ]; then
        echo -e "${GREEN}✓ Test netcat simple: Réponse reçue${NC}"
        echo "Réponse du serveur:"
        cat /tmp/irc_test1.log
    else
        echo -e "${RED}✗ Test netcat simple: Aucune réponse${NC}"
    fi
}

# Test 2: Test des channels avec netcat
test_netcat_channels() {
    echo -e "\n${BLUE}=== Test 2: Test des channels avec netcat ===${NC}"
    
    (
        echo -e "${YELLOW}Test des channels...${NC}"
        printf 'PASS %s\r\nNICK Alice\r\nUSER alice 0 * :Alice Test\r\nJOIN #general\r\nPRIVMSG #general :Hello from Alice\r\nTOPIC #general :Test topic\r\nQUIT\r\n' "$SERVER_PASSWORD"
        sleep 2
    ) | nc localhost $SERVER_PORT > /tmp/irc_test2.log 2>&1 &
    
    sleep 3
    
    if [ -s /tmp/irc_test2.log ]; then
        echo -e "${GREEN}✓ Test channels: Réponse reçue${NC}"
        echo "Réponse du serveur:"
        cat /tmp/irc_test2.log
    else
        echo -e "${RED}✗ Test channels: Aucune réponse${NC}"
    fi
}

# Test 3: Test multi-clients avec netcat
test_netcat_multi() {
    echo -e "\n${BLUE}=== Test 3: Test multi-clients avec netcat ===${NC}"
    
    # Client 1
    (
        printf 'PASS %s\r\nNICK Bob\r\nUSER bob 0 * :Bob User\r\nJOIN #test\r\n' "$SERVER_PASSWORD"
        sleep 2
        printf 'PRIVMSG #test :Message from Bob\r\n'
        sleep 2
        printf 'QUIT\r\n'
    ) | nc localhost $SERVER_PORT > /tmp/irc_bob.log 2>&1 &
    
    sleep 1
    
    # Client 2
    (
        printf 'PASS %s\r\nNICK Charlie\r\nUSER charlie 0 * :Charlie User\r\nJOIN #test\r\n' "$SERVER_PASSWORD"
        sleep 2
        printf 'PRIVMSG #test :Message from Charlie\r\n'
        sleep 2
        printf 'QUIT\r\n'
    ) | nc localhost $SERVER_PORT > /tmp/irc_charlie.log 2>&1 &
    
    sleep 5
    
    echo -e "${YELLOW}Logs de Bob:${NC}"
    cat /tmp/irc_bob.log
    echo -e "\n${YELLOW}Logs de Charlie:${NC}"
    cat /tmp/irc_charlie.log
}

# Test 4: Vérification de la compatibilité irssi
test_irssi_compatibility() {
    echo -e "\n${BLUE}=== Test 4: Compatibilité irssi ===${NC}"
    
    if ! command -v irssi &> /dev/null; then
        echo -e "${YELLOW}irssi n'est pas installé. Installation...${NC}"
        sudo apt-get update && sudo apt-get install -y irssi
    fi
    
    echo -e "${YELLOW}Création du fichier de configuration irssi...${NC}"
    
    cat > /tmp/irssi_test.conf << EOF
servers = (
  {
    address = "localhost";
    port = "$SERVER_PORT";
    password = "$SERVER_PASSWORD";
    nick = "IrssiTest";
    username = "irssiuser";
    realname = "Irssi Test User";
    autoconnect = "yes";
  }
);

settings = {
  core = {
    real_name = "Irssi Test User";
    user_name = "irssiuser";
    nick = "IrssiTest";
  };
  "fe-text" = { autostick_split_windows = "off"; };
};
EOF
    
    echo -e "${YELLOW}Test de connexion irssi (automatique)...${NC}"
    
    # Script irssi automatique
    cat > /tmp/irssi_script.txt << 'EOF'
/connect localhost
/join #irssitest
/msg #irssitest Hello from irssi!
/topic #irssitest Test topic from irssi
/quit
EOF
    
    timeout 10 irssi --config=/tmp/irssi_test.conf --home=/tmp < /tmp/irssi_script.txt > /tmp/irssi_output.log 2>&1
    
    if [ $? -eq 0 ] || [ $? -eq 124 ]; then
        echo -e "${GREEN}✓ Test irssi terminé${NC}"
        echo "Sortie irssi (dernières lignes):"
        tail -n 20 /tmp/irssi_output.log
    else
        echo -e "${RED}✗ Problème avec irssi${NC}"
    fi
}

# Test 5: Test des erreurs et cas limites
test_error_cases() {
    echo -e "\n${BLUE}=== Test 5: Gestion des erreurs ===${NC}"
    
    echo -e "${YELLOW}Test mot de passe incorrect...${NC}"
    (
        printf 'PASS mauvaispass\r\nNICK ErrorTest\r\nUSER error 0 * :Error Test\r\nQUIT\r\n'
    ) | nc localhost $SERVER_PORT > /tmp/irc_error1.log 2>&1 &
    
    sleep 2
    
    echo -e "${YELLOW}Test nickname déjà utilisé...${NC}"
    (
        printf 'PASS %s\r\nNICK TestUser\r\nUSER test1 0 * :Test 1\r\n' "$SERVER_PASSWORD"
        sleep 1
    ) | nc localhost $SERVER_PORT > /tmp/irc_test_dup1.log 2>&1 &
    
    sleep 1
    
    (
        printf 'PASS %s\r\nNICK TestUser\r\nUSER test2 0 * :Test 2\r\nQUIT\r\n' "$SERVER_PASSWORD"
        sleep 1
    ) | nc localhost $SERVER_PORT > /tmp/irc_test_dup2.log 2>&1 &
    
    sleep 3
    
    echo "Erreur mot de passe:"
    cat /tmp/irc_error1.log
    echo -e "\nTest duplicate nickname (client 1):"
    cat /tmp/irc_test_dup1.log
    echo -e "\nTest duplicate nickname (client 2):"
    cat /tmp/irc_test_dup2.log
}

# Fonction principale
main() {
    echo -e "${BLUE}Vérification que le serveur fonctionne...${NC}"
    
    if ! wait_for_server; then
        echo -e "${RED}Démarrez d'abord le serveur avec: ./ircserv 6667 motdepasse${NC}"
        exit 1
    fi
    
    # Nettoyage des anciens logs
    rm -f /tmp/irc_*.log /tmp/irssi_*.log /tmp/irssi_*.conf /tmp/irssi_script.txt
    
    # Exécution des tests
    test_netcat_simple
    test_netcat_channels
    test_netcat_multi
    test_irssi_compatibility
    test_error_cases
    
    echo -e "\n${GREEN}=== Tests terminés ===${NC}"
    echo -e "${YELLOW}Vérifiez les logs du serveur pour plus de détails${NC}"
}

# Exécution
main "$@"
