#!/bin/bash

# Script de test pour l'authentification IRC
# Usage: ./test_irc_auth.sh [port] [password]

PORT=${1:-6669}
PASSWORD=${2:-password}
TIMEOUT=3

echo "🔧 Test de l'authentification IRC sur le port $PORT"
echo "📝 Séquence: PASS → NICK → USER → RPL_WELCOME attendu"
echo ""

# Test de la séquence d'authentification IRC
{
    sleep 0.1
    echo "PASS $PASSWORD"
    sleep 0.1
    echo "NICK TestUser"
    sleep 0.1
    echo "USER testuser hostname servername :Test Real Name"
    sleep 1
    echo "QUIT :Fin du test"
} | timeout $TIMEOUT telnet localhost $PORT

echo ""
echo "✅ Test d'authentification terminé"