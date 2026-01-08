#include <stdio.h>      // Bibliothèque standard pour les entrées/sorties (printf, fopen...)
#include <stdlib.h>     // Pour la gestion mémoire et les utilitaires (exit, rand, system, atof)
#include <string.h>     // Pour manipuler les chaînes de texte (strcmp, strcpy, strlen)
#include <termios.h>    // SPÉCIFIQUE LINUX : Pour modifier le comportement du terminal (saisie sans Entrée)
#include <unistd.h>     // SPÉCIFIQUE LINUX : Pour les constantes systèmes (STDIN_FILENO)
#include <time.h>       // Pour initialiser l'aléatoire avec l'heure actuelle

//  CONSTANTES D'AFFICHAGE 
// Ces #define permettent de changer la position des éléments facilement sans toucher au code

#define POS_SAISIE_X 37     // Position X (colonne) où l'utilisateur tape le code
#define POS_SAISIE_Y 32     // Position Y (ligne) où l'utilisateur tape le code
#define MAX_CARACT 9        // Nombre maximum de caractères qu'on peut saisir
#define POS_LOG_X 5         // Position des messages d'erreur ou d'info (X)
#define POS_LOG_Y 35        // Position des messages d'erreur ou d'info (Y)

// Coordonnées pour la zone de caisse (Total, Payé, Monnaie)
#define CAISSE_TOTAL_X 133
#define CAISSE_TOTAL_Y 29
#define CAISSE_PAYE_X 141
#define CAISSE_PAYE_Y 30
#define CAISSE_MONNAIE_X 127
#define CAISSE_MONNAIE_Y 31

#define FINAL_MSG_X 62      // Position du message de fin
#define FINAL_MSG_Y 42

//  CONSTANTES DU MAGASIN 
#define NB_CAT 3            // Nombre de catégories (Fruits, Légumes, Viandes) 
#define ART_PAR_CAT 15      // Nombre d'articles par catégorie
#define NB_PANIERS 2        // Nombre de paniers disponibles
#define CAP_PANIER 9        // Capacité maximale d'un panier (9 articles) 

// Tableau 3D constant contenant les codes articles (ex: "F00").
// Structure : [Catégorie][Article][Chaîne de 4 caractères max]

const char codes[NB_CAT][ART_PAR_CAT][4] = {
    {"F00", "F01", "F02", "F03", "F04", "F05", "F06", "F07", "F08", "F09", "F10", "F11", "F12", "F13", "F14"},
    {"L00", "L01", "L02", "L03", "L04", "L05", "L06", "L07", "L08", "L09", "L10", "L11", "L12", "L13", "L14"},
    {"V00", "V01", "V02", "V03", "V04", "V05", "V06", "V07", "V08", "V09", "V10", "V11", "V12", "V13", "V14"}
};

// Tableau 2D constant des prix correspondant aux codes ci-dessus.

const float prix[NB_CAT][ART_PAR_CAT] = {
    {3.4, 4.1, 2.9, 2.2, 2.0, 3.1, 1.0, 2.5, 3.8, 2.5, 2.0, 1.6, 1.8, 3.7, 1.5},
    {2.4, 2.1, 1.9, 2.8, 1.7, 2.1, 2.9, 1.9, 1.9, 3.1, 2.1, 2.5, 2.8, 2.3, 2.7},
    {3.2, 3.9, 4.3, 3.8, 4.9, 5.0, 5.1, 2.3, 3.8, 4.0, 5.4, 3.1, 4.3, 4.8, 5.1}
};

// Tableau 3D constant des émojis à afficher.

const char emojis[NB_CAT][ART_PAR_CAT][10] = {
    {"🍓", "🍇", "🍏", "🍎", "🍐", "🍊", "🍌", "🍋", "🍉", "🍈", "🍒", "🍑", "🥥", "🍍", "🥝"},
    {"🍅", "🍆", "🥦", "🌽", "🥒", "🌶️", "🥕", "🥔", "🍠", "🥑", "🍀", "🥜", "🌱", "🌿", "🍄"},
    {"🍗", "🍖", "🥩", "🥓", "🍔", "🍟", "🍱", "🦞", "🐚", "🦀", "🐠", "🐟", "🍤", "🐡", "🐋"}
};

