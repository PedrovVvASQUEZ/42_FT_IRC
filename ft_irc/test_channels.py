#!/usr/bin/env python3
import socket
import time
import threading
import sys

def irc_client(name, host, port, password, commands, delay=0):
    """Client IRC pour les tests"""
    print(f"🚀 {name} connecting...")
    
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        
        # Attendre un délai si spécifié
        if delay > 0:
            time.sleep(delay)
        
        # Authentification
        sock.send(f"PASS {password}\r\n".encode())
        time.sleep(0.1)
        sock.send(f"NICK {name}\r\n".encode())
        time.sleep(0.1)
        sock.send(f"USER {name.lower()} 0 * :{name} Test User\r\n".encode())
        time.sleep(0.5)
        
        # Recevoir welcome
        try:
            sock.settimeout(2)
            response = sock.recv(2048)
            if response:
                print(f"📥 {name} connected: {len(response.decode().strip().split(':'))} responses")
        except socket.timeout:
            pass
        
        # Exécuter les commandes
        for cmd in commands:
            print(f"📤 {name}: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(1)
            
            # Recevoir réponse
            try:
                sock.settimeout(2)
                response = sock.recv(2048)
                if response:
                    lines = response.decode().strip().split('\n')
                    for line in lines:
                        if line.strip():
                            print(f"📥 {name}: {line.strip()}")
            except socket.timeout:
                print(f"⏰ {name}: No response for {cmd}")
        
        # Écouter pendant un moment pour les broadcasts
        print(f"👂 {name}: Listening for broadcasts...")
        for _ in range(3):
            try:
                sock.settimeout(2)
                response = sock.recv(2048)
                if response:
                    lines = response.decode().strip().split('\n')
                    for line in lines:
                        if line.strip():
                            print(f"🔊 {name}: {line.strip()}")
            except socket.timeout:
                pass
        
        sock.send("QUIT\r\n".encode())
        sock.close()
        print(f"👋 {name} disconnected")
        
    except Exception as e:
        print(f"❌ {name} error: {e}")

def test_channel_operations():
    """Test des opérations sur les channels"""
    print("🧪 Testing channel operations and operators...")
    
    # Alice - fondatrice du channel (opérateur automatique)
    alice_commands = [
        "JOIN #operators",
        "TOPIC #operators :Test channel for operators",
        "MODE #operators +o Bob",  # Donner op à Bob quand il arrivera
    ]
    
    # Bob - utilisateur normal puis opérateur
    bob_commands = [
        "JOIN #operators",
        "PRIVMSG #operators :Hello from Bob!",
        "TOPIC #operators :Bob tries to change topic",  # Devrait échouer initialement
    ]
    
    # Charlie - utilisateur normal
    charlie_commands = [
        "JOIN #operators",  
        "PRIVMSG #operators :Hello from Charlie!",
        "TOPIC #operators :Charlie tries topic",  # Devrait échouer
    ]
    
    # Alice finale - tests d'opérateur
    alice_final_commands = [
        "MODE #operators +o Bob",  # Donner op à Bob
        "KICK #operators Charlie :Testing kick",  # Kick Charlie
        "TOPIC #operators :Alice final topic",
    ]
    
    # Lancer les clients
    alice_thread = threading.Thread(target=irc_client, 
                                   args=("Alice", "localhost", 6670, "motdepasse", alice_commands, 0))
    bob_thread = threading.Thread(target=irc_client, 
                                 args=("Bob", "localhost", 6670, "motdepasse", bob_commands, 1))
    charlie_thread = threading.Thread(target=irc_client, 
                                     args=("Charlie", "localhost", 6670, "motdepasse", charlie_commands, 2))
    alice_final_thread = threading.Thread(target=irc_client,
                                         args=("Alice", "localhost", 6670, "motdepasse", alice_final_commands, 5))
    
    print("🚀 Starting parallel IRC tests...")
    alice_thread.start()
    bob_thread.start()
    charlie_thread.start()
    
    # Attendre un peu puis lancer les commandes finales d'Alice
    time.sleep(3)
    alice_final_thread.start()
    
    # Attendre la fin
    alice_thread.join()
    bob_thread.join() 
    charlie_thread.join()
    alice_final_thread.join()
    
    print("✅ Channel operations test completed!")

def test_permissions():
    """Test des permissions des opérateurs"""
    print("\n🔐 Testing operator permissions...")
    
    # Test simple : non-opérateur essaie des commandes d'opérateur
    commands = [
        "JOIN #perms",  # Créer channel (devient op)
        "MODE #perms +o NonExistent",  # Essayer de donner op à quelqu'un qui n'existe pas
        "KICK #perms NonExistent :Test",  # Essayer de kick quelqu'un qui n'existe pas
        "TOPIC #perms :Operator set topic",  # Ceci devrait marcher (founder = operator)
    ]
    
    client_thread = threading.Thread(target=irc_client, 
                                    args=("OpTest", "localhost", 6670, "motdepasse", commands, 0))
    client_thread.start()
    client_thread.join()
    
    print("✅ Permissions test completed!")

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 6670
    password = sys.argv[2] if len(sys.argv) > 2 else "motdepasse"
    
    print(f"🔗 Testing IRC server on localhost:{port}")
    
    test_channel_operations()
    test_permissions()
    
    print("\n🎉 All channel tests completed!")
