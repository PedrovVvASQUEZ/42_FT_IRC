#!/usr/bin/env python3
"""
Script de test intelligent qui attend que le serveur soit prêt
"""
import socket
import time
import sys
import subprocess
import os
import signal

def wait_for_server(host='localhost', port=6667, timeout=10):
    """Attend que le serveur soit prêt à accepter des connexions"""
    print(f"⏳ Waiting for server on {host}:{port}...")
    start_time = time.time()
    
    while time.time() - start_time < timeout:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(1.0)
            result = sock.connect_ex((host, port))
            sock.close()
            
            if result == 0:
                print(f"✅ Server is ready on {host}:{port}")
                return True
        except:
            pass
        
        time.sleep(0.1)  # Vérifier toutes les 100ms
    
    print(f"❌ Server not ready after {timeout} seconds")
    return False

def start_server_and_test():
    """Lance le serveur et exécute les tests"""
    server_process = None
    
    try:
        # Lancer le serveur
        print("🚀 Starting IRC server...")
        server_process = subprocess.Popen(
            ['./ircserv', '6667', 'password'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        
        # Attendre que le serveur soit prêt
        if not wait_for_server():
            print("❌ Server failed to start")
            return False
        
        # Exécuter les tests
        print("\n🧪 Running tests...")
        
        # Test simple
        test_result = subprocess.run(['python3', 'simple_test.py'])
        if test_result.returncode != 0:
            print("❌ Simple test failed")
            return False
        
        print("\n🧪 Running advanced tests...")
        # Test avancé
        test_result = subprocess.run(['python3', 'advanced_test.py'])
        if test_result.returncode != 0:
            print("❌ Advanced test failed")
            return False
        
        print("\n🎉 All tests passed!")
        return True
        
    except KeyboardInterrupt:
        print("\n⚠️  Tests interrupted by user")
        return False
    except Exception as e:
        print(f"❌ Error: {e}")
        return False
    finally:
        # Arrêter le serveur proprement
        if server_process:
            print("\n🛑 Stopping server...")
            server_process.send_signal(signal.SIGINT)
            server_process.wait(timeout=5)
            print("✅ Server stopped")

if __name__ == "__main__":
    success = start_server_and_test()
    sys.exit(0 if success else 1)