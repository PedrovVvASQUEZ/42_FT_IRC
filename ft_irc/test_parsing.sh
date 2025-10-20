#!/bin/bash

# Test spécifique du parsing des messages IRC
echo "=== Test du parsing des messages IRC ==="

# Test 1: Messages simples
echo "Test 1: Messages simples"
printf 'PASS motdepasse\r\n' | nc localhost 6667 &
sleep 1

# Test 2: Messages avec paramètres
echo "Test 2: Messages avec paramètres"  
printf 'USER testuser 0 * :Real Name\r\n' | nc localhost 6667 &
sleep 1

# Test 3: Messages avec trailing parameter
echo "Test 3: Messages avec trailing parameter"
printf 'PRIVMSG #channel :This is a long message with spaces\r\n' | nc localhost 6667 &
sleep 1

# Test 4: Séquence complète
echo "Test 4: Séquence complète"
(
  printf 'PASS motdepasse\r\n'
  sleep 0.1
  printf 'NICK TestParser\r\n'  
  sleep 0.1
  printf 'USER parser 0 * :Parser Test\r\n'
  sleep 0.1
  printf 'JOIN #test\r\n'
  sleep 0.1  
  printf 'PRIVMSG #test :Hello world!\r\n'
  sleep 0.1
  printf 'QUIT :Goodbye\r\n'
) | nc localhost 6667

sleep 2
echo "Tests terminés"
