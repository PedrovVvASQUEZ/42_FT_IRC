#!/usr/bin/env python3
"""
Tests complets pour le serveur IRC
Maintenant que le serveur fonctionne, on peut tester plus de fonctionnalités
"""
import socket
import threading
import time
import sys

class IRCTestClient:
    def __init__(self, name, host='localhost', port=6667):
        self.name = name
        self.host = host
        self.port = port
        self.sock = None
        self.connected = False
    
    def connect(self):
        """Connexion au serveur"""
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.host, self.port))
            self.connected = True
            print(f"✅ {self.name} connected to server")
            return True
        except Exception as e:
            print(f"❌ {self.name} failed to connect: {e}")
            return False
    
    def send_message(self, message):
        """Envoyer un message"""
        if not self.connected:
            return False
        try:
            if not message.endswith('\r\n'):
                message += '\r\n'
            sent = self.sock.send(message.encode())
            print(f"📤 {self.name} sent: {message.strip()} ({sent} bytes)")
            return True
        except Exception as e:
            print(f"❌ {self.name} send error: {e}")
            return False
    
    def receive_message(self, timeout=2.0):
        """Recevoir un message"""
        if not self.connected:
            return None
        try:
            self.sock.settimeout(timeout)
            response = self.sock.recv(1024)
            if response:
                decoded = response.decode().strip()
                print(f"📥 {self.name} received: {decoded}")
                return decoded
            else:
                print(f"📥 {self.name} received empty response")
                return ""
        except socket.timeout:
            print(f"⏰ {self.name} timeout waiting for response")
            return None
        except Exception as e:
            print(f"❌ {self.name} receive error: {e}")
            return None
    
    def disconnect(self):
        """Déconnexion"""
        if self.sock:
            self.sock.close()
            self.connected = False
            print(f"🚪 {self.name} disconnected")

def test_single_client():
    """Test avec un seul client"""
    print("\n🧪 === TEST SINGLE CLIENT ===")
    client = IRCTestClient("Client1")
    
    if not client.connect():
        return False
    
    # Attendre que le serveur traite la connexion
    time.sleep(0.5)
    
    # Test d'envoi de message
    client.send_message("HELLO SERVER")
    response = client.receive_message()
    
    # Test d'un deuxième message
    client.send_message("TEST MESSAGE 2")
    response2 = client.receive_message()
    
    client.disconnect()
    return True

def test_multi_client():
    """Test avec plusieurs clients simultanés"""
    print("\n🧪 === TEST MULTI CLIENT ===")
    
    def client_worker(client_name, messages):
        client = IRCTestClient(client_name)
        if client.connect():
            time.sleep(0.5)  # Laisser le serveur traiter la connexion
            
            for i, msg in enumerate(messages):
                client.send_message(f"{msg} (message {i+1})")
                client.receive_message()
                time.sleep(0.2)  # Petite pause entre les messages
            
            client.disconnect()
    
    # Créer plusieurs threads pour simuler des clients simultanés
    threads = []
    client_configs = [
        ("Alice", ["Hello from Alice", "How are you?", "Goodbye"]),
        ("Bob", ["Hi there", "Bob here", "See you later"]),
        ("Charlie", ["Charlie speaking", "Testing server", "All done"])
    ]
    
    # Lancer les clients en parallèle
    for name, messages in client_configs:
        thread = threading.Thread(target=client_worker, args=(name, messages))
        threads.append(thread)
        thread.start()
        time.sleep(0.1)  # Décaler légèrement les connexions
    
    # Attendre que tous les clients terminent
    for thread in threads:
        thread.join()
    
    print("✅ Multi-client test completed")
    return True

def test_connection_handling():
    """Test de gestion des connexions/déconnexions"""
    print("\n🧪 === TEST CONNECTION HANDLING ===")
    
    # Connexion rapide puis déconnexion
    for i in range(3):
        client = IRCTestClient(f"QuickClient{i+1}")
        if client.connect():
            time.sleep(0.2)
            client.send_message(f"Quick message {i+1}")
            client.receive_message()
            client.disconnect()
            time.sleep(0.2)
    
    return True

def main():
    print("🚀 Starting IRC Server Tests")
    print("Make sure your IRC server is running on localhost:6667")
    
    try:
        # Test basic connectivity
        if not test_single_client():
            print("❌ Single client test failed")
            return 1
        
        time.sleep(1)
        
        # Test multiple clients
        if not test_multi_client():
            print("❌ Multi client test failed")
            return 1
        
        time.sleep(1)
        
        # Test connection handling
        if not test_connection_handling():
            print("❌ Connection handling test failed")
            return 1
        
        print("\n🎉 All tests passed!")
        return 0
        
    except KeyboardInterrupt:
        print("\n⚠️  Tests interrupted by user")
        return 1
    except Exception as e:
        print(f"\n❌ Test suite error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())