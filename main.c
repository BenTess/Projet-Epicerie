#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ==================== CONSTANTES D'AFFICHAGE ====================
#define POS_SAISIE_X 37
#define POS_SAISIE_Y 32
#define POS_LOG_X 5
#define POS_LOG_Y 35

#define CAISSE_TOTAL_X 133
#define CAISSE_TOTAL_Y 29
#define CAISSE_PAYE_X 141
#define CAISSE_PAYE_Y 30
#define CAISSE_MONNAIE_X 127
#define CAISSE_MONNAIE_Y 31

#define FINAL_MSG_X 62
#define FINAL_MSG_Y 42

// ==================== CONSTANTES POUR LES PRODUITS ====================
#define NB_CATEGORIES 3
#define ARTICLES_PAR_CATEGORIE 15
#define NB_PANIERS 2
#define CAPACITE_PANIER 9

// ==================== TABLEAUX DES CODES PRODUITS ====================
// F = Fruits, L = Légumes, V = Viandes
const char codes[NB_CATEGORIES][ARTICLES_PAR_CATEGORIE][4] = {
    {"F00", "F01", "F02", "F03", "F04", "F05", "F06", "F07", "F08", "F09", "F10", "F11", "F12", "F13", "F14"},
    {"L00", "L01", "L02", "L03", "L04", "L05", "L06", "L07", "L08", "L09", "L10", "L11", "L12", "L13", "L14"},
    {"V00", "V01", "V02", "V03", "V04", "V05", "V06", "V07", "V08", "V09", "V10", "V11", "V12", "V13", "V14"}
};

// ==================== TABLEAUX DES PRIX ====================
const float prix[NB_CATEGORIES][ARTICLES_PAR_CATEGORIE] = {
    {3.4, 4.1, 2.9, 2.2, 2.0, 3.1, 1.0, 2.5, 3.8, 2.5, 2.0, 1.6, 1.8, 3.7, 1.5},
    {2.4, 2.1, 1.9, 2.8, 1.7, 2.1, 2.9, 1.9, 1.9, 3.1, 2.1, 2.5, 2.8, 2.3, 2.7},
    {3.2, 3.9, 4.3, 3.8, 4.9, 5.0, 5.1, 2.3, 3.8, 4.0, 5.4, 3.1, 4.3, 4.8, 5.1}
};

// ==================== TABLEAUX DES EMOJIS ====================
const char emojis[NB_CATEGORIES][ARTICLES_PAR_CATEGORIE][10] = {
    {"🍓", "🍇", "🍏", "🍎", "🍐", "🍊", "🍌", "🍋", "🍉", "🍈", "🍒", "🍑", "🥥", "🍍", "🥝"},
    {"🍅", "🍆", "🥦", "🌽", "🥒", "🌶️", "🥕", "🥔", "🍠", "🥑", "🍀", "🥜", "🌱", "🌿", "🍄"},
    {"🍗", "🍖", "🥩", "🥓", "🍔", "🍟", "🍱", "🦞", "🐚", "🦀", "🐠", "🐟", "🍤", "🐡", "🐋"}
};

// ==================== VARIABLES GLOBALES POUR LES PANIERS ====================
char paniers[NB_PANIERS][CAPACITE_PANIER][4];
int nombre_articles[NB_PANIERS] = {0, 0};

// ==================== POSITIONS D'AFFICHAGE DES PANIERS ====================
// Panier 1 : 3x3 grille
int panier1_X[9] = {76, 79, 82, 76, 79, 82, 76, 79, 82};
int panier1_Y[9] = {28, 28, 28, 29, 29, 29, 30, 30, 30};

// Panier 2 : 3x3 grille
int panier2_X[9] = {91, 94, 97, 91, 94, 97, 91, 94, 97};
int panier2_Y[9] = {28, 28, 28, 29, 29, 29, 30, 30, 30};

// ==================== FONCTIONS D'AFFICHAGE ====================

// Fonction pour déplacer le curseur à une position (x, y) sur le terminal
void deplacer_curseur(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Fonction pour effacer une zone de texte
void effacer_zone(int x, int y, int longueur) {
    deplacer_curseur(x, y);
    for (int i = 0; i < longueur; i++) {
        printf(" ");
    }
}

// Fonction pour charger et afficher la vitrine depuis le fichier Maquette.txt
void afficher_vitrine(const char *nom_fichier) {
    // Effacer l'écran
    system("clear");
    
    // Ouvrir le fichier
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", nom_fichier);
        exit(1);
    }
    
    // Lire et afficher ligne par ligne
    char ligne[512];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        printf("%s", ligne);
    }
    
    fclose(fichier);
}

// ==================== FONCTIONS DE CALCUL ====================

