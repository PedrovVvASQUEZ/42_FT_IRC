#!/usr/bin/env python3

import socket
import time
import sys

def test_join_part(host='localhost', port=6670, password='password'):
    """Test des commandes JOIN et PART IRC"""
    try:
        # Connexion au serveur
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        
        print(f"🔌 Connexion à {host}:{port}")
        sock.connect((host, port))
        print("✅ Connecté!")
        
        # Authentification complète
        commands = [
            f"PASS {password}",
            "NICK TestUser", 
            "USER testuser hostname servername :Test Real Name"
        ]
        
        for cmd in commands:
            print(f"📤 Envoi: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.1)
        
        # Attendre l'authentification
        time.sleep(1)
        try:
            response = sock.recv(2048).decode()
            if "Welcome" in response:
                print("✅ Authentification réussie!")
        except socket.timeout:
            pass
        
        # Test des commandes JOIN/PART
        print("\n📺 Test séquence JOIN/PART...")
        test_commands = [
            "JOIN #test",
            "JOIN #general", 
            "PART #test",
            "PART #general :Au revoir!",
            "PART #inexistant"  # Test d'erreur
        ]
        
        for cmd in test_commands:
            print(f"\n📤 Envoi: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.5)
            
            # Écouter les réponses
            try:
                sock.settimeout(2)
                response = sock.recv(2048).decode()
                if response:
                    print(f"📥 Réponses:\n{response}")
            except socket.timeout:
                print("⏱️  Pas de réponse")
        
        # Quitter proprement
        sock.send("QUIT :Test terminé\r\n".encode())
        sock.close()
        print("👋 Déconnexion propre")
        
    except Exception as e:
        print(f"❌ Erreur: {e}")
        return False
    
    return True

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 6670
    password = sys.argv[2] if len(sys.argv) > 2 else "password"
    test_join_part(port=port, password=password)