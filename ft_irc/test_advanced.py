#!/usr/bin/env python3

import socket
import time
import sys
import threading

def client_handler(client_name, host, port, password, commands, delay=0):
    """Gestionnaire pour un client IRC"""
    try:
        time.sleep(delay)  # Délai pour séquencer les connexions
        
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
            time.sleep(1)  # Plus de temps pour voir les réponses
            
            # Écouter les réponses
            try:
                sock.settimeout(2)
                response = sock.recv(2048).decode()
                if response:
                    print(f"📥 {client_name}: {response.strip()}")
            except socket.timeout:
                pass
        
        # Écouter un moment pour recevoir les broadcasts
        print(f"👂 {client_name}: Écoute des broadcasts...")
        for _ in range(3):
            try:
                sock.settimeout(2)
                response = sock.recv(2048).decode()
                if response:
                    print(f"📥 {client_name}: {response.strip()}")
            except socket.timeout:
                pass
        
        # Quitter
        sock.send("QUIT :Test terminé\r\n".encode())
        sock.close()
        print(f"👋 {client_name}: Déconnecté")
        
    except Exception as e:
        print(f"❌ {client_name}: Erreur: {e}")

def test_advanced_commands(host='localhost', port=6670, password='password'):
    """Test des commandes avancées : TOPIC, KICK, INVITE, MODE"""
    
    print("🚀 Test des commandes avancées IRC...")
    
    # Commandes pour Alice (administratrice)
    alice_commands = [
        "JOIN #admin",
        "TOPIC #admin",  # Voir le topic actuel
        "TOPIC #admin :Channel administrateur - Bienvenue!",  # Définir topic
        "MODE #admin +o Alice",  # Se donner les droits d'op
        "INVITE Bob #admin",  # Inviter Bob
        "PRIVMSG #admin :Salut Bob, bienvenue!"
    ]
    
    # Commandes pour Bob 
    bob_commands = [
        "JOIN #admin",  # Rejoindre après invitation
        "TOPIC #admin",  # Voir le topic
        "PRIVMSG #admin :Merci Alice!",
        "MODE #admin",  # Voir les modes du channel
    ]
    
    # Commandes pour Charlie (sera kické)
    charlie_commands = [
        "JOIN #admin",  # Rejoindre le channel
        "PRIVMSG #admin :Salut tout le monde!",
        "TOPIC #admin :Charlie change le topic",  # Essayer de changer le topic
    ]
    
    # Commandes finales pour Alice (kick Charlie)
    alice_final_commands = [
        "KICK #admin Charlie :Comportement inapproprié",
        "TOPIC #admin :Ordre restauré",
        "MODE #admin +o Bob"  # Donner op à Bob
    ]
    
    # Lancer les clients en séquence
    alice_thread = threading.Thread(target=client_handler, 
                                   args=("Alice", host, port, password, alice_commands, 0))
    bob_thread = threading.Thread(target=client_handler, 
                                 args=("Bob", host, port, password, bob_commands, 2))
    charlie_thread = threading.Thread(target=client_handler, 
                                     args=("Charlie", host, port, password, charlie_commands, 4))
    alice_final_thread = threading.Thread(target=client_handler,
                                         args=("Alice", host, port, password, alice_final_commands, 8))
    
    alice_thread.start()
    bob_thread.start() 
    charlie_thread.start()
    
    # Attendre que Charlie soit connecté avant qu'Alice fasse le kick
    time.sleep(6)
    alice_final_thread.start()
    
    alice_thread.join()
    bob_thread.join()
    charlie_thread.join()
    alice_final_thread.join()
    
    print("\n✅ Test des commandes avancées terminé!")
    return True

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 6670
    password = sys.argv[2] if len(sys.argv) > 2 else "password"
    test_advanced_commands(port=port, password=password)