// Fonction pour calculer le total des achats dans les deux paniers
float calculer_total() {
    float total = 0.0;
    
    // Pour chaque panier
    for (int p = 0; p < NB_PANIERS; p++) {
        // Pour chaque article dans le panier
        for (int i = 0; i < nombre_articles[p]; i++) {
            // Chercher le prix de l'article
            for (int c = 0; c < NB_CATEGORIES; c++) {
                for (int a = 0; a < ARTICLES_PAR_CATEGORIE; a++) {
                    if (strcmp(paniers[p][i], codes[c][a]) == 0) {
                        total = total + prix[c][a];
                    }
                }
            }
        }
    }
    
    return total;
}

// Fonction pour mettre à jour l'affichage du total en temps réel
void afficher_total_caisse() {
    effacer_zone(CAISSE_TOTAL_X, CAISSE_TOTAL_Y, 10);
    deplacer_curseur(CAISSE_TOTAL_X, CAISSE_TOTAL_Y);
    printf("%.2f €", calculer_total());
}

// ==================== FONCTIONS DE GESTION DES PANIERS ====================

// Fonction pour afficher les emojis des articles dans les paniers
void afficher_paniers() {
    // Effacer d'abord tous les emplacements des deux paniers
    for (int i = 0; i < 9; i++) {
        effacer_zone(panier1_X[i], panier1_Y[i], 3);
        effacer_zone(panier2_X[i], panier2_Y[i], 3);
    }
    
    // Afficher les articles de chaque panier
    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < nombre_articles[p]; i++) {
            // Déterminer la position selon le panier
            int x, y;
            if (p == 0) {
                x = panier1_X[i];
                y = panier1_Y[i];
            } else {
                x = panier2_X[i];
                y = panier2_Y[i];
            }
            
            // Trouver l'emoji correspondant au code de l'article
            for (int c = 0; c < NB_CATEGORIES; c++) {
                for (int a = 0; a < ARTICLES_PAR_CATEGORIE; a++) {
                    if (strcmp(paniers[p][i], codes[c][a]) == 0) {
                        deplacer_curseur(x, y);
                        printf("%s", emojis[c][a]);
                    }
                }
            }
        }
    }
}

// Fonction pour compter combien de fois un article est présent dans tous les paniers
int compter_article(const char *code_article) {
    int compteur = 0;
    
    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < nombre_articles[p]; i++) {
            if (strcmp(paniers[p][i], code_article) == 0) {
                compteur++;
            }
        }
    }
    
    return compteur;
}

// Fonction pour ajouter un article dans un panier
void ajouter_article(const char *code_article, int categorie_id, int article_id) {
    // Vérifier la limite de 3 exemplaires maximum
    if (compter_article(code_article) >= 3) {
        deplacer_curseur(POS_LOG_X, POS_LOG_Y);
        printf("Erreur : %s est déjà présent 3 fois !", code_article);
        return;
    }
    
    // Choisir un panier au hasard (0 ou 1)
    int p = rand() % 2;
    
    // Si le panier choisi est plein, prendre l'autre
    if (nombre_articles[p] >= CAPACITE_PANIER) {
        p = 1 - p;
    }
    
    // Vérifier que le panier a de la place
    if (nombre_articles[p] < CAPACITE_PANIER) {
        // Ajouter l'article dans le panier
        strcpy(paniers[p][nombre_articles[p]], code_article);
        nombre_articles[p]++;
        
        // Mettre à jour l'affichage
        afficher_paniers();
        afficher_total_caisse();
        
        // Afficher un message de confirmation
        deplacer_curseur(POS_LOG_X, POS_LOG_Y);
        printf("CHOIX ARTICLE : %s %s ajouté au panier %d", 
               emojis[categorie_id][article_id], code_article, p + 1);
    }
}

// Fonction pour supprimer un article d'un panier (dernière occurrence)
void supprimer_article(int numero_panier, const char *code_article) {
    int p = numero_panier - 1; // Convertir en index (0 ou 1)
    
    // Vérifier que le numéro de panier est valide
    if (p < 0 || p >= NB_PANIERS) {
        deplacer_curseur(POS_LOG_X, POS_LOG_Y);
        printf("Erreur : Numéro de panier invalide !");
        return;
    }
    
    // Chercher la dernière occurrence de l'article (en partant de la fin)
    int trouve = 0;
    for (int i = nombre_articles[p] - 1; i >= 0; i--) {
        if (strcmp(paniers[p][i], code_article) == 0) {
            // Article trouvé, on le supprime en décalant les suivants
            for (int j = i; j < nombre_articles[p] - 1; j++) {
                strcpy(paniers[p][j], paniers[p][j + 1]);
            }
            nombre_articles[p]--;
            trouve = 1;
            
            // Mettre à jour l'affichage
            afficher_paniers();
            afficher_total_caisse();
            
            deplacer_curseur(POS_LOG_X, POS_LOG_Y);
            printf("ARTICLE %s SUPPRIME DU PANIER %d", code_article, numero_panier);
            break;
        }
    }
    
    if (!trouve) {
        deplacer_curseur(POS_LOG_X, POS_LOG_Y);
        printf("Erreur : Article %s non trouvé dans le panier %d", code_article, numero_panier);
    }
}

