#!/usr/bin/env python3
import socket
import threading
import time
import sys

class IRCClient:
    def __init__(self, name):
        self.name = name
        self.sock = None
        self.messages = []
        
    def connect(self, host='localhost', port=6669, password='password'):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.sock.connect((host, port))
            
            # Authentification
            self.sock.send(f"PASS {password}\r\n".encode())
            self.sock.send(f"NICK {self.name}\r\n".encode())
            self.sock.send(f"USER {self.name} 0 * :Test User\r\n".encode())
            
            # Recevoir les messages de bienvenue
            time.sleep(0.5)
            data = self.sock.recv(4096).decode('utf-8', errors='ignore')
            print(f"[{self.name}] Connected: {data.strip()}")
            return True
            
        except Exception as e:
            print(f"[{self.name}] Connection failed: {e}")
            return False
    
    def send_command(self, command):
        if self.sock:
            print(f"[{self.name}] Sending: {command}")
            self.sock.send(f"{command}\r\n".encode())
            time.sleep(0.2)
            
    def receive_messages(self):
        if self.sock:
            try:
                data = self.sock.recv(4096).decode('utf-8', errors='ignore')
                if data:
                    messages = data.strip().split('\r\n')
                    for msg in messages:
                        if msg:
                            print(f"[{self.name}] Received: {msg}")
                            self.messages.append(msg)
                return data
            except:
                return None
    
    def disconnect(self):
        if self.sock:
            self.sock.send(b"QUIT :Test finished\r\n")
            self.sock.close()

def test_advanced_permissions():
    print("🔐 Test des Permissions Avancées IRC")
    print("=" * 60)
    
    # Créer 3 clients
    alice = IRCClient("Alice")
    bob = IRCClient("Bob") 
    charlie = IRCClient("Charlie")
    
    # Connexion des clients
    if not all([alice.connect(), bob.connect(), charlie.connect()]):
        print("❌ Échec de connexion")
        return
    
    time.sleep(1)
    
    print("\n👑 1. Alice crée le channel #advanced (devient auto-opérateur)")
    alice.send_command("JOIN #advanced")
    alice.receive_messages()
    
    print("\n👥 2. Bob et Charlie rejoignent le channel")
    bob.send_command("JOIN #advanced")
    charlie.send_command("JOIN #advanced")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages() 
    charlie.receive_messages()
    
    print("\n📋 3. Vérification de la liste (Alice devrait avoir @)")
    alice.send_command("NAMES #advanced")
    alice.receive_messages()
    
    print("\n🚫 4. Bob (non-opérateur) essaie de kick Charlie - DEVRAIT ÉCHOUER")
    bob.send_command("KICK #advanced Charlie :Test unauthorized kick")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n📝 5. Charlie essaie de changer le topic - DEVRAIT ÉCHOUER")
    charlie.send_command("TOPIC #advanced :Topic non autorisé")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n🔧 6. Bob essaie de donner +o à Charlie - DEVRAIT ÉCHOUER")
    bob.send_command("MODE #advanced +o Charlie")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n✅ 7. Alice (opérateur) donne +o à Bob - DEVRAIT RÉUSSIR")
    alice.send_command("MODE #advanced +o Bob")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n📋 8. Vérification (Alice et Bob devraient avoir @)")
    alice.send_command("NAMES #advanced")
    alice.receive_messages()
    
    print("\n📝 9. Bob (maintenant opérateur) change le topic - DEVRAIT RÉUSSIR")
    bob.send_command("TOPIC #advanced :Topic par Bob opérateur")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n⚡ 10. Bob kick Charlie - DEVRAIT RÉUSSIR")
    bob.send_command("KICK #advanced Charlie :Kick autorisé par opérateur")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n🛡️  11. Alice essaie de retirer +o de Bob (dernier autre opérateur)")
    alice.send_command("MODE #advanced -o Bob")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    
    print("\n📋 12. Vérification finale de la liste")
    alice.send_command("NAMES #advanced")
    alice.receive_messages()
    
    print("\n👋 Déconnexion des clients")
    alice.disconnect()
    bob.disconnect()
    charlie.disconnect()
    
    print("\n✅ Test des permissions terminé !")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 test_permissions.py <port> <password>")
        print("Example: python3 test_permissions.py 6669 password")
        sys.exit(1)
    
    port = int(sys.argv[1])
    password = sys.argv[2]
    
    # Modification pour utiliser les paramètres
    def test_with_params():
        print(f"🔐 Test des Permissions sur port {port}")
        print("=" * 60)
        
        # Créer 3 clients
        alice = IRCClient("Alice")
        bob = IRCClient("Bob") 
        charlie = IRCClient("Charlie")
        
        # Connexion des clients avec les paramètres fournis
        if not all([alice.connect(port=port, password=password), 
                   bob.connect(port=port, password=password), 
                   charlie.connect(port=port, password=password)]):
            print("❌ Échec de connexion")
            return
        
        time.sleep(1)
        
        print("\n👑 1. Alice crée le channel #advanced (devient auto-opérateur)")
        alice.send_command("JOIN #advanced")
        alice.receive_messages()
        
        print("\n👥 2. Bob et Charlie rejoignent le channel")
        bob.send_command("JOIN #advanced")
        charlie.send_command("JOIN #advanced")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages() 
        charlie.receive_messages()
        
        print("\n📋 3. Vérification de la liste (Alice devrait avoir @)")
        alice.send_command("NAMES #advanced")
        alice.receive_messages()
        
        print("\n🚫 4. Bob (non-opérateur) essaie de kick Charlie - DEVRAIT ÉCHOUER")
        bob.send_command("KICK #advanced Charlie :Test unauthorized kick")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n📝 5. Charlie essaie de changer le topic - DEVRAIT ÉCHOUER")
        charlie.send_command("TOPIC #advanced :Topic non autorisé")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n🔧 6. Bob essaie de donner +o à Charlie - DEVRAIT ÉCHOUER")
        bob.send_command("MODE #advanced +o Charlie")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n✅ 7. Alice (opérateur) donne +o à Bob - DEVRAIT RÉUSSIR")
        alice.send_command("MODE #advanced +o Bob")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n📋 8. Vérification (Alice et Bob devraient avoir @)")
        alice.send_command("NAMES #advanced")
        alice.receive_messages()
        
        print("\n📝 9. Bob (maintenant opérateur) change le topic - DEVRAIT RÉUSSIR")
        bob.send_command("TOPIC #advanced :Topic par Bob opérateur")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n⚡ 10. Bob kick Charlie - DEVRAIT RÉUSSIR")
        bob.send_command("KICK #advanced Charlie :Kick autorisé par opérateur")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n🛡️  11. Alice essaie de retirer +o de Bob (test protection dernier opérateur)")
        alice.send_command("MODE #advanced -o Alice")  # Alice retire ses propres privilèges
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        
        print("\n📋 12. Vérification finale de la liste")
        alice.send_command("NAMES #advanced")
        alice.receive_messages()
        
        print("\n👋 Déconnexion des clients")
        alice.disconnect()
        bob.disconnect()
        charlie.disconnect()
        
        print("\n✅ Test des permissions terminé !")
    
    test_with_params()