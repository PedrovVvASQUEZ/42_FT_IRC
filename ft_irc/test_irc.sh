#!/bin/bash

echo "=== Test des fonctionnalités IRC ==="
echo

echo "1. Test de JOIN - Bob rejoint un channel"
{
    echo "PASS motdepasse"
    echo "NICK Bob"  
    echo "USER bob 0 * :Bob User"
    sleep 1
    echo "JOIN #test"
    sleep 1
    echo "PRIVMSG #test :Hello from Bob!"
    sleep 2
    echo "QUIT :Bye"
} | nc localhost 6667 &

sleep 3

echo "2. Test de JOIN - Charlie rejoint le même channel"
{
    echo "PASS motdepasse" 
    echo "NICK Charlie"
    echo "USER charlie 0 * :Charlie User"
    sleep 1
    echo "JOIN #test"
    sleep 1
    echo "PRIVMSG #test :Hello Bob, I'm Charlie!"
    sleep 1
    echo "PRIVMSG Bob :Private message to Bob"
    sleep 2
    echo "PART #test :Leaving now"
    sleep 1
    echo "QUIT :Goodbye"
} | nc localhost 6667 &

wait
echo "=== Tests terminés ==="
