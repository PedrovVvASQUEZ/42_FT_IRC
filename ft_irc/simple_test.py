#!/usr/bin/env python3
import socket
import sys
import time

def test_simple():
    print("🔌 Connecting to IRC server...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        sock.connect(('localhost', 6667))
        print("✅ Connected!")
        
        # Attendre que le serveur traite la connexion
        print("⏳ Waiting for server to process connection...")
        time.sleep(1.0)
        
        # Envoyer un message simple
        message = "HELLO\r\n"
        print(f"📤 Sending: {message.strip()}")
        sent = sock.send(message.encode())
        print(f"📤 Sent {sent} bytes")
        
        # Attendre que le serveur traite le message
        print("⏳ Waiting for server response...")
        time.sleep(2.0)
        
        # Essayer de recevoir
        sock.settimeout(5.0)  # Plus de temps
        try:
            response = sock.recv(1024)
            if response:
                print(f"📥 Received: {response.decode().strip()}")
            else:
                print("📥 No response received (empty)")
        except socket.timeout:
            print("⏰ No response received (timeout)")
        except Exception as e:
            print(f"❌ Error receiving: {e}")
            
        # Attendre un peu avant de fermer
        time.sleep(1.0)
            
    except Exception as e:
        print(f"❌ Connection error: {e}")
    finally:
        print("🚪 Closing connection...")
        sock.close()
        print("🚪 Connection closed")

if __name__ == "__main__":
    test_simple()