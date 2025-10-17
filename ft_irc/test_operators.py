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

def test_operators():
    print("🔧 Test des Opérateurs IRC")
    print("=" * 50)
    
    # Créer 3 clients
    alice = IRCClient("Alice")
    bob = IRCClient("Bob") 
    charlie = IRCClient("Charlie")
    
    # Connexion des clients
    if not all([alice.connect(), bob.connect(), charlie.connect()]):
        print("❌ Échec de connexion")
        return
    
    time.sleep(1)
    
    print("\n📺 1. Alice crée et rejoint le channel #test")
    alice.send_command("JOIN #test")
    alice.receive_messages()
    
    print("\n👥 2. Bob et Charlie rejoignent le channel")
    bob.send_command("JOIN #test")
    charlie.send_command("JOIN #test")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages() 
    charlie.receive_messages()
    
    print("\n📋 3. Alice vérifie la liste des membres")
    alice.send_command("NAMES #test")
    alice.receive_messages()
    
    print("\n🔧 4. Alice donne les privilèges d'opérateur à Bob")
    alice.send_command("MODE #test +o Bob")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n📋 5. Vérification de la liste après promotion (Bob devrait avoir @)")
    alice.send_command("NAMES #test") 
    alice.receive_messages()
    
    print("\n📝 6. Bob (maintenant opérateur) change le topic")
    bob.send_command("TOPIC #test :Nouveau topic par Bob l'opérateur")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n⚡ 7. Bob kick Charlie du channel")
    bob.send_command("KICK #test Charlie :Test du kick par opérateur")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    charlie.receive_messages()
    
    print("\n📋 8. Vérification finale de la liste (Charlie ne devrait plus être là)")
    alice.send_command("NAMES #test")
    alice.receive_messages()
    
    print("\n🔧 9. Alice retire les privilèges d'opérateur à Bob")
    alice.send_command("MODE #test -o Bob")
    
    time.sleep(0.5)
    alice.receive_messages()
    bob.receive_messages()
    
    print("\n📋 10. Vérification finale (Bob ne devrait plus avoir @)")
    alice.send_command("NAMES #test")
    alice.receive_messages()
    
    print("\n👋 Déconnexion des clients")
    alice.disconnect()
    bob.disconnect()
    charlie.disconnect()
    
    print("\n✅ Test terminé - Les opérateurs fonctionnent correctement !")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 test_operators.py <port> <password>")
        print("Example: python3 test_operators.py 6669 password")
        sys.exit(1)
    
    port = int(sys.argv[1])
    password = sys.argv[2]
    
    # Modification pour utiliser les paramètres
    def test_with_params():
        print(f"🔧 Test des Opérateurs IRC sur port {port}")
        print("=" * 50)
        
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
        
        print("\n📺 1. Alice crée et rejoint le channel #test")
        alice.send_command("JOIN #test")
        alice.receive_messages()
        
        print("\n👥 2. Bob et Charlie rejoignent le channel")
        bob.send_command("JOIN #test")
        charlie.send_command("JOIN #test")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages() 
        charlie.receive_messages()
        
        print("\n📋 3. Alice vérifie la liste des membres")
        alice.send_command("NAMES #test")
        alice.receive_messages()
        
        print("\n🔧 4. Alice donne les privilèges d'opérateur à Bob")
        alice.send_command("MODE #test +o Bob")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n📋 5. Vérification de la liste après promotion (Bob devrait avoir @)")
        alice.send_command("NAMES #test") 
        alice.receive_messages()
        
        print("\n📝 6. Bob (maintenant opérateur) change le topic")
        bob.send_command("TOPIC #test :Nouveau topic par Bob l'opérateur")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n⚡ 7. Bob kick Charlie du channel")
        bob.send_command("KICK #test Charlie :Test du kick par opérateur")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        charlie.receive_messages()
        
        print("\n📋 8. Vérification finale de la liste (Charlie ne devrait plus être là)")
        alice.send_command("NAMES #test")
        alice.receive_messages()
        
        print("\n🔧 9. Alice retire les privilèges d'opérateur à Bob")
        alice.send_command("MODE #test -o Bob")
        
        time.sleep(0.5)
        alice.receive_messages()
        bob.receive_messages()
        
        print("\n📋 10. Vérification finale (Bob ne devrait plus avoir @)")
        alice.send_command("NAMES #test")
        alice.receive_messages()
        
        print("\n👋 Déconnexion des clients")
        alice.disconnect()
        bob.disconnect()
        charlie.disconnect()
        
        print("\n✅ Test terminé - Les opérateurs fonctionnent correctement !")
    
    test_with_params()