// Le stock des paniers de l'utilisateur (NON constant car on le modifie).
// Paniers[Numéro panier][Index article][Code article stocké]
char paniers[NB_PANIERS][CAP_PANIER][4];

// Compteur pour savoir combien d'articles sont actuellement dans chaque panier.
int nb_articles[NB_PANIERS] = {0, 0}; 

// Tableaux contenant les coordonnées X et Y précises de chaque emplacement dans les paniers (pour l'affichage)
int panier1_X[9] = {76, 79, 82, 76, 79, 82, 76, 79, 82};
int panier1_Y[9] = {28, 28, 28, 29, 29, 29, 30, 30, 30};
int panier2_X[9] = {91, 94, 97, 91, 94, 97, 91, 94, 97};
int panier2_Y[9] = {28, 28, 28, 29, 29, 29, 30, 30, 30};

// Fonction pour déplacer le curseur du terminal avec les codes ANSI
// Format ANSI standard : \033[ligne;colonneH
void placer_curseur(int x, int y) { printf("\033[%d;%dH", y, x); }

// Fonction pour nettoyer une zone spécifique de l'écran (ex: effacer une erreur précédente)
void effacer_zone(int x, int y, int longueur) {
    placer_curseur(x, y);            // On se place au début de la zone
    for (int i = 0; i < longueur; i++) printf(" "); // On remplace par des espaces
}

// Fonction pour lire le fichier texte "maquette.txt" et l'afficher
void charger_vitrine(const char *nom_fichier) {
    if (system("clear") == -1) {}    // Efface le terminal (commande Linux). Le if vide évite un warning compilateur.
    FILE *f = fopen(nom_fichier, "r"); // Ouvre le fichier en lecture seule ("r") [cite: 70]
    if (f == NULL) exit(1);          // Si le fichier n'existe pas, on quitte le programme brutalement
    char ligne[512];                 // Buffer pour stocker chaque ligne lue
    while (fgets(ligne, sizeof(ligne), f)) printf("%s", ligne); // Lit ligne par ligne et affiche
    fclose(f);                       // Ferme le fichier proprement
}

// Calcule le prix total de tout ce qui est dans les paniers
float calculer_total(void) {
    float sum = 0.0;
    // 4 boucles imbriquées pour retrouver le prix de chaque article scanné
    for (int p = 0; p < NB_PANIERS; p++)                // Pour chaque panier
        for (int i = 0; i < nb_articles[p]; i++)        // Pour chaque article dans ce panier
            for (int c = 0; c < NB_CAT; c++)            // On cherche dans les catégories
                for (int a = 0; a < ART_PAR_CAT; a++)   // On cherche dans les articles
                    // Si le code dans le panier correspond au code en base de données :
                    if (strcmp(paniers[p][i], codes[c][a]) == 0) sum += prix[c][a]; // On ajoute le prix
    return sum;
}

[cite_start]// Affiche le total en temps réel (zone caisse) [cite: 64]
void maj_caisse_live(void) {
    effacer_zone(CAISSE_TOTAL_X, CAISSE_TOTAL_Y, 8); // Nettoie l'ancien prix
    placer_curseur(CAISSE_TOTAL_X, CAISSE_TOTAL_Y);  // Se repositionne
    printf("%.2f €", calculer_total());              // Affiche le nouveau total
}

[cite_start]// Redessine tous les émojis dans les paniers (appelé après chaque ajout/suppression) [cite: 63]
void redessiner_paniers(void) {
    // Étape 1 : On efface visuellement les deux paniers pour éviter les doublons fantômes
    for (int i = 0; i < 9; i++) {
        effacer_zone(panier1_X[i], panier1_Y[i], 2);
        effacer_zone(panier2_X[i], panier2_Y[i], 2);
    }
    // Étape 2 : On réaffiche le contenu actuel
    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < nb_articles[p]; i++) {
            // Sélectionne les bonnes coordonnées selon si c'est panier 1 ou 2
            int x = (p == 0) ? panier1_X[i] : panier2_X[i];
            int y = (p == 0) ? panier1_Y[i] : panier2_Y[i];
            
            // On cherche l'émoji correspondant au code stocké
            for (int c = 0; c < NB_CAT; c++)
                for (int a = 0; a < ART_PAR_CAT; a++)
                    if (strcmp(paniers[p][i], codes[c][a]) == 0) {
                        placer_curseur(x, y); printf("%s", emojis[c][a]); // Affiche l'émoji
                    }
        }
    }
}

