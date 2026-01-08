# Projet INF1031 : Épicerie de Nuit (NightShop) 🛒

## 📝 Présentation Générale
Ce projet consiste à modéliser en langage C une épicerie de nuit en libre-service. L'objectif est de simuler une interface de caisse interactive tournant sous environnement GNU/Linux en utilisant uniquement le terminal.

## 🚀 Fonctionnalités principales
* **Affichage de la vitrine** : Le décor du magasin est chargé depuis un fichier externe nommé `Maquette.txt`.
* **Gestion des produits** : 3 catégories (Fruits, Légumes, Viandes) avec codes, prix et émojis.
* **Paniers intelligents** :
    * Deux paniers d'une capacité de 9 articles chacun.
    * Répartition aléatoire des articles entre les paniers.
    * Limite de 3 exemplaires maximum par article.
* **Commandes disponibles** :
    * `CODE` : Ajoute l'article (ex: F02).
    * `SUP <numéro_panier> <code_article>` : Supprime la dernière occurrence d'un article.
    * `FIN` : Arrête les courses et passe en caisse.
* **Système de Caisse** : 
    * Calcul automatique du total à chaque ajout.
    * Validation du montant payé (doit être suffisant).
    * Calcul et affichage de la monnaie à rendre.

## 🛠️ Installation et Prérequis
### Prérequis
Le projet nécessite un environnement **Linux**. Pour un affichage correct des émojis, installez le pack de polices suivant :
```bash
sudo apt-get install ttf-ancient-fonts