// ==================== FONCTION PRINCIPALE ====================

int main() {
    char code_saisi[10];
    
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));
    
    // Initialiser les paniers vides
    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < CAPACITE_PANIER; i++) {
            strcpy(paniers[p][i], "");
        }
    }

    // Afficher la vitrine depuis le fichier maquette.txt
    afficher_vitrine("maquette.txt");
    
    // ==================== BOUCLE PRINCIPALE DE SAISIE ====================
    while (1) {
        // Effacer la zone de saisie
        effacer_zone(POS_SAISIE_X, POS_SAISIE_Y, 9);
        deplacer_curseur(POS_SAISIE_X, POS_SAISIE_Y);
        
        // Lire le code saisi par l'utilisateur
        scanf("%s", code_saisi);
        
        // Effacer la zone de log pour afficher les messages
        effacer_zone(POS_LOG_X, POS_LOG_Y, 70);
        
        // Si l'utilisateur tape "FIN", on arrête les courses
        if (strcmp(code_saisi, "FIN") == 0) {
            break;
        }
        
        // Si c'est une commande de suppression "SUP"
        if (strcmp(code_saisi, "SUP") == 0) {
            int numero_panier;
            char code_article[10];
            
            // Lire le numéro du panier et le code de l'article
            scanf("%d %s", &numero_panier, code_article);
            
            supprimer_article(numero_panier, code_article);
        }
        else {
            // Chercher si le code correspond à un article existant
            int trouve = 0;
            int categorie_id = 0;
            int article_id = 0;
            
            for (int c = 0; c < NB_CATEGORIES; c++) {
                for (int a = 0; a < ARTICLES_PAR_CATEGORIE; a++) {
                    if (strcmp(code_saisi, codes[c][a]) == 0) {
                        trouve = 1;
                        categorie_id = c;
                        article_id = a;
                        break;
                    }
                }
                if (trouve) break;
            }
            
            if (trouve) {
                // Ajouter l'article dans un panier
                ajouter_article(code_saisi, categorie_id, article_id);
                
                // Si les deux paniers sont pleins, passage en caisse automatique
                if (nombre_articles[0] == CAPACITE_PANIER && 
                    nombre_articles[1] == CAPACITE_PANIER) {
                    effacer_zone(POS_LOG_X, POS_LOG_Y, 70);
                    deplacer_curseur(POS_LOG_X, POS_LOG_Y);
                    printf("PANIERS PLEINS ! PASSAGE EN CAISSE AUTOMATIQUE...");
                    fflush(stdout);
                    break;
                }
            }
            else {
                // Code invalide
                deplacer_curseur(POS_LOG_X, POS_LOG_Y);
                printf("Erreur : Code invalide !");
            }
        }
        
        fflush(stdout);
    }
    
    // ==================== PHASE DE PAIEMENT ====================
    float total = calculer_total();
    
    if (total > 0) {
        float montant_paye = 0.0;
        
        // Demander le montant jusqu'à ce qu'il soit suffisant
        do {
            effacer_zone(CAISSE_PAYE_X, CAISSE_PAYE_Y, 10);
            deplacer_curseur(CAISSE_PAYE_X, CAISSE_PAYE_Y);
            scanf("%f", &montant_paye);
            
            // Tolérance de 0.01 pour les erreurs d'arrondi
            if (montant_paye < total - 0.01) {
                effacer_zone(POS_LOG_X, POS_LOG_Y, 70);
                deplacer_curseur(POS_LOG_X, POS_LOG_Y);
                printf("MONTANT INSUFFISANT ! Veuillez payer au moins %.2f €", total);
            }
        } while (montant_paye < total - 0.01);
        
        // Afficher la monnaie à rendre
        deplacer_curseur(CAISSE_MONNAIE_X, CAISSE_MONNAIE_Y);
        printf("%.2f €", montant_paye - total);
    }
    else {
        // Paniers vides
        deplacer_curseur(CAISSE_TOTAL_X, CAISSE_TOTAL_Y);
        printf("0.00 €");
    }
    
    // ==================== MESSAGE FINAL ====================
    deplacer_curseur(FINAL_MSG_X, FINAL_MSG_Y);
    printf("VOS COURSES SONT PRETES - MERCI DE VOTRE VISITE !");
    
    // Repositionner le curseur en bas de l'écran
    deplacer_curseur(1, 45);
    
    return 0;
}