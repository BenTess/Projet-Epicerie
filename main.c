#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h> // Indispensable pour le hasard (Étape 5)

// --- RÉGLAGES DE L'AFFICHAGE ---
#define POS_SAISIE_X 37
#define POS_SAISIE_Y 32
#define MAX_CARACT 8 
#define POS_LOG_X 5
#define POS_LOG_Y 35

// --- DONNÉES (Étape 3) ---
#define NB_CAT 3          // [cite: 40]
#define ART_PAR_CAT 15    // [cite: 11, 21, 32]
#define NB_PANIERS 2      // 
#define CAP_PANIER 9      // [cite: 59]

const char codes[NB_CAT][ART_PAR_CAT][4] = {
    {"F00", "F01", "F02", "F03", "F04", "F05", "F06", "F07", "F08", "F09", "F10", "F11", "F12", "F13", "F14"},
    {"L00", "L01", "L02", "L03", "L04", "L05", "L06", "L07", "L08", "L09", "L10", "L11", "L12", "L13", "L14"},
    {"V00", "V01", "V02", "V03", "V04", "V05", "V06", "V07", "V08", "V09", "V10", "V11", "V12", "V13", "V14"}
};

const float prix[NB_CAT][ART_PAR_CAT] = {
    {3.4, 4.1, 2.9, 2.2, 2.0, 3.1, 1.0, 2.5, 3.8, 2.5, 2.0, 1.6, 1.8, 3.7, 1.5},
    {2.4, 2.1, 1.9, 2.8, 1.7, 2.1, 2.9, 1.9, 1.9, 3.1, 2.1, 2.5, 2.8, 2.3, 2.7},
    {3.2, 3.9, 4.3, 3.8, 4.9, 5.0, 5.1, 2.3, 3.8, 4.0, 5.4, 3.1, 4.3, 4.8, 5.1}
};

const char emojis[NB_CAT][ART_PAR_CAT][10] = {
    {"🍓", "🍇", "🍏", "🍎", "🍐", "🍊", "🍌", "🍋", "🍉", "🍈", "🍒", "🍑", "🥥", "🍍", "🥝"},
    {"🍅", "🍆", "🥦", "🌽", "🥒", "🌶️", "🥕", "🥔", "🍠", "🥑", "🍀", "🥜", "🌱", "🌿", "🍄"},
    {"🍗", "🍖", "🥩", "🥓", "🍔", "🍟", "🍱", "🦞", "🐚", "🦀", "🐠", "🐟", "🍤", "🐡", "🐋"}
};

char paniers[NB_PANIERS][CAP_PANIER][4]; // Tableau 3D [cite: 98]
int nb_articles[NB_PANIERS] = {0, 0}; 

// --- FONCTIONS ---

void placer_curseur(int x, int y) {
    printf("\033[%d;%dH", y, x); // [cite: 92, 93]
}

void effacer_zone(int x, int y, int longueur) {
    placer_curseur(x, y);
    for (int i = 0; i < longueur; i++) printf(" "); // [cite: 96]
}

void charger_vitrine(const char *nom_fichier) {
    system("clear"); // [cite: 77]
    FILE *f = fopen(nom_fichier, "r");
    if (f == NULL) exit(1);
    char ligne[512];
    while (fgets(ligne, sizeof(ligne), f)) printf("%s", ligne);
    fclose(f);
}

void saisir_clavier(char *destination, int limite) {
    struct termios oldt, newt;
    int i = 0; char c;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (1) {
        c = getchar();
        if (c == '\n') break; 
        if (c == 127 || c == 8) { 
            if (i > 0) { i--; printf("\b \b"); }
        } 
        else if (i < limite && c >= 32 && c <= 126) {
            destination[i++] = c;
            putchar(c); 
        }
    }
    destination[i] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

// --- LOGIQUE ÉTAPE 5 ---

// Compte combien de fois un code est deja present dans les paniers 
int compter_articles(const char *code) {
    int total = 0;
    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < nb_articles[p]; i++) {
            if (strcmp(paniers[p][i], code) == 0) total++;
        }
    }
    return total;
}

// Tente d'ajouter un article selon la logique aleatoire [cite: 47, 48]
int tenter_ajout(const char *code) {
    int p = rand() % 2; // Choix du panier 0 ou 1 au hasard 

    // Si le panier choisi est plein, on regarde l'autre 
    if (nb_articles[p] >= CAP_PANIER) {
        p = (p == 0) ? 1 : 0;
    }

    // Si les deux paniers sont pleins [cite: 49]
    if (nb_articles[p] >= CAP_PANIER) {
        return -1; 
    }

    // On ajoute l'article
    strcpy(paniers[p][nb_articles[p]], code);
    nb_articles[p]++;
    return p; // Retourne le panier (0 ou 1) ou l'article a ete mis
}

// --- PROGRAMME PRINCIPAL ---

int main() {
    char code_saisi[MAX_CARACT + 1];
    srand(time(NULL)); // Initialise le hasard

    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < CAP_PANIER; i++) strcpy(paniers[p][i], ""); 
    }

    charger_vitrine("maquette.txt");

    while (1) {
        effacer_zone(POS_SAISIE_X, POS_SAISIE_Y, MAX_CARACT);
        placer_curseur(POS_SAISIE_X, POS_SAISIE_Y);
        saisir_clavier(code_saisi, MAX_CARACT);

        if (strcmp(code_saisi, "FIN") == 0) break; // 

        int trouve = 0;
        int cat_id = -1, art_id = -1;

        for (int c = 0; c < NB_CAT; c++) {
            for (int a = 0; a < ART_PAR_CAT; a++) {
                if (strcmp(code_saisi, codes[c][a]) == 0) {
                    trouve = 1;
                    cat_id = c;
                    art_id = a;
                    break;
                }
            }
            if (trouve) break;
        }

        effacer_zone(POS_LOG_X, POS_LOG_Y, 70); 
        placer_curseur(POS_LOG_X, POS_LOG_Y);

        if (trouve) {
            // VERIFICATION LIMITE 3 ARTICLES 
            if (compter_articles(code_saisi) >= 3) {
                printf("Erreur : %s est deja present 3 fois !", code_saisi);
            } else {
                int p_choisi = tenter_ajout(code_saisi);

                if (p_choisi != -1) {
                    // Utilisation des variables pour l'affichage (evite l'erreur unused-variable)
                    printf("Ajoute %s %s dans PANIER %d", emojis[cat_id][art_id], code_saisi, p_choisi + 1);
                    
                    // Verifier si c'est la fin automatique [cite: 59]
                    if (nb_articles[0] == CAP_PANIER && nb_articles[1] == CAP_PANIER) {
                        placer_curseur(POS_LOG_X, POS_LOG_Y + 1);
                        printf("Paniers pleins ! Passage en caisse...");
                        fflush(stdout);
                        sleep(2);
                        break;
                    }
                } else {
                    printf("Les paniers sont pleins ! Fin des courses.");
                    fflush(stdout);
                    sleep(2);
                    break;
                }
            }
        } else {
            printf("Erreur : Le code '%s' n'existe pas !", code_saisi);
        }
        
        fflush(stdout);
    }

    placer_curseur(1, 60);
    printf("Debut de la caisse...\n");
    return 0;
}