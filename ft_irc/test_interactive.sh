#!/bin/bash

echo "=== Test nc interactif avec simulation ==="
echo "Simulation d'une session nc interactive:"

# Simuler une session interactive nc
{
    echo "PASS motdepasse"
    sleep 0.5
    echo "NICK InteractiveUser" 
    sleep 0.5
    echo "USER interactive 0 * :Interactive User"
    sleep 0.5
    echo "JOIN #test"
    sleep 0.5
    echo "PRIVMSG #test :Hello from interactive nc!"
    sleep 0.5
    echo "QUIT :Goodbye"
    sleep 1
} | nc localhost 6667

echo "Test terminé. Vérifiez les logs du serveur."
