#!/bin/bash
# Script simple pour lancer et tester le serveur

echo "🚀 Starting IRC server..."
./ircserv 6667 password &
SERVER_PID=$!

echo "⏳ Waiting for server to be ready..."
sleep 2

echo "🧪 Running tests..."
python3 simple_test.py

echo "🛑 Stopping server..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "✅ Done!"