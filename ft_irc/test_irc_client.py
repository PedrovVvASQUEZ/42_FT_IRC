#!/usr/bin/env python3

import socket
import time
import sys

def test_irc_auth(host='localhost', port=6669, password='password'):
    """Test de l'authentification IRC"""
    try:
        # Connexion au serveur
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(5)
        
        print(f"🔌 Connexion à {host}:{port}")
        sock.connect((host, port))
        print("✅ Connecté!")
        
        # Séquence d'authentification IRC
        commands = [
            f"PASS {password}",
            "NICK TestUser", 
            "USER testuser hostname servername :Test Real Name"
        ]
        
        for cmd in commands:
            print(f"📤 Envoi: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.2)
            
            # Tentative de réception de réponse
            try:
                sock.settimeout(1)
                response = sock.recv(1024).decode()
                if response:
                    print(f"📥 Réponse: {response.strip()}")
            except socket.timeout:
                print("⏱️  Pas de réponse (timeout)")
            except:
                pass
        
        # Attendre les messages de bienvenue
        print("\n🎉 Attente des messages de bienvenue...")
        try:
            sock.settimeout(3)
            response = sock.recv(2048).decode()
            if response:
                print(f"📥 Messages reçus:\n{response}")
        except socket.timeout:
            print("⏱️  Pas de message de bienvenue reçu")
        
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
    test_irc_auth(port=port, password=password)