// Remplace scanf pour gérer l'appui sur "Entrée" et "Effacer" manuellement
void saisir_clavier(char *destination, int limite) {
    struct termios oldt, newt; // Structures pour stocker la config du terminal
    int i = 0, c;

    // 1. On récupère la configuration actuelle
    tcgetattr(STDIN_FILENO, &oldt); 
    newt = oldt;

    // 2. On désactive ICANON (plus besoin d'appuyer sur Entrée pour valider)
    //    On désactive ECHO (les touches tapées ne s'affichent pas toutes seules, on veut contrôler l'affichage)
    newt.c_lflag &= ~(unsigned int)(ICANON | ECHO); 
    
    // 3. On applique la nouvelle configuration
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Boucle de lecture caractère par caractère
    while (1) {
        c = getchar(); // Lit une touche
        
        if (c == '\n' || c == EOF) break; // Si c'est "Entrée", on a fini de saisir

        // Gestion de la touche "Effacer" (Code 127 ou 8)
        if ((c == 127 || c == 8) && i > 0) { 
            i--;                // On recule dans notre buffer
            printf("\b \b");    // Astuce visuelle : Reculer curseur (\b), écrire espace, reculer encore
        } 
        // Si c'est un caractère valide (lettre/chiffre) et qu'on n'a pas dépassé la limite
        else if (i < limite && c >= 32 && c <= 126) {
            destination[i++] = (char)c; // On l'ajoute au buffer
            putchar(c);                 // On l'affiche manuellement (puisque ECHO est désactivé)
        }
    }
    destination[i] = '\0'; // On termine la chaîne proprement
    
    // 4. IMPORTANT : On remet le terminal dans son état d'origine avant de sortir
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void traiter_suppression(const char *commande) {
    int num_panier = 0; char code_art[10];
    
    // Analyse la commande "SUP 1 F00". Si le format n'est pas bon, on sort.
    if (sscanf(commande, "SUP %d %s", &num_panier, code_art) != 2) return;
    
    int p = num_panier - 1; // Convertit "Panier 1" en index 0
    if (p < 0 || p >= NB_PANIERS) return; // Sécurité index hors limites

    [cite_start]// Parcours à l'envers (de la fin vers le début) pour trouver la DERNIÈRE occurrence [cite: 57]
    for (int i = nb_articles[p] - 1; i >= 0; i--) {
        if (strcmp(paniers[p][i], code_art) == 0) { // Si trouvé
            [cite_start]// Décalage des articles suivants vers la gauche pour boucher le trou [cite: 58]
            for (int j = i; j < nb_articles[p] - 1; j++) strcpy(paniers[p][j], paniers[p][j+1]);
            
            nb_articles[p]--; // On réduit le compteur d'articles
            redessiner_paniers(); maj_caisse_live(); // Mise à jour visuelle
            
            // Message de confirmation
            effacer_zone(POS_LOG_X, POS_LOG_Y, 70); placer_curseur(POS_LOG_X, POS_LOG_Y);
            printf("ARTICLE %s SUPPRIME DU PANIER %d", code_art, num_panier);
            return; // On sort après avoir supprimé UNE occurrence
        }
    }
}

int main(void) {
    char code_saisi[MAX_CARACT + 1];
    srand((unsigned int)time(NULL)); [cite_start]// Initialise le générateur aléatoire [cite: 47]
    
    // Initialisation des paniers à vide
    for (int p = 0; p < NB_PANIERS; p++)
        for (int i = 0; i < CAP_PANIER; i++) strcpy(paniers[p][i], ""); 
    
    charger_vitrine("maquette.txt"); [cite_start]// Affiche le décor [cite: 76]

    // --- BOUCLE PRINCIPALE DU JEU ---
    while (1) {
        // Prépare la zone de saisie
        effacer_zone(POS_SAISIE_X, POS_SAISIE_Y, MAX_CARACT);
        placer_curseur(POS_SAISIE_X, POS_SAISIE_Y);
        
        saisir_clavier(code_saisi, MAX_CARACT); [cite_start]// Attend que l'utilisateur tape quelque chose [cite: 73]
        
        if (strcmp(code_saisi, "FIN") == 0) break; [cite_start]// Si "FIN", on sort pour aller payer [cite: 60]

        effacer_zone(POS_LOG_X, POS_LOG_Y, 70); placer_curseur(POS_LOG_X, POS_LOG_Y); // Nettoie zone info
        
        [cite_start]// Si la commande commence par "SUP ", on lance la suppression [cite: 56]
        if (strncmp(code_saisi, "SUP ", 4) == 0) traiter_suppression(code_saisi);
        else {
            // --- AJOUT D'ARTICLE ---
            int trouve = 0, cid = 0, aid = 0;
            // Vérifie si le code existe dans la "base de données"
            for (int c = 0; c < NB_CAT; c++) {
                for (int a = 0; a < ART_PAR_CAT; a++)
                    if (strcmp(code_saisi, codes[c][a]) == 0) { trouve = 1; cid = c; aid = a; break; }
                if (trouve) break;
            }

            if (trouve) {
                [cite_start]// Compte combien de fois cet article est déjà présent (Max 3) [cite: 54]
                int count = 0;
                for(int p=0; p<NB_PANIERS; p++) 
                    for(int i=0; i<nb_articles[p]; i++) 
                        if(strcmp(paniers[p][i], code_saisi) == 0) count++;
                
                if (count >= 3) printf("Erreur : %s est deja présent 3 fois !", code_saisi);
                else {
                    [cite_start]// Choix aléatoire du panier (0 ou 1) [cite: 47]
                    int p = rand() % 2;
                    [cite_start]// Si le panier choisi est plein, on essaie l'autre [cite: 48]
                    if (nb_articles[p] >= CAP_PANIER) p = 1 - p;
                    
                    // Si le panier final a de la place
                    if (nb_articles[p] < CAP_PANIER) {
                        strcpy(paniers[p][nb_articles[p]++], code_saisi); // Ajoute le code
                        redessiner_paniers(); maj_caisse_live();          // Met à jour l'écran
                        placer_curseur(POS_LOG_X, POS_LOG_Y);
                        printf("CHOIX ARTICLE : %s %s", emojis[cid][aid], code_saisi);
                        
                        [cite_start]// Si les DEUX paniers sont pleins, fin force des courses [cite: 59]
                        if (nb_articles[0] == 9 && nb_articles[1] == 9) break;
                    }
                }
            } else printf("Erreur : Code invalide !"); [cite_start]// [cite: 62]
        }
        fflush(stdout); // Force l'affichage immédiat
    }

    // --- PHASE DE CAISSE ---
    float total = calculer_total(), paye = 0.0; char montant_str[9];
    
    if (total > 0) {
        do {
            [cite_start]// Demande le paiement tant que ce n'est pas suffisant [cite: 66, 67]
            effacer_zone(CAISSE_PAYE_X, CAISSE_PAYE_Y, 8); placer_curseur(CAISSE_PAYE_X, CAISSE_PAYE_Y);
            saisir_clavier(montant_str, 8); paye = (float)atof(montant_str); // Convertit texte en float
            
            if (paye < total) {
                effacer_zone(POS_LOG_X, POS_LOG_Y, 70); placer_curseur(POS_LOG_X, POS_LOG_Y);
                printf("MONTANT INSUFFISANT !");
            }
        } while (paye < total);
        
        [cite_start]// Affiche la monnaie à rendre [cite: 68]
        placer_curseur(CAISSE_MONNAIE_X, CAISSE_MONNAIE_Y); printf("%.2f €", paye - total);
    } else {
        placer_curseur(CAISSE_TOTAL_X, CAISSE_TOTAL_Y); printf("VIDE (0.00 €)");
    }
    
    // Message de fin
    placer_curseur(FINAL_MSG_X, FINAL_MSG_Y);
    printf("VOS COURSES SONT PRÊTES MERCI DE VOTRE VISITE");
    placer_curseur(1, 45); // Place le curseur en bas pour ne pas gêner le terminal après la fin
    return 0;
}