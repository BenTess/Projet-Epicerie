#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#define POS_SAISIE_X 37
#define POS_SAISIE_Y 32
#define MAX_CARACT 9 
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

#define NB_CAT 3          
#define ART_PAR_CAT 15    
#define NB_PANIERS 2      
#define CAP_PANIER 9      

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

char paniers[NB_PANIERS][CAP_PANIER][4];
int nb_articles[NB_PANIERS] = {0, 0}; 

int panier1_X[9] = {76, 79, 82, 76, 79, 82, 76, 79, 82};
int panier1_Y[9] = {28, 28, 28, 29, 29, 29, 30, 30, 30};
int panier2_X[9] = {91, 94, 97, 91, 94, 97, 91, 94, 97};
int panier2_Y[9] = {28, 28, 28, 29, 29, 29, 30, 30, 30};

void placer_curseur(int x, int y) { printf("\033[%d;%dH", y, x); }

void effacer_zone(int x, int y, int longueur) {
    placer_curseur(x, y);
    for (int i = 0; i < longueur; i++) printf(" ");
}

void charger_vitrine(const char *nom_fichier) {
    if (system("clear") == -1) {}
    FILE *f = fopen(nom_fichier, "r");
    if (f == NULL) exit(1);
    char ligne[512];
    while (fgets(ligne, sizeof(ligne), f)) printf("%s", ligne);
    fclose(f);
}

float calculer_total(void) {
    float sum = 0.0;
    for (int p = 0; p < NB_PANIERS; p++)
        for (int i = 0; i < nb_articles[p]; i++)
            for (int c = 0; c < NB_CAT; c++)
                for (int a = 0; a < ART_PAR_CAT; a++)
                    if (strcmp(paniers[p][i], codes[c][a]) == 0) sum += prix[c][a];
    return sum;
}

void maj_caisse_live(void) {
    effacer_zone(CAISSE_TOTAL_X, CAISSE_TOTAL_Y, 8);
    placer_curseur(CAISSE_TOTAL_X, CAISSE_TOTAL_Y);
    printf("%.2f €", calculer_total());
}

void redessiner_paniers(void) {
    for (int i = 0; i < 9; i++) {
        effacer_zone(panier1_X[i], panier1_Y[i], 2);
        effacer_zone(panier2_X[i], panier2_Y[i], 2);
    }
    for (int p = 0; p < NB_PANIERS; p++) {
        for (int i = 0; i < nb_articles[p]; i++) {
            int x = (p == 0) ? panier1_X[i] : panier2_X[i];
            int y = (p == 0) ? panier1_Y[i] : panier2_Y[i];
            for (int c = 0; c < NB_CAT; c++)
                for (int a = 0; a < ART_PAR_CAT; a++)
                    if (strcmp(paniers[p][i], codes[c][a]) == 0) {
                        placer_curseur(x, y); printf("%s", emojis[c][a]);
                    }
        }





        
    }
}

void saisir_clavier(char *destination, int limite) {
    struct termios oldt, newt;
    int i = 0, c;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(unsigned int)(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (1) {
        c = getchar();
        if (c == '\n' || c == EOF) break; 
        if ((c == 127 || c == 8) && i > 0) { i--; printf("\b \b"); } 
        else if (i < limite && c >= 32 && c <= 126) {
            destination[i++] = (char)c; putchar(c); 
        }
    }
    destination[i] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void traiter_suppression(const char *commande) {
    int num_panier = 0; char code_art[10];
    if (sscanf(commande, "SUP %d %s", &num_panier, code_art) != 2) return;
    int p = num_panier - 1;
    if (p < 0 || p >= NB_PANIERS) return;
    for (int i = nb_articles[p] - 1; i >= 0; i--) {
        if (strcmp(paniers[p][i], code_art) == 0) {
            for (int j = i; j < nb_articles[p] - 1; j++) strcpy(paniers[p][j], paniers[p][j+1]);
            nb_articles[p]--;
            redessiner_paniers(); maj_caisse_live();
            effacer_zone(POS_LOG_X, POS_LOG_Y, 70); placer_curseur(POS_LOG_X, POS_LOG_Y);
            printf("ARTICLE %s SUPPRIME DU PANIER %d", code_art, num_panier);
            return;
        }
    }
}

int main(void) {
    char code_saisi[MAX_CARACT + 1];
    srand((unsigned int)time(NULL)); 
    for (int p = 0; p < NB_PANIERS; p++)
        for (int i = 0; i < CAP_PANIER; i++) strcpy(paniers[p][i], ""); 
    charger_vitrine("maquette.txt");
    while (1) {
        effacer_zone(POS_SAISIE_X, POS_SAISIE_Y, MAX_CARACT);
        placer_curseur(POS_SAISIE_X, POS_SAISIE_Y);
        saisir_clavier(code_saisi, MAX_CARACT);
        if (strcmp(code_saisi, "FIN") == 0) break; 
        effacer_zone(POS_LOG_X, POS_LOG_Y, 70); placer_curseur(POS_LOG_X, POS_LOG_Y);
        if (strncmp(code_saisi, "SUP ", 4) == 0) traiter_suppression(code_saisi);
        else {
            int trouve = 0, cid = 0, aid = 0;
            for (int c = 0; c < NB_CAT; c++) {
                for (int a = 0; a < ART_PAR_CAT; a++)
                    if (strcmp(code_saisi, codes[c][a]) == 0) { trouve = 1; cid = c; aid = a; break; }
                if (trouve) break;
            }
            if (trouve) {
                int count = 0;
                for(int p=0; p<NB_PANIERS; p++) 
                    for(int i=0; i<nb_articles[p]; i++) 
                        if(strcmp(paniers[p][i], code_saisi) == 0) count++;
                if (count >= 3) printf("Erreur : %s est deja présent 3 fois !", code_saisi);
                else {
                    int p = rand() % 2;
                    if (nb_articles[p] >= CAP_PANIER) p = 1 - p;
                    if (nb_articles[p] < CAP_PANIER) {
                        strcpy(paniers[p][nb_articles[p]++], code_saisi);
                        redessiner_paniers(); maj_caisse_live();
                        placer_curseur(POS_LOG_X, POS_LOG_Y); // FIX : Repositionne ici
                        printf("CHOIX ARTICLE : %s %s", emojis[cid][aid], code_saisi);
                        if (nb_articles[0] == 9 && nb_articles[1] == 9) break;
                    }
                }
            } else printf("Erreur : Code invalide !");
        }
        fflush(stdout);
    }
    float total = calculer_total(), paye = 0.0; char montant_str[9];
    if (total > 0) {
        do {
            effacer_zone(CAISSE_PAYE_X, CAISSE_PAYE_Y, 8); placer_curseur(CAISSE_PAYE_X, CAISSE_PAYE_Y);
            saisir_clavier(montant_str, 8); paye = (float)atof(montant_str);
            if (paye < total) {
                effacer_zone(POS_LOG_X, POS_LOG_Y, 70); placer_curseur(POS_LOG_X, POS_LOG_Y);
                printf("MONTANT INSUFFISANT !");
            }
        } while (paye < total);
        placer_curseur(CAISSE_MONNAIE_X, CAISSE_MONNAIE_Y); printf("%.2f €", paye - total);
    } else {
        placer_curseur(CAISSE_TOTAL_X, CAISSE_TOTAL_Y); printf("VIDE (0.00 €)");
    }
    placer_curseur(FINAL_MSG_X, FINAL_MSG_Y);
    printf("VOS COURSES SONT PRÊTES MERCI DE VOTRE VISITE");
    placer_curseur(1, 45); return 0;
}