#!/usr/bin/env python3

import socket
import time
import sys
import threading

def client_handler(client_name, host, port, password, commands, responses):
    """Gestionnaire pour un client IRC"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        
        print(f"🔌 {client_name}: Connexion à {host}:{port}")
        sock.connect((host, port))
        
        # Authentification
        auth_commands = [
            f"PASS {password}",
            f"NICK {client_name}", 
            f"USER {client_name.lower()} hostname servername :{client_name} Real Name"
        ]
        
        for cmd in auth_commands:
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.1)
        
        # Attendre l'authentification
        time.sleep(1)
        try:
            response = sock.recv(2048).decode()
            if "Welcome" in response:
                print(f"✅ {client_name}: Authentifié!")
        except socket.timeout:
            pass
        
        # Exécuter les commandes spécifiques
        for cmd in commands:
            print(f"📤 {client_name}: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.5)
            
            # Écouter les réponses
            try:
                sock.settimeout(2)
                response = sock.recv(2048).decode()
                if response:
                    responses[client_name].append(response)
                    print(f"📥 {client_name}: {response.strip()}")
            except socket.timeout:
                pass
        
        # Écouter pendant un moment pour recevoir les messages des autres
        print(f"👂 {client_name}: Écoute des messages...")
        for _ in range(3):  # Écouter 3 cycles
            try:
                sock.settimeout(2)
                response = sock.recv(2048).decode()
                if response:
                    responses[client_name].append(response)
                    print(f"📥 {client_name}: {response.strip()}")
            except socket.timeout:
                pass
        
        # Quitter
        sock.send("QUIT :Test terminé\r\n".encode())
        sock.close()
        print(f"👋 {client_name}: Déconnecté")
        
    except Exception as e:
        print(f"❌ {client_name}: Erreur: {e}")

def test_privmsg(host='localhost', port=6670, password='password'):
    """Test des messages PRIVMSG (channel et privé)"""
    
    responses = {"Alice": [], "Bob": []}
    
    # Commandes pour Alice
    alice_commands = [
        "JOIN #general",
        "PRIVMSG #general :Salut tout le monde!",
        "PRIVMSG Bob :Salut Bob, ça va?",
        "PRIVMSG #inexistant :Test erreur"
    ]
    
    # Commandes pour Bob  
    bob_commands = [
        "JOIN #general",
        "PRIVMSG #general :Salut Alice!",
        "PRIVMSG Alice :Oui ça va, merci!",
        "PRIVMSG Charlie :Message vers user inexistant"
    ]
    
    # Lancer les deux clients en parallèle
    alice_thread = threading.Thread(target=client_handler, 
                                   args=("Alice", host, port, password, alice_commands, responses))
    bob_thread = threading.Thread(target=client_handler, 
                                 args=("Bob", host, port, password, bob_commands, responses))
    
    print("🚀 Lancement des tests PRIVMSG avec 2 clients...")
    alice_thread.start()
    time.sleep(1)  # Délai pour que Alice se connecte en premier
    bob_thread.start()
    
    alice_thread.join()
    bob_thread.join()
    
    print("\n✅ Test PRIVMSG terminé!")
    return True

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 6670
    password = sys.argv[2] if len(sys.argv) > 2 else "password"
    test_privmsg(port=port, password=password)