#!/usr/bin/env python3
"""
Script de test pour vérifier les modes de channels implémentés
Modes testés: +i, +t, +k, +o, +l
"""

import socket
import time
import sys

def send_and_receive(sock, command, delay=0.2):
    """Envoie une commande et récupère la réponse"""
    sock.send((command + "\r\n").encode())
    time.sleep(delay)
    try:
        data = sock.recv(4096).decode()
        return data.strip()
    except:
        return ""

def test_channel_modes():
    """Test complet des modes de channels"""
    
    # Connexion du premier utilisateur (admin)
    admin_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        admin_sock.connect(('localhost', 6667))
        
        # Authentification admin
        send_and_receive(admin_sock, "PASS mypassword")
        send_and_receive(admin_sock, "NICK admin")
        send_and_receive(admin_sock, "USER admin 0 * :Administrator")
        time.sleep(0.5)
        
        print("=== Test 1: Création du channel et mode +t par défaut ===")
        response = send_and_receive(admin_sock, "JOIN #test")
        print("JOIN #test:", response.split('\n')[0])
        
        # Test du topic avec mode +t (admin est opérateur)
        response = send_and_receive(admin_sock, "TOPIC #test :Topic administrateur")
        print("TOPIC (admin):", response.split('\n')[0])
        
        print("\n=== Test 2: Mode +i (invite only) ===")
        response = send_and_receive(admin_sock, "MODE #test +i")
        print("MODE +i:", response.split('\n')[0])
        
        print("\n=== Test 3: Mode +k (password) ===")
        response = send_and_receive(admin_sock, "MODE #test +k secret123")
        print("MODE +k:", response.split('\n')[0])
        
        print("\n=== Test 4: Mode +l (user limit) ===")
        response = send_and_receive(admin_sock, "MODE #test +l 5")
        print("MODE +l:", response.split('\n')[0])
        
        print("\n=== Test 5: Vérification des modes actifs ===")
        response = send_and_receive(admin_sock, "MODE #test")
        print("MODE #test:", response.split('\n')[0])
        
    except ConnectionRefusedError:
        print("❌ Impossible de se connecter au serveur sur le port 6667")
        return False
    
    # Test avec un deuxième utilisateur
    user_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        user_sock.connect(('localhost', 6667))
        
        # Authentification utilisateur
        send_and_receive(user_sock, "PASS mypassword")
        send_and_receive(user_sock, "NICK user")
        send_and_receive(user_sock, "USER user 0 * :Regular User")
        time.sleep(0.5)
        
        print("\n=== Test 6: JOIN sans mot de passe (doit échouer) ===")
        response = send_and_receive(user_sock, "JOIN #test")
        print("JOIN sans key:", response.split('\n')[0])
        
        print("\n=== Test 7: JOIN avec mauvais mot de passe ===")
        response = send_and_receive(user_sock, "JOIN #test wrongpass")
        print("JOIN mauvaise key:", response.split('\n')[0])
        
        print("\n=== Test 8: Invitation par admin ===")
        response = send_and_receive(admin_sock, "INVITE user #test")
        print("INVITE:", response.split('\n')[0])
        
        print("\n=== Test 9: JOIN avec invitation et bon mot de passe ===")
        response = send_and_receive(user_sock, "JOIN #test secret123")
        print("JOIN avec key:", response.split('\n')[0])
        
        print("\n=== Test 10: Tentative de changer le topic (non-op) ===")
        response = send_and_receive(user_sock, "TOPIC #test :Topic utilisateur")
        print("TOPIC (user):", response.split('\n')[0])
        
        print("\n=== Test 11: Donner des droits d'opérateur ===")
        response = send_and_receive(admin_sock, "MODE #test +o user")
        print("MODE +o:", response.split('\n')[0])
        
        print("\n=== Test 12: Changer le topic (maintenant op) ===")
        response = send_and_receive(user_sock, "TOPIC #test :Topic utilisateur maintenant op")
        print("TOPIC (user op):", response.split('\n')[0])
        
        print("\n=== Test 13: Retirer le mode +t ===")
        response = send_and_receive(admin_sock, "MODE #test -t")
        print("MODE -t:", response.split('\n')[0])
        
        print("\n=== Test 14: Retirer les modes +i, +k, +l ===")
        response = send_and_receive(admin_sock, "MODE #test -i")
        print("MODE -i:", response.split('\n')[0])
        response = send_and_receive(admin_sock, "MODE #test -k")
        print("MODE -k:", response.split('\n')[0])
        response = send_and_receive(admin_sock, "MODE #test -l")
        print("MODE -l:", response.split('\n')[0])
        
        print("\n=== Test 15: Modes finaux ===")
        response = send_and_receive(admin_sock, "MODE #test")
        print("MODE final:", response.split('\n')[0])
        
        # Nettoyage
        send_and_receive(admin_sock, "QUIT :Test terminé")
        send_and_receive(user_sock, "QUIT :Test terminé")
        
    except Exception as e:
        print(f"❌ Erreur avec le deuxième utilisateur: {e}")
        return False
    finally:
        admin_sock.close()
        user_sock.close()
    
    return True

if __name__ == "__main__":
    print("🔧 Test des modes de channels IRC")
    print("📍 Assurez-vous que le serveur fonctionne sur localhost:6667")
    print()
    
    success = test_channel_modes()
    print("\n🎯 Test " + ("réussi !" if success else "échoué !"))
    sys.exit(0 if success else 1)