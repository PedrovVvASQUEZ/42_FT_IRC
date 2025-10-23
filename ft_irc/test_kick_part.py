#!/usr/bin/env python3
import socket
import time
import threading

def irc_client_simple(name, commands, delay=0):
    """Client IRC simple pour tests"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(("localhost", 6670))
        
        if delay > 0:
            time.sleep(delay)
        
        # Authentification
        sock.send(f"PASS motdepasse\r\n".encode())
        sock.send(f"NICK {name}\r\n".encode())
        sock.send(f"USER {name.lower()} 0 * :{name} User\r\n".encode())
        time.sleep(1)
        
        # Commandes
        for cmd in commands:
            print(f"📤 {name}: {cmd}")
            sock.send((cmd + "\r\n").encode())
            time.sleep(0.5)
            
            # Recevoir réponse
            try:
                sock.settimeout(1)
                response = sock.recv(2048)
                if response:
                    for line in response.decode().strip().split('\n'):
                        if line.strip():
                            print(f"📥 {name}: {line.strip()}")
            except socket.timeout:
                pass
        
        # Écouter broadcasts
        time.sleep(1)
        try:
            sock.settimeout(1)
            response = sock.recv(2048)
            if response:
                for line in response.decode().strip().split('\n'):
                    if line.strip():
                        print(f"🔊 {name}: {line.strip()}")
        except socket.timeout:
            pass
            
        sock.send("QUIT\r\n".encode())
        sock.close()
        
    except Exception as e:
        print(f"❌ {name}: {e}")

def test_kick_part():
    """Test des commandes KICK et PART"""
    print("🧪 Testing KICK and PART commands...")
    
    # Alice - créateur et opérateur
    alice_commands = [
        "JOIN #kicktest",
        "TOPIC #kicktest :Testing kicks and parts"
    ]
    
    # Bob - membre normal
    bob_commands = [
        "JOIN #kicktest",
        "PRIVMSG #kicktest :Bob is here!"
    ]
    
    # Charlie - membre qui sera kické
    charlie_commands = [
        "JOIN #kicktest", 
        "PRIVMSG #kicktest :Charlie says hello!"
    ]
    
    # Alice finale - kick Charlie
    alice_final_commands = [
        "KICK #kicktest Charlie :Testing kick functionality",
        "PART #kicktest :Alice leaves voluntarily"
    ]
    
    threads = []
    
    # Lancer Alice
    t1 = threading.Thread(target=irc_client_simple, args=("Alice", alice_commands, 0))
    threads.append(t1)
    t1.start()
    
    # Lancer Bob
    t2 = threading.Thread(target=irc_client_simple, args=("Bob", bob_commands, 1))
    threads.append(t2) 
    t2.start()
    
    # Lancer Charlie
    t3 = threading.Thread(target=irc_client_simple, args=("Charlie", charlie_commands, 2))
    threads.append(t3)
    t3.start()
    
    # Actions finales d'Alice
    time.sleep(4)
    t4 = threading.Thread(target=irc_client_simple, args=("Alice", alice_final_commands, 0))
    threads.append(t4)
    t4.start()
    
    # Attendre tous les threads
    for t in threads:
        t.join()
    
    print("✅ KICK and PART test completed!")

if __name__ == "__main__":
    test_kick_part()
