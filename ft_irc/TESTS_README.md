# 🧪 Tests pour le serveur IRC

## 📋 Fichiers de test disponibles

### 🚀 **Tests automatiques (RECOMMANDÉS)**

#### `run_tests.py` ⭐ **PRINCIPAL**
Script intelligent qui lance le serveur, attend qu'il soit prêt, exécute tous les tests et arrête le serveur proprement.
```bash
./run_tests.py
```

#### `quick_test.sh` 🔧 **SIMPLE**  
Script bash rapide avec timer fixe (2 secondes).
```bash
./quick_test.sh
```

### 🎯 **Tests manuels**

#### `simple_test.py` 📝 **BASIQUE**
Test de connexion simple avec un seul client.
```bash
# Lancer d'abord le serveur
./ircserv 6667 password &

# Puis le test
python3 simple_test.py
```

#### `advanced_test.py` 🏆 **COMPLET**
Tests avancés avec plusieurs clients simultanés, gestion des déconnexions, etc.
```bash
# Lancer d'abord le serveur  
./ircserv 6667 password &

# Puis les tests avancés
python3 advanced_test.py
```

## 🎯 **Utilisation recommandée**

Pour le développement quotidien :
```bash
# Test complet automatique
./run_tests.py
```

Pour des tests rapides :
```bash  
# Test simple automatique
./quick_test.sh
```

## 📊 **Ce que testent les scripts**

- ✅ Connexions multiples simultanées
- ✅ Envoi/réception de messages  
- ✅ Gestion des déconnexions
- ✅ Réutilisation des file descriptors
- ✅ Pas de fuites de ressources
- ✅ Gestion des erreurs réseau

## 🔧 **Prérequis**

- Serveur IRC compilé (`make`)
- Python 3 installé
- Port 6667 disponible

<!-- cd /workspaces/FT_IRC/ft_irc && rm test_main.cpp test_client.py run_test.sh -->