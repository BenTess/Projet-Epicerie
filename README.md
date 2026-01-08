# Projet INF1031 : Épicerie de Nuit (NightShop) 🛒

## 📝 Présentation Générale
[cite_start]Ce projet consiste à modéliser en langage C une épicerie de nuit en libre-service[cite: 84]. [cite_start]L'objectif est de simuler une interface de caisse interactive tournant sous environnement GNU/Linux en utilisant uniquement le terminal[cite: 85].

## 🚀 Fonctionnalités principales
* [cite_start]**Affichage de la vitrine** : Le décor du magasin est chargé depuis un fichier externe nommé `Maquette.txt`[cite: 152, 153].
* [cite_start]**Gestion des produits** : 3 catégories (Fruits, Légumes, Viandes) avec codes, prix et émojis[cite: 117, 122, 177].
* **Paniers intelligents** :
    * [cite_start]Deux paniers d'une capacité de 9 articles chacun[cite: 136].
    * [cite_start]Répartition aléatoire des articles entre les paniers[cite: 124].
    * [cite_start]Limite de 3 exemplaires maximum par article[cite: 131].
* **Commandes disponibles** :
    * [cite_start]`CODE` : Ajoute l'article (ex: F02)[cite: 124].
    * [cite_start]`SUP <numéro_panier> <code_article>` : Supprime la dernière occurrence d'un article[cite: 133, 134].
    * [cite_start]`FIN` : Arrête les courses et passe en caisse[cite: 137].
* **Système de Caisse** : 
    * [cite_start]Calcul automatique du total à chaque ajout[cite: 141].
    * [cite_start]Validation du montant payé (doit être suffisant)[cite: 143].
    * [cite_start]Calcul et affichage de la monnaie à rendre[cite: 145].

## 🛠️ Installation et Prérequis
### Prérequis
[cite_start]Le projet nécessite un environnement **Linux**[cite: 85]. Pour un affichage correct des émojis, installez le pack de polices suivant :
```bash
sudo apt-get install ttf-ancient-fonts
