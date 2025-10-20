#!/bin/bash

echo "=== Test nc interactif simple ==="

# Test avec \n seulement (simulation nc interactif)
printf 'PASS motdepasse\nNICK TestLF\nUSER test 0 * :Test LF\nQUIT\n' | nc localhost 6667

echo "Test terminé."
