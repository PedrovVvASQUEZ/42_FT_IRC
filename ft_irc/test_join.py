#!/usr/bin/env python3

import socket
import time
import sys

def test_irc_join(host='localhost', port=6669, password='password'):
    """Test de la commande JOIN IRC"""
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
        
        # Attendre les messages de bienvenue
        print("\n🎉 Attente de l'authentification...")
        time.sleep(1)
        try:
            response = sock.recv(2048).decode()
            if "Welcome" in response:
                print("✅ Authentification réussie!")
            else:
                print(f"📥 Réponse: {response}")
        except socket.timeout:
            print("⏱️  Timeout sur l'authentification")
        
        # Test de la commande JOIN
        print("\n📺 Test de la commande JOIN...")
        join_commands = [
            "JOIN #test",
            "JOIN #general", 
            "JOIN #random"
        ]
        
        for cmd in join_commands:
            print(f"📤 Envoi: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.5)
            
            # Écouter les réponses
            try:
                sock.settimeout(2)
                response = sock.recv(2048).decode()
                if response:
                    print(f"📥 Réponses:\n{response}")
            except socket.timeout:
                print("⏱️  Pas de réponse au JOIN")
        
        # Quitter proprement
        sock.send("QUIT :Test terminé\r\n".encode())
        sock.close()
        print("👋 Déconnexion propre")
        
    except Exception as e:
        print(f"❌ Erreur: {e}")
        return False
    
    return True

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 6669
    password = sys.argv[2] if len(sys.argv) > 2 else "password"
    test_irc_join(port=port, password=password)