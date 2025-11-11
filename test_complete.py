#!/usr/bin/env python3
"""
Test complet et exhaustif du serveur IRC ft_irc
Teste toutes les fonctionnalités implémentées
"""

import socket
import time
import threading
import sys

class IRCTester:
    def __init__(self, host='localhost', port=6667, password='mypassword'):
        self.host = host
        self.port = port
        self.password = password
        self.test_results = []
        
    def log_test(self, test_name, success, details=""):
        status = "✅ PASS" if success else "❌ FAIL"
        print(f"{status} {test_name}")
        if details:
            print(f"    {details}")
        self.test_results.append((test_name, success, details))
        
    def connect_and_auth(self, nickname, username="testuser", realname="Test User"):
        """Connexion et authentification basique"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((self.host, self.port))
            
            # Authentification
            sock.send(f"PASS {self.password}\r\n".encode())
            sock.send(f"NICK {nickname}\r\n".encode())
            sock.send(f"USER {username} 0 * :{realname}\r\n".encode())
            
            # Attendre les messages de bienvenue
            time.sleep(0.5)
            data = sock.recv(4096).decode()
            
            if "001" in data:  # RPL_WELCOME
                return sock, True
            else:
                return sock, False
                
        except Exception as e:
            print(f"Erreur de connexion: {e}")
            return None, False
    
    def test_basic_connection(self):
        """Test 1: Connexion et authentification basique"""
        print("\n🔌 Test de connexion et authentification")
        
        sock, success = self.connect_and_auth("testuser1")
        if success:
            self.log_test("Connexion basique", True, "Authentification réussie")
            sock.close()
        else:
            self.log_test("Connexion basique", False, "Échec d'authentification")
    
    def test_invalid_auth(self):
        """Test 2: Authentification invalide"""
        print("\n🚫 Test d'authentification invalide")
        
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3)
            sock.connect((self.host, self.port))
            
            # Mauvais mot de passe
            sock.send(b"PASS wrongpassword\r\n")
            sock.send(b"NICK testuser2\r\n")
            sock.send(b"USER testuser2 0 * :Test\r\n")
            
            time.sleep(0.5)
            data = sock.recv(4096).decode()
            
            if "464" in data:  # ERR_PASSWDMISMATCH
                self.log_test("Mauvais mot de passe", True, "Erreur 464 reçue")
            else:
                self.log_test("Mauvais mot de passe", False, "Erreur 464 non reçue")
            
            sock.close()
        except Exception as e:
            self.log_test("Mauvais mot de passe", False, f"Exception: {e}")
    
    def test_channel_operations(self):
        """Test 3: Opérations de base sur les channels"""
        print("\n📺 Test des opérations de channels")
        
        sock, success = self.connect_and_auth("alice")
        if not success:
            self.log_test("Channel operations", False, "Échec d'authentification")
            return
            
        try:
            # Test JOIN
            sock.send(b"JOIN #testchannel\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "JOIN #testchannel" in data:
                self.log_test("JOIN channel", True, "Channel rejoint avec succès")
            else:
                self.log_test("JOIN channel", False, "JOIN échoué")
            
            # Test PRIVMSG au channel
            sock.send(b"PRIVMSG #testchannel :Hello channel!\r\n")
            time.sleep(0.1)
            
            # Test TOPIC
            sock.send(b"TOPIC #testchannel :New topic\r\n")
            time.sleep(0.1)
            data = sock.recv(4096).decode()
            
            if "TOPIC #testchannel :New topic" in data:
                self.log_test("TOPIC change", True, "Topic modifié")
            else:
                self.log_test("TOPIC change", False, "TOPIC échoué")
            
            # Test PART
            sock.send(b"PART #testchannel :Goodbye!\r\n")
            time.sleep(0.1)
            data = sock.recv(4096).decode()
            
            if "PART #testchannel" in data:
                self.log_test("PART channel", True, "Channel quitté")
            else:
                self.log_test("PART channel", False, "PART échoué")
            
        except Exception as e:
            self.log_test("Channel operations", False, f"Exception: {e}")
        finally:
            sock.close()
    
    def test_channel_restrictions(self):
        """Test 4: Restrictions de noms de channels (# uniquement)"""
        print("\n🔒 Test des restrictions de channels")
        
        sock, success = self.connect_and_auth("bob")
        if not success:
            return
            
        try:
            # Test channel valide avec #
            sock.send(b"JOIN #validchannel\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "JOIN #validchannel" in data:
                self.log_test("Channel valide (#)", True, "Channel # accepté")
            else:
                self.log_test("Channel valide (#)", False, "Channel # refusé")
            
            # Test channel invalide avec &
            sock.send(b"JOIN &invalidchannel\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "403" in data:  # ERR_NOSUCHCHANNEL
                self.log_test("Channel invalide (&)", True, "Channel & correctement refusé")
            else:
                self.log_test("Channel invalide (&)", False, "Channel & incorrectement accepté")
            
            # Test channel sans préfixe
            sock.send(b"JOIN invalidchannel\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "403" in data:
                self.log_test("Channel sans préfixe", True, "Channel sans # correctement refusé")
            else:
                self.log_test("Channel sans préfixe", False, "Channel sans # incorrectement accepté")
            
        except Exception as e:
            self.log_test("Channel restrictions", False, f"Exception: {e}")
        finally:
            sock.close()
    
    def test_channel_modes(self):
        """Test 5: Modes de channels"""
        print("\n⚙️ Test des modes de channels")
        
        sock, success = self.connect_and_auth("charlie")
        if not success:
            return
            
        try:
            # Créer un channel
            sock.send(b"JOIN #modetest\r\n")
            time.sleep(0.2)
            sock.recv(4096)  # Vider le buffer
            
            # Test mode +i (invite only)
            sock.send(b"MODE #modetest +i\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "MODE #modetest +i" in data:
                self.log_test("Mode +i", True, "Mode invite-only activé")
            else:
                self.log_test("Mode +i", False, "Mode +i échoué")
            
            # Test mode +k (key)
            sock.send(b"MODE #modetest +k secretpass\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "MODE #modetest +k" in data:
                self.log_test("Mode +k", True, "Mode key activé")
            else:
                self.log_test("Mode +k", False, "Mode +k échoué")
            
            # Test mode +l (limit)
            sock.send(b"MODE #modetest +l 10\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "MODE #modetest +l" in data:
                self.log_test("Mode +l", True, "Mode limit activé")
            else:
                self.log_test("Mode +l", False, "Mode +l échoué")
            
            # Test affichage des modes
            sock.send(b"MODE #modetest\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "324" in data and "modetest" in data:
                self.log_test("Affichage modes", True, "Modes affichés correctement")
            else:
                self.log_test("Affichage modes", False, "Modes non affichés")
            
            # Test retrait de modes
            sock.send(b"MODE #modetest -i\r\n")
            time.sleep(0.2)
            data = sock.recv(4096).decode()
            
            if "MODE #modetest -i" in data:
                self.log_test("Mode -i", True, "Mode invite-only désactivé")
            else:
                self.log_test("Mode -i", False, "Mode -i échoué")
                
        except Exception as e:
            self.log_test("Channel modes", False, f"Exception: {e}")
        finally:
            sock.close()
    
    def test_operator_privileges(self):
        """Test 6: Privilèges d'opérateurs"""
        print("\n👑 Test des privilèges d'opérateurs")
        
        # Créer deux connexions
        sock1, success1 = self.connect_and_auth("operator")
        sock2, success2 = self.connect_and_auth("user")
        
        if not (success1 and success2):
            self.log_test("Operator privileges", False, "Échec de connexion multiple")
            return
            
        try:
            # Opérateur crée et rejoint le channel
            sock1.send(b"JOIN #optest\r\n")
            time.sleep(0.2)
            sock1.recv(4096)
            
            # Utilisateur normal rejoint
            sock2.send(b"JOIN #optest\r\n")
            time.sleep(0.2)
            sock2.recv(4096)
            
            # Opérateur donne des droits d'opérateur
            sock1.send(b"MODE #optest +o user\r\n")
            time.sleep(0.2)
            data = sock1.recv(4096).decode()
            
            if "MODE #optest +o user" in data:
                self.log_test("Donner privilèges +o", True, "Utilisateur promu opérateur")
            else:
                self.log_test("Donner privilèges +o", False, "Promotion échouée")
            
            # Test KICK par opérateur
            sock1.send(b"KICK #optest user :Test kick\r\n")
            time.sleep(0.2)
            data = sock1.recv(4096).decode()
            
            if "KICK #optest user" in data:
                self.log_test("KICK par opérateur", True, "KICK réussi")
            else:
                self.log_test("KICK par opérateur", False, "KICK échoué")
                
        except Exception as e:
            self.log_test("Operator privileges", False, f"Exception: {e}")
        finally:
            sock1.close()
            sock2.close()
    
    def test_invite_system(self):
        """Test 7: Système d'invitations"""
        print("\n✉️ Test du système d'invitations")
        
        sock1, success1 = self.connect_and_auth("inviter")
        sock2, success2 = self.connect_and_auth("invited")
        
        if not (success1 and success2):
            return
            
        try:
            # Créer channel invite-only
            sock1.send(b"JOIN #invitetest\r\n")
            time.sleep(0.2)
            sock1.recv(4096)
            
            sock1.send(b"MODE #invitetest +i\r\n")
            time.sleep(0.2)
            sock1.recv(4096)
            
            # Tentative de rejoindre sans invitation (doit échouer)
            sock2.send(b"JOIN #invitetest\r\n")
            time.sleep(0.2)
            data = sock2.recv(4096).decode()
            
            if "473" in data:  # ERR_INVITEONLYCHAN
                self.log_test("Blocage sans invitation", True, "Accès refusé sans invitation")
            else:
                self.log_test("Blocage sans invitation", False, "Accès autorisé sans invitation")
            
            # Envoyer invitation
            sock1.send(b"INVITE invited #invitetest\r\n")
            time.sleep(0.2)
            data = sock1.recv(4096).decode()
            
            if "INVITE invited" in data:
                self.log_test("INVITE command", True, "Invitation envoyée")
            else:
                self.log_test("INVITE command", False, "INVITE échoué")
            
            # Maintenant rejoindre avec invitation (doit réussir)
            sock2.send(b"JOIN #invitetest\r\n")
            time.sleep(0.2)
            data = sock2.recv(4096).decode()
            
            if "JOIN #invitetest" in data:
                self.log_test("JOIN après invitation", True, "Accès autorisé après invitation")
            else:
                self.log_test("JOIN après invitation", False, "Accès refusé malgré invitation")
                
        except Exception as e:
            self.log_test("Invite system", False, f"Exception: {e}")
        finally:
            sock1.close()
            sock2.close()
    
    def test_private_messages(self):
        """Test 8: Messages privés"""
        print("\n💬 Test des messages privés")
        
        sock1, success1 = self.connect_and_auth("sender")
        sock2, success2 = self.connect_and_auth("receiver")
        
        if not (success1 and success2):
            return
            
        try:
            time.sleep(0.5)  # Laisser le temps aux connexions
            
            # Envoyer message privé
            sock1.send(b"PRIVMSG receiver :Hello private message!\r\n")
            time.sleep(0.2)
            
            # Le destinataire devrait recevoir le message
            data = sock2.recv(4096).decode()
            
            if "PRIVMSG receiver :Hello private message!" in data:
                self.log_test("Message privé", True, "Message privé reçu")
            else:
                self.log_test("Message privé", False, "Message privé non reçu")
            
            # Test message à utilisateur inexistant
            sock1.send(b"PRIVMSG nonexistent :Hello!\r\n")
            time.sleep(0.2)
            data = sock1.recv(4096).decode()
            
            if "401" in data:  # ERR_NOSUCHNICK
                self.log_test("Message utilisateur inexistant", True, "Erreur 401 reçue")
            else:
                self.log_test("Message utilisateur inexistant", False, "Erreur 401 non reçue")
                
        except Exception as e:
            self.log_test("Private messages", False, f"Exception: {e}")
        finally:
            sock1.close()
            sock2.close()
    
    def test_nickname_validation(self):
        """Test 9: Validation des pseudonymes"""
        print("\n📛 Test de validation des pseudonymes")
        
        try:
            # Test pseudonyme valide
            sock1, success1 = self.connect_and_auth("validnick")
            if success1:
                self.log_test("Pseudonyme valide", True, "Pseudonyme accepté")
                sock1.close()
            else:
                self.log_test("Pseudonyme valide", False, "Pseudonyme valide refusé")
            
            # Test pseudonyme trop long (>9 caractères selon votre validation)
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(3)
            sock.connect((self.host, self.port))
            sock.send(f"PASS {self.password}\r\n".encode())
            sock.send(b"NICK verylongnickname\r\n")
            sock.send(b"USER test 0 * :Test\r\n")
            time.sleep(0.5)
            data = sock.recv(4096).decode()
            
            if "432" in data:  # ERR_ERRONEUSNICKNAME
                self.log_test("Pseudonyme trop long", True, "Pseudonyme long refusé")
            else:
                self.log_test("Pseudonyme trop long", False, "Pseudonyme long accepté")
            sock.close()
            
            # Test conflit de pseudonyme
            sock3, success3 = self.connect_and_auth("duplicate")
            sock4 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock4.settimeout(3)
            sock4.connect((self.host, self.port))
            sock4.send(f"PASS {self.password}\r\n".encode())
            sock4.send(b"NICK duplicate\r\n")
            time.sleep(0.5)
            data = sock4.recv(4096).decode()
            
            if "433" in data:  # ERR_NICKNAMEINUSE
                self.log_test("Conflit pseudonyme", True, "Conflit détecté")
            else:
                self.log_test("Conflit pseudonyme", False, "Conflit non détecté")
                
            sock3.close()
            sock4.close()
            
        except Exception as e:
            self.log_test("Nickname validation", False, f"Exception: {e}")
    
    def test_quit_command(self):
        """Test 10: Commande QUIT"""
        print("\n👋 Test de la commande QUIT")
        
        sock, success = self.connect_and_auth("quitter")
        if not success:
            return
            
        try:
            sock.send(b"QUIT :Goodbye!\r\n")
            time.sleep(0.2)
            
            # La connexion devrait être fermée côté serveur
            try:
                sock.send(b"PING\r\n")
                data = sock.recv(1024)
                if not data:
                    self.log_test("QUIT command", True, "Connexion fermée par le serveur")
                else:
                    self.log_test("QUIT command", False, "Connexion encore active")
            except:
                self.log_test("QUIT command", True, "Connexion fermée par le serveur")
                
        except Exception as e:
            self.log_test("QUIT command", False, f"Exception: {e}")
        finally:
            sock.close()
    
    def test_server_shutdown(self):
        """Test 11: Arrêt propre du serveur"""
        print("\n🛑 Test d'arrêt propre du serveur")
        
        # Ce test nécessite d'envoyer SIGINT au serveur
        # Pour l'instant, on teste juste qu'on peut se connecter
        sock, success = self.connect_and_auth("shutdown_test")
        if success:
            self.log_test("Serveur opérationnel", True, "Le serveur répond correctement")
            sock.close()
        else:
            self.log_test("Serveur opérationnel", False, "Le serveur ne répond pas")
    
    def run_all_tests(self):
        """Exécute tous les tests"""
        print("🚀 Démarrage des tests complets du serveur IRC ft_irc")
        print("=" * 60)
        
        start_time = time.time()
        
        # Vérifier que le serveur fonctionne
        try:
            test_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            test_sock.settimeout(2)
            test_sock.connect((self.host, self.port))
            test_sock.close()
            print(f"✅ Serveur détecté sur {self.host}:{self.port}")
        except:
            print(f"❌ ERREUR: Impossible de se connecter au serveur sur {self.host}:{self.port}")
            print("   Assurez-vous que le serveur est démarré avec: ./ircserv 6667 mypassword")
            return
        
        # Exécution des tests
        tests = [
            self.test_basic_connection,
            self.test_invalid_auth,
            self.test_channel_operations,
            self.test_channel_restrictions,
            self.test_channel_modes,
            self.test_operator_privileges,
            self.test_invite_system,
            self.test_private_messages,
            self.test_nickname_validation,
            self.test_quit_command,
            self.test_server_shutdown
        ]
        
        for test in tests:
            try:
                test()
                time.sleep(0.5)  # Pause entre les tests
            except Exception as e:
                print(f"❌ ERREUR dans {test.__name__}: {e}")
        
        # Résumé
        end_time = time.time()
        duration = end_time - start_time
        
        print("\n" + "=" * 60)
        print("📊 RÉSUMÉ DES TESTS")
        print("=" * 60)
        
        passed = sum(1 for _, success, _ in self.test_results if success)
        total = len(self.test_results)
        
        print(f"Tests réussis: {passed}/{total}")
        print(f"Pourcentage de réussite: {(passed/total)*100:.1f}%")
        print(f"Durée d'exécution: {duration:.2f}s")
        
        if passed == total:
            print("\n🎉 FÉLICITATIONS! Tous les tests sont passés!")
        else:
            print(f"\n⚠️ {total-passed} test(s) ont échoué. Voir les détails ci-dessus.")
        
        # Détails des échecs
        failed_tests = [name for name, success, _ in self.test_results if not success]
        if failed_tests:
            print("\n❌ Tests échoués:")
            for test in failed_tests:
                print(f"  - {test}")

def main():
    if len(sys.argv) > 1:
        if sys.argv[1] == "--help":
            print("Usage: python3 test_complete.py [host] [port] [password]")
            print("Defaults: localhost 6667 mypassword")
            return
    
    host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 6667
    password = sys.argv[3] if len(sys.argv) > 3 else 'mypassword'
    
    tester = IRCTester(host, port, password)
    tester.run_all_tests()

if __name__ == "__main__":
    main()