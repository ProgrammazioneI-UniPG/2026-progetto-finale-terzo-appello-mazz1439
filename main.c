#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void pulisci_schermo_main()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main()
{
    srand(time(NULL));
    int scelta;
    do
    {
        pulisci_schermo_main();

        printf("\n");
        printf("%s\n  COSE STRANE: OCCHINZ%s\n\n", B_RED, RESET);

        printf(" %s╔══════════════════════════════════╗%s\n", CYAN, RESET);
        printf(" %s║%s                                  %s║%s\n", CYAN, RESET, CYAN, RESET);
        printf(" %s║%s  [1] Imposta gioco               %s║%s\n", CYAN, B_WHITE, CYAN, RESET);
        printf(" %s║%s  [2] Gioca                       %s║%s\n", CYAN, B_WHITE, CYAN, RESET);
        printf(" %s║%s  [3] Termina gioco               %s║%s\n", CYAN, B_WHITE, CYAN, RESET);
        printf(" %s║%s  [4] Visualizza crediti          %s║%s\n", CYAN, B_WHITE, CYAN, RESET);
        printf(" %s║%s                                  %s║%s\n", CYAN, RESET, CYAN, RESET);
        printf(" %s╚══════════════════════════════════╝%s\n", CYAN, RESET);

        printf("\n %sInserisci la tua scelta:%s ", YELLOW, RESET);

        if (scanf("%d", &scelta) != 1)
        {
            while (getchar() != '\n')
                ;
            scelta = 0;
        }
        else
        {
            while (getchar() != '\n')
                ; 
        }

        switch (scelta)
        {
        case 1:
            imposta_gioco();
            break;
        case 2:
            gioca();
            break;
        case 3:
            termina_gioco();
            break;
        case 4:
            crediti();
            break;
        default:
            printf("\n %sComando non valido.%s\n", RED, RESET);
            printf(" Premere INVIO...");
            while (getchar() != '\n')
                ;
            break;
        }

    } while (scelta != 3);

    return 0;
}
