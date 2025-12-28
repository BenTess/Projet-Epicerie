1. Directives de préprocesseur (Bibliothèques)

    #include <stdio.h> : Bibliothèque standard pour les entrées/sorties (utilisée pour printf, fopen, putchar).

    #include <stdlib.h> : Utilisée pour system("clear") et exit().

    #include <string.h> : Permet d'utiliser strcmp pour comparer des chaînes de caractères.

    #include <termios.h> : Bibliothèque spécifique à Linux pour configurer le comportement du terminal (clavier).

    #include <unistd.h> : Fournit des constantes comme STDIN_FILENO (entrée standard).

2. Définition des constantes (Coordonnées)

    #define POS_SAISIE_X 37 et 32 : Coordonnées (Colonne, Ligne) où le curseur se placera pour que l'utilisateur tape son code.

    #define MAX_CARACT 8 : Limite le nombre de caractères que l'utilisateur peut saisir (longueur du cadre).

    #define POS_LOG_X 5 et 35 : Coordonnées pour afficher les messages d'information (le "Log") en bas de la vitrine.

3. Fonction placer_curseur

    void placer_curseur(int x, int y) : Reçoit une colonne x et une ligne y.

    printf("\033[%d;%dH", y, x); : Envoie le code de contrôle ANSI au terminal pour déplacer physiquement le curseur à la position souhaitée. Note que l'ordre ANSI est Ligne, puis Colonne.

4. Fonction effacer_zone

    void effacer_zone(int x, int y, int longueur) : Sert à "nettoyer" une partie de l'écran.

    for (int i = 0; i < longueur; i++) printf(" "); : Se place à l'endroit voulu et écrit des espaces vides pour effacer ce qui était écrit auparavant.

5. Fonction saisir_code_verrouille (Le "Cœur" du blocage)

Cette fonction empêche l'utilisateur d'écrire plus de 8 caractères et attend la touche "Entrée".

    struct termios oldt, newt; : Déclare deux structures pour sauvegarder et modifier l'état du terminal.

    tcgetattr(STDIN_FILENO, &oldt); : Récupère la configuration actuelle du terminal.

    newt.c_lflag &= ~(ICANON | ECHO); :

        ICANON : Désactive le mode ligne (on n'attend plus "Entrée" pour lire une touche).

        ECHO : Désactive l'affichage automatique des touches frappées à l'écran.

    tcsetattr(..., &newt); : Applique ces modifications.

    while (1) : Boucle infinie qui lit chaque touche.

        if (c == '\n') break; : Si l'utilisateur appuie sur Entrée, on sort de la boucle et on valide la saisie.

        if (c == 127 || c == 8) : Gère la touche Retour Arrière. On décrémente l'index i et on utilise \b \b (backspace) pour effacer le caractère visuellement.

        else if (i < limite && c >= 32 && c <= 126) : Si on n'a pas atteint la limite de 8, on enregistre le caractère dans dest et on l'affiche avec putchar(c). Si on dépasse 8, rien ne se passe (blocage).

    tcsetattr(..., &oldt); : Rétablit la configuration normale du terminal avant de quitter la fonction.

6. Fonction afficher_maquette

    system("clear"); : Efface tout le terminal au lancement du projet.

FILE *fichier = fopen(nom_fichier, "r"); : Ouvre ton fichier maquette.txt en mode lecture.

while (fgets(ligne, sizeof(ligne), fichier)) : Lit le fichier ligne par ligne et l'affiche sur la console.

    fclose(fichier); : Ferme le fichier proprement.

7. Fonction main (La boucle principale)

    char code[MAX_CARACT + 1]; : Tableau pour stocker le code saisi (8 lettres + le caractère de fin \0).

    afficher_maquette("maquette.txt"); : Affiche ta vitrine dès le début du programme.

    while (1) : Boucle infinie d'interaction avec l'utilisateur.

        effacer_zone(...) : Vide le cadre de saisie avant chaque nouvelle tentative.

        saisir_code_verrouille(code, MAX_CARACT); : Appelle ta fonction de saisie sécurisée.

        if (strcmp(code, "FIN") == 0) break; : Si l'utilisateur tape "FIN", le programme s'arrête.

    effacer_zone(POS_LOG_X, POS_LOG_Y, 50); : Efface l'ancien message de log.

    printf("Dernier article saisi : %s", code); : Affiche le dernier code validé par la touche "Entrée".

    fflush(stdout); : Force le terminal à afficher le texte immédiatement.

placer_curseur(1, 40); : Place le curseur tout en bas de la vitrine avant de quitter pour ne pas salir l'affichage final.