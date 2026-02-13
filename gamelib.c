#include "gamelib.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// --- VARIABILI GLOBALI ---
static Giocatore *giocatori[4] = {NULL, NULL, NULL, NULL};
static struct Zona_mondoreale *prima_zona_mondoreale = NULL;
static struct Zona_soprasotto *prima_zona_soprasotto = NULL;

static int scelta_mappa;
static int undici_scelto = 0;
static int mappa_completa = 0;
static int gioco_in_corso = 0;

static int buff_maglietta_attivo = 0;
static int buff_schitarrata_attivo = 0;
static int oggetto_usato_in_questo_turno = 0;

static void pulisci_schermo()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void premi_invio()
{
    printf("\n%sPremere INVIO per continuare...%s", YELLOW, RESET);
    while (getchar() != '\n')
        ;
}

static const char *get_nome_zona(int tipo)
{
    switch (tipo)
    {
    case bosco:
        return "Bosco";
    case scuola:
        return "Scuola";
    case laboratorio:
        return "Laboratorio";
    case caverna:
        return "Caverna";
    case strada:
        return "Strada";
    case giardino:
        return "Giardino";
    case supermercato:
        return "Supermercato";
    case centrale_elettrica:
        return "Centrale Elettrica";
    case deposito_abbandonato:
        return "Deposito Abbandonato";
    case stazione_polizia:
        return "Stazione Polizia";
    default:
        return "Sconosciuto";
    }
}

static const char *get_nome_nemico(int nemico)
{
    switch (nemico)
    {
    case nessun_nemico:
        return GREEN "Nessuno" RESET;
    case billi:
        return RED "Billi" RESET;
    case democane:
        return RED "Democane" RESET;
    case demotorzone:
        return MAGENTA BOLD "DEMOTORZONE" RESET;
    default:
        return "?";
    }
}

static const char *get_nome_oggetto(int oggetto)
{
    switch (oggetto)
    {
    case nessun_oggetto:
        return "Nessuno";
    case bicicletta:
        return CYAN "Bicicletta" RESET;
    case maglietta_fuocoinferno:
        return CYAN "Maglietta Fuocoinferno" RESET;
    case bussola:
        return CYAN "Bussola" RESET;
    case schitarrata_metallica:
        return CYAN "Schitarrata Metallica" RESET;
    default:
        return "?";
    }
}

static int get_numero_zone()
{
    int num_zone = 0;
    struct Zona_mondoreale *temp_zona = prima_zona_mondoreale;
    while (temp_zona != NULL)
    {
        temp_zona = temp_zona->avanti;
        num_zone++;
    }
    return num_zone;
}

static int check_demotorzone()
{
    int demotorzone_ok = 0;
    struct Zona_soprasotto *temp_zona = prima_zona_soprasotto;
    while (temp_zona != NULL)
    {
        if (temp_zona->nemico == demotorzone)
        {
            demotorzone_ok++;
        }
        temp_zona = temp_zona->avanti;
    }
    return (demotorzone_ok == 1) ? 1 : 0;
}

static void elimina_mappa()
{
    // MONDO REALE
    struct Zona_mondoreale *zona_attuale_mondo = prima_zona_mondoreale;
    struct Zona_mondoreale *prossima_zona_mondo;

    while (zona_attuale_mondo != NULL)
    {
        prossima_zona_mondo = zona_attuale_mondo->avanti;
        free(zona_attuale_mondo);
        zona_attuale_mondo = prossima_zona_mondo;
    }
    prima_zona_mondoreale = NULL;

    // SOPRASOTTO
    struct Zona_soprasotto *zona_attuale_sotto = prima_zona_soprasotto;
    struct Zona_soprasotto *prossima_zona_sotto;

    while (zona_attuale_sotto != NULL)
    {
        prossima_zona_sotto = zona_attuale_sotto->avanti;
        free(zona_attuale_sotto);
        zona_attuale_sotto = prossima_zona_sotto;
    }
    prima_zona_soprasotto = NULL;
}

static void elimina_giocatori()
{
    // Scorre l'array dei giocatori e libera la memoria
    for (int i = 0; i < 4; i++)
    {
        if (giocatori[i] != NULL)
        {
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }
}

static void salva_vincitore(const char *nome_vincitore)
{
    char vincitori[3][32];
    int count = 0;

    FILE *f = fopen("vincitori.txt", "r");
    if (f != NULL)
    {
        while (count < 3 && fscanf(f, "%31s", vincitori[count]) == 1)
        {
            count++;
        }
        fclose(f);
    }

    if (count < 3)
    {
        strcpy(vincitori[count], nome_vincitore);
        count++;
    }
    else
    {
        strcpy(vincitori[0], vincitori[1]);
        strcpy(vincitori[1], vincitori[2]);
        strcpy(vincitori[2], nome_vincitore);
    }

    f = fopen("vincitori.txt", "w");
    if (f != NULL)
    {
        for (int i = 0; i < count; i++)
        {
            fprintf(f, "%s\n", vincitori[i]);
        }
        fclose(f);
    }
}

static void genera_mappa()
{
    pulisci_schermo();
    elimina_mappa();

    struct Zona_mondoreale *ultima_zona_mondoreale = NULL;
    struct Zona_soprasotto *ultima_zona_soprasotto = NULL;

    int pos_demotorzone = rand() % 15;

    for (int i = 0; i < 15; i++)
    {
        struct Zona_mondoreale *nuova_zona_mondoreale = (struct Zona_mondoreale *)malloc(sizeof(struct Zona_mondoreale));
        struct Zona_soprasotto *nuova_zona_soprasotto = (struct Zona_soprasotto *)malloc(sizeof(struct Zona_soprasotto));

        // Assegnazione Tipo
        nuova_zona_mondoreale->tipo = rand() % 10;
        nuova_zona_soprasotto->tipo = nuova_zona_mondoreale->tipo;

        // Assegnazione Nemici e Oggetti Mondo Reale
        int r = rand() % 100;
        if (r < 60)
            nuova_zona_mondoreale->nemico = nessun_nemico; // 60% probabilità
        else if (r < 85)
            nuova_zona_mondoreale->nemico = democane; // 25% probabilità
        else
            nuova_zona_mondoreale->nemico = billi; // 15% probabilità

        r = rand() % 100;
        if (r < 30)
            nuova_zona_mondoreale->oggetto = nessun_oggetto; // 30% probabilità
        else if (r < 50)
            nuova_zona_mondoreale->oggetto = bicicletta; // 20% probabilità
        else if (r < 75)
            nuova_zona_mondoreale->oggetto = maglietta_fuocoinferno; // 25% probabilità
        else if (r < 90)
            nuova_zona_mondoreale->oggetto = bussola; // 15% probabilità
        else
            nuova_zona_mondoreale->oggetto = schitarrata_metallica; // 10% probabilità

        // Assegnazione Nemici Soprasotto
        if (i == pos_demotorzone)
        {
            nuova_zona_soprasotto->nemico = demotorzone;
        }
        else
        {
            int r2 = rand() % 100;
            if (r2 < 35)
                nuova_zona_soprasotto->nemico = nessun_nemico;
            else
                nuova_zona_soprasotto->nemico = democane;
        }

        // Collegamenti Verticali
        nuova_zona_mondoreale->link_soprasotto = nuova_zona_soprasotto;
        nuova_zona_soprasotto->link_mondoreale = nuova_zona_mondoreale;

        // Inizializzazione puntatori
        nuova_zona_mondoreale->avanti = NULL;
        nuova_zona_soprasotto->avanti = NULL;
        nuova_zona_mondoreale->indietro = NULL;
        nuova_zona_soprasotto->indietro = NULL;

        // Collegamenti Orizzontali
        if (prima_zona_mondoreale != NULL)
        {
            nuova_zona_mondoreale->indietro = ultima_zona_mondoreale;
            nuova_zona_soprasotto->indietro = ultima_zona_soprasotto;

            ultima_zona_mondoreale->avanti = nuova_zona_mondoreale;
            ultima_zona_soprasotto->avanti = nuova_zona_soprasotto;
        }
        else
        {
            prima_zona_mondoreale = nuova_zona_mondoreale;
            prima_zona_soprasotto = nuova_zona_soprasotto;
        }

        ultima_zona_mondoreale = nuova_zona_mondoreale;
        ultima_zona_soprasotto = nuova_zona_soprasotto;
    }

    mappa_completa = 0;
    printf("%sMappa generata con successo! (15 zone)%s\n", GREEN, RESET);
    premi_invio();
}

static void inserisci_zona()
{
    pulisci_schermo();
    printf("=== INSERIMENTO NUOVA ZONA ===\n");
    int pos_zona;
    int max = get_numero_zone();
    do
    {
        printf("Inserisci posizione (0-%d): ", max);
        if (scanf("%d", &pos_zona) != 1)
        {
            printf("Errore: Inserire solo numeri!");
            while (getchar() != '\n')
                ;
            pos_zona = -1;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (pos_zona < 0 || pos_zona > max);

    struct Zona_mondoreale *nuova_zona_mondoreale = (struct Zona_mondoreale *)malloc(sizeof(struct Zona_mondoreale));
    struct Zona_soprasotto *nuova_zona_soprasotto = (struct Zona_soprasotto *)malloc(sizeof(struct Zona_soprasotto));

    // Collegamento verticale
    nuova_zona_mondoreale->link_soprasotto = nuova_zona_soprasotto;
    nuova_zona_soprasotto->link_mondoreale = nuova_zona_mondoreale;

    // Inizializzazione sicura
    nuova_zona_mondoreale->avanti = NULL;
    nuova_zona_mondoreale->indietro = NULL;
    nuova_zona_soprasotto->avanti = NULL;
    nuova_zona_soprasotto->indietro = NULL;

    // --- TIPO ZONA ---
    int scelta_tipo;
    printf("\nScegli il tipo della zona:\n");
    printf("[1] Bosco\n[2] Scuola\n[3] Laboratorio\n[4] Caverna\n[5] Strada\n[6] Giardino\n[7] Supermercato\n[8] Centrale Elettrica\n[9] Deposito Abbandonato\n[10] Stazione di Polizia\n");
    do
    {
        printf("Scelta (1-10): ");
        if (scanf("%d", &scelta_tipo) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            scelta_tipo = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (scelta_tipo < 1 || scelta_tipo > 10);

    nuova_zona_mondoreale->tipo = (enum Tipo_zona)(scelta_tipo - 1);
    nuova_zona_soprasotto->tipo = nuova_zona_mondoreale->tipo;

    printf("--> Hai scelto: %s%s%s\n", BLUE, get_nome_zona(nuova_zona_mondoreale->tipo), RESET);

    // --- NEMICO MONDO REALE ---
    int scelta_nemico_m;
    printf("\nNemico Mondo Reale: [1] Nessuno [2] Billi [3] Democane\n");
    do
    {
        printf("Scelta: ");
        if (scanf("%d", &scelta_nemico_m) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            scelta_nemico_m = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (scelta_nemico_m < 1 || scelta_nemico_m > 3);
    nuova_zona_mondoreale->nemico = (enum Tipo_nemico)(scelta_nemico_m - 1);

    // --- OGGETTO MONDO REALE ---
    int sc_ogg;
    printf("\nOggetto: [1] Nessuno [2] Bici [3] Maglietta Fuocoinferno [4] Bussola [5] Schitarrata Metallica\n");
    do
    {
        printf("Scelta: ");
        if (scanf("%d", &sc_ogg) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            sc_ogg = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (sc_ogg < 1 || sc_ogg > 5);
    nuova_zona_mondoreale->oggetto = (enum Tipo_oggetto)(sc_ogg - 1);

    // --- NEMICO SOPRASOTTO ---
    int demotorzone_presente = check_demotorzone();
    int scelta_nemico_s;

    printf("\nNemico Soprasotto:\n[1] Nessuno\n[2] Democane\n");
    if (!demotorzone_presente)
        printf("[3] %sDEMOTORZONE%s\n", MAGENTA, RESET);
    else
        printf("[3] (Demotorzone gia' presente)\n");

    do
    {
        printf("Scelta: ");
        if (scanf("%d", &scelta_nemico_s) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            scelta_nemico_s = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
        // Controllo range e blocco boss se già presente
        if (scelta_nemico_s < 1 || scelta_nemico_s > 3)
            scelta_nemico_s = 0;
        if (scelta_nemico_s == 3 && demotorzone_presente)
        {
            printf("Errore: Demotorzone gia' presente!\n");
            scelta_nemico_s = 0;
        }
    } while (scelta_nemico_s == 0);
    if (scelta_nemico_s == 1)
        scelta_nemico_s = 0;
    nuova_zona_soprasotto->nemico = (enum Tipo_nemico)(scelta_nemico_s);

    // --- INSERIMENTO LISTA ---

    if (pos_zona == 0) // TESTA
    {
        nuova_zona_mondoreale->avanti = prima_zona_mondoreale;
        nuova_zona_soprasotto->avanti = prima_zona_soprasotto;

        if (prima_zona_mondoreale != NULL)
        {
            prima_zona_mondoreale->indietro = nuova_zona_mondoreale;
            prima_zona_soprasotto->indietro = nuova_zona_soprasotto;
        }
        prima_zona_mondoreale = nuova_zona_mondoreale;
        prima_zona_soprasotto = nuova_zona_soprasotto;
    }
    else // MEZZO O CODA
    {
        struct Zona_mondoreale *prec_m = prima_zona_mondoreale;
        struct Zona_soprasotto *prec_s = prima_zona_soprasotto;
        int i = 0;

        while (i < pos_zona - 1 && prec_m != NULL)
        {
            prec_m = prec_m->avanti;
            prec_s = prec_s->avanti;
            i++;
        }

        if (prec_m == NULL)
        {
            printf("Errore critico inserimento.\n");
            return;
        }

        struct Zona_mondoreale *succ_m = prec_m->avanti;
        struct Zona_soprasotto *succ_s = prec_s->avanti;

        // Link prec -> nuovo
        prec_m->avanti = nuova_zona_mondoreale;
        prec_s->avanti = nuova_zona_soprasotto;

        // Link nuovo -> prec
        nuova_zona_mondoreale->indietro = prec_m;
        nuova_zona_soprasotto->indietro = prec_s;

        // Link nuovo -> succ
        nuova_zona_mondoreale->avanti = succ_m;
        nuova_zona_soprasotto->avanti = succ_s;

        // Link succ -> nuovo (se esiste)
        if (succ_m != NULL)
        {
            succ_m->indietro = nuova_zona_mondoreale;
            succ_s->indietro = nuova_zona_soprasotto;
        }
    }

    printf("\n%sZona inserita con successo!%s\n", GREEN, RESET);
    premi_invio();
}

static void cancella_zona()
{
    pulisci_schermo();
    if (prima_zona_mondoreale == NULL)
    {
        printf("La mappa e' vuota.\n");
        premi_invio();
        return;
    }

    printf("=== CANCELLA ZONA ===\n");
    int scelta_zona;
    int max = get_numero_zone() - 1;

    do
    {
        printf("Inserisci la posizione (0-%d): ", max);
        if (scanf("%d", &scelta_zona) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            scelta_zona = -1;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (scelta_zona < 0 || scelta_zona > max);

    int i = 0;
    struct Zona_mondoreale *zona_temp_m = prima_zona_mondoreale;

    while (i < scelta_zona && zona_temp_m != NULL)
    {
        zona_temp_m = zona_temp_m->avanti;
        i++;
    }

    if (zona_temp_m == NULL)
        return;

    struct Zona_soprasotto *zona_temp_s = zona_temp_m->link_soprasotto;

    // Ricollegamento puntatori
    if (zona_temp_m->indietro != NULL)
    {
        zona_temp_m->indietro->avanti = zona_temp_m->avanti;
        zona_temp_s->indietro->avanti = zona_temp_s->avanti;
    }
    else
    {
        prima_zona_mondoreale = zona_temp_m->avanti;
        prima_zona_soprasotto = zona_temp_s->avanti;
    }

    if (zona_temp_m->avanti != NULL)
    {
        zona_temp_m->avanti->indietro = zona_temp_m->indietro;
        zona_temp_s->avanti->indietro = zona_temp_s->indietro;
    }

    free(zona_temp_m);
    free(zona_temp_s);

    mappa_completa = 0;
    printf("\n%sZona eliminata con successo.%s\n", GREEN, RESET);
    premi_invio();
}

static void stampa_mappa()
{
    pulisci_schermo();
    if (prima_zona_mondoreale == NULL)
    {
        printf("Mappa vuota.\n");
        premi_invio();
        return;
    }

    int scelta;
    do
    {
        printf("Quale mappa stampare?\n1. Mondo Reale\n2. Soprasotto\nScelta: ");
        if (scanf("%d", &scelta) != 1)
        {
            printf("Errore: Inserire un numero che sia 1 o 2\n");
            while (getchar() != '\n')
                ;
            scelta = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (scelta < 1 || scelta > 2);

    printf("\n%s--- VISUALIZZAZIONE MAPPA ---\n%s", BOLD, RESET);

    int i = 0;
    if (scelta == 1) // MONDO REALE
    {
        struct Zona_mondoreale *zona_temp = prima_zona_mondoreale;
        while (zona_temp != NULL)
        {
            printf("[%02d] %-15s | Nemico: %-10s | Oggetto: %s\n",
                   i, get_nome_zona(zona_temp->tipo), get_nome_nemico(zona_temp->nemico), get_nome_oggetto(zona_temp->oggetto));
            zona_temp = zona_temp->avanti;
            i++;
        }
    }
    else // SOPRASOTTO
    {
        struct Zona_soprasotto *zona_temp = prima_zona_soprasotto;
        while (zona_temp != NULL)
        {
            printf("[%02d] %-15s | Nemico: %-10s\n",
                   i, get_nome_zona(zona_temp->tipo), get_nome_nemico(zona_temp->nemico));
            zona_temp = zona_temp->avanti;
            i++;
        }
    }
    printf("------------------------------\n");
    premi_invio();
}

static void stampa_zona()
{
    pulisci_schermo();
    if (prima_zona_mondoreale == NULL)
    {
        printf("Mappa vuota.\n");
        premi_invio();
        return;
    }

    int scelta_sz;
    int max = get_numero_zone() - 1;
    do
    {
        printf("Inserisci la posizione (0-%d): ", max);
        if (scanf("%d", &scelta_sz) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            scelta_sz = -1;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (scelta_sz < 0 || scelta_sz > max);

    int i = 0;
    struct Zona_mondoreale *zona_temp_m = prima_zona_mondoreale;
    while (i < scelta_sz && zona_temp_m != NULL)
    {
        zona_temp_m = zona_temp_m->avanti;
        i++;
    }

    if (zona_temp_m == NULL)
        return;

    struct Zona_soprasotto *zona_temp_s = zona_temp_m->link_soprasotto;

    printf("\n%s=== DETTAGLI ZONA %d ===%s\n", BLUE, i, RESET);
    printf("--------------------------------\n");
    printf("%sMONDO REALE%s\n", BOLD, RESET);
    printf("  Luogo:   %s\n", get_nome_zona(zona_temp_m->tipo));
    printf("  Nemico:  %s\n", get_nome_nemico(zona_temp_m->nemico));
    printf("  Oggetto: %s\n", get_nome_oggetto(zona_temp_m->oggetto));
    printf("--------------------------------\n");
    printf("%sSOPRASOTTO%s\n", BOLD, RESET);
    printf("  Luogo:   %s\n", get_nome_zona(zona_temp_s->tipo));
    printf("  Nemico:  %s\n", get_nome_nemico(zona_temp_s->nemico));
    printf("--------------------------------\n");

    premi_invio();
}

static void chiudi_mappa()
{
    pulisci_schermo();
    int num_zone = get_numero_zone();
    int demo_ok = check_demotorzone();

    if (num_zone >= 15 && demo_ok)
    {
        mappa_completa = 1;
        printf("%sMappa chiusa con successo!%s\n", GREEN, RESET);
        printf("Si può procedere al gioco.\n");
        premi_invio();
        scelta_mappa = 6;
    }
    else
    {
        printf("%sERRORE VALIDAZIONE:%s\n", RED, RESET);
        printf("- Numero Zone: %d (Richiesto >= 15) -> %s\n", num_zone, num_zone >= 15 ? "OK" : "NO");
        printf("- Demotorzone: %d (Richiesto == 1)  -> %s\n", demo_ok, demo_ok ? "OK" : "NO");
        mappa_completa = 0;
        scelta_mappa = 0;
        premi_invio();
    }
}

static void menu_mappa()
{
    scelta_mappa = 0;
    do
    {
        pulisci_schermo();
        printf("%s=== CREAZIONE MAPPA ===%s\n", BLUE, RESET);
        printf("Zone attuali: %d\n", get_numero_zone());
        printf("Demotorzone presente: %s\n", check_demotorzone() ? "SI" : "NO");
        printf("----------------------------------\n");
        printf("1. Genera mappa casuale\n");
        printf("2. Inserisci Zona manuale\n");
        printf("3. Cancella Zona\n");
        printf("4. Stampa mappa completa\n");
        printf("5. Stampa dettaglio zona\n");
        printf("6. Chiudi mappa e torna al menu\n");
        printf("----------------------------------\n");
        printf("Scelta: ");

        if (scanf("%d", &scelta_mappa) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            scelta_mappa = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }

        switch (scelta_mappa)
        {
        case 1:
            genera_mappa();
            break;
        case 2:
            inserisci_zona();
            break;
        case 3:
            cancella_zona();
            break;
        case 4:
            stampa_mappa();
            break;
        case 5:
            stampa_zona();
            break;
        case 6:
            chiudi_mappa();
            break;
        default:
            break;
        }

    } while (scelta_mappa != 6);
}

void imposta_gioco()
{
    if (prima_zona_mondoreale != NULL)
    {
        elimina_mappa();
        elimina_giocatori();
    }

    int num_giocatori = 0;
    pulisci_schermo();

    printf("=== IMPOSTAZIONE GIOCATORI ===\n");
    printf("Inserisci il numero dei giocatori (1-4)\n");
    do
    {
        printf("Scelta: ");
        if (scanf("%d", &num_giocatori) != 1)
        {
            printf("Errore: Inserire solo numeri!\n");
            while (getchar() != '\n')
                ;
            num_giocatori = 0;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (num_giocatori < 1 || num_giocatori > 4);

    undici_scelto = 0;

    for (int i = 0; i < num_giocatori; i++)
    {
        pulisci_schermo();
        printf("--- Configurazione Giocatore %d/%d ---\n", i + 1, num_giocatori);

        giocatori[i] = (struct Giocatore *)malloc(sizeof(struct Giocatore));

        printf("Nome: ");
        scanf("%31s", giocatori[i]->nome);

        // Stats random
        giocatori[i]->attacco_psichico = (rand() % 13) + 8;
        giocatori[i]->difesa_psichica = (rand() % 13) + 8;
        giocatori[i]->fortuna = (rand() % 16) + 5;

        // Stats fisse
        giocatori[i]->hp = 50;

        printf("\nStatistiche Iniziali:\n");
        printf("Attacco Psichico: %d\n", giocatori[i]->attacco_psichico);
        printf("Difesa Psichica:  %d\n", giocatori[i]->difesa_psichica);
        printf("Fortuna:          %d\n", giocatori[i]->fortuna);
        printf("Punti vita (HP):  %d\n", giocatori[i]->hp);

        int scelta;
        int scelta_valida = 0;
        do
        {
            printf("\nOpzioni Modifica:\n");
            printf("[1] Attacco +3, Difesa -3\n");
            printf("[2] Attacco -3, Difesa +3\n");
            printf("[3] Nessuna modifica\n");
            if (!undici_scelto)
                printf("%s[4] Diventa UndiciVirgolaCinque (+4/+4/-7)%s\n", BOLD, RESET);

            printf("Scelta: ");
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

            if (scelta == 4 && undici_scelto)
                scelta = -1;

            switch (scelta)
            {
            case 1:
                giocatori[i]->attacco_psichico += 3;
                if (giocatori[i]->attacco_psichico > 20)
                    giocatori[i]->attacco_psichico = 20;

                giocatori[i]->difesa_psichica -= 3;

                scelta_valida = 1;
                break;
            case 2:
                giocatori[i]->difesa_psichica += 3;
                if (giocatori[i]->difesa_psichica > 20)
                    giocatori[i]->difesa_psichica = 20;
                giocatori[i]->attacco_psichico -= 3;

                scelta_valida = 1;
                break;
            case 3:
                scelta_valida = 1;
                break;
            case 4:
                undici_scelto = 1;
                strcpy(giocatori[i]->nome, "UndiciVirgolaCinque");
                giocatori[i]->attacco_psichico += 4;
                giocatori[i]->difesa_psichica += 4;
                giocatori[i]->fortuna -= 7;

                if (giocatori[i]->attacco_psichico > 20)
                    giocatori[i]->attacco_psichico = 20;
                if (giocatori[i]->difesa_psichica > 20)
                    giocatori[i]->difesa_psichica = 20;
                if (giocatori[i]->fortuna < 1)
                    giocatori[i]->fortuna = 1;
                scelta_valida = 1;
                break;
            default:
                printf("Scelta non valida.\n");
            }
        } while (!scelta_valida);

        printf("\n%sStatistiche Finali:%s\n", BOLD, RESET);
        printf("%sAttacco: %d | Difesa: %d | Fortuna: %d | Punti vita (HP): %d%s\n", GREEN,
               giocatori[i]->attacco_psichico, giocatori[i]->difesa_psichica, giocatori[i]->fortuna, giocatori[i]->hp, RESET);

        // Inizializza zaino
        for (int j = 0; j < 3; j++)
            giocatori[i]->zaino[j] = nessun_oggetto;

        giocatori[i]->mondo = 0;
        giocatori[i]->nemico_gia_sconfitto = 0;

        premi_invio();
    }

    menu_mappa();
}

static void avanza(struct Giocatore *g, int *mosso)
{
    int nemico_presente = 0;

    if (*mosso)
    {
        printf("Ti sei già mosso in questo turno!\n");
        premi_invio();
        return;
    }

    if (g->mondo == 0)
    {
        if (g->pos_mondoreale->nemico != nessun_nemico)
        {
            nemico_presente = 1;
        }
    }
    else
    {
        if (g->pos_soprasotto->nemico != nessun_nemico)
        {
            nemico_presente = 1;
        }
    }

    if (nemico_presente && !g->nemico_gia_sconfitto)
    {
        printf("Per avanzare devi prima sconfiggere il nemico!");
        premi_invio();
        return;
    }
    else
    {
        if (g->pos_mondoreale->avanti == NULL)
        {
            printf("Sei all'ultima zona. Non puoi avanzare!");
            premi_invio();
            return;
        }
        else
        {
            *mosso = 1;
            g->pos_mondoreale = g->pos_mondoreale->avanti;
            g->pos_soprasotto = g->pos_soprasotto->avanti;
            g->nemico_gia_sconfitto = 0;
            printf("Sei avanzato nella zona successiva.\n");
        }
    }

    premi_invio();
}

static void indietreggia(struct Giocatore *g, int *mosso)
{
    int nemico_presente = 0;

    if (*mosso)
    {
        printf("Ti sei già mosso in questo turno!\n");
        premi_invio();
        return;
    }

    if (g->mondo == 0)
    {
        if (g->pos_mondoreale->nemico != nessun_nemico)
        {
            nemico_presente = 1;
        }
    }
    else
    {
        if (g->pos_soprasotto->nemico != nessun_nemico)
        {
            nemico_presente = 1;
        }
    }

    if (nemico_presente && !g->nemico_gia_sconfitto)
    {
        printf("Per indietreggiare devi prima sconfiggere il nemico!");
        premi_invio();
        return;
    }
    else
    {
        if (g->pos_mondoreale->indietro == NULL)
        {
            printf("Sei alla prima zona. Non puoi tornare indietro!\n");
            premi_invio();
            return;
        }
        else
        {
            *mosso = 1;
            g->pos_mondoreale = g->pos_mondoreale->indietro;
            g->pos_soprasotto = g->pos_soprasotto->indietro;
            g->nemico_gia_sconfitto = 0;
            printf("Sei indietreggiato nella zona precedente.\n");
        }
    }

    premi_invio();
}

static void cambia_mondo(struct Giocatore *g, int *mosso)
{
    if (*mosso == 1)
    {
        printf("Ti sei già mosso in questo turno! Non puoi cambiare mondo!\n");
        premi_invio();
        return;
    }

    if (g->mondo == 0)
    {
        if (g->pos_mondoreale->nemico != nessun_nemico && !g->nemico_gia_sconfitto)
        {
            printf("Non puoi cambiare mondo! C'è un nemico!\n");
            premi_invio();
            return;
        }
        else
        {
            g->mondo = 1;
            *mosso = 1;
            g->nemico_gia_sconfitto = 0;
            printf("Sei nel Soprasotto!\n");
        }
    }
    else
    {
        int dado = (rand() % 20) + 1;
        printf("Tiro Fortuna: %d (La tua Fortuna: %d)\n", dado, g->fortuna);

        if (dado < g->fortuna)
        {
            printf("%sSuccesso! Vai nel Mondoreale!%s\n", GREEN, RESET);
            g->mondo = 0;
            *mosso = 1;
        }
        else
        {
            printf("%sFallimento!%s Rimani nel Soprasotto\n", RED, RESET);
            *mosso = 1;
        }
    }

    premi_invio();
}

static int utilizza_oggetto(struct Giocatore *g, int in_combattimento)
{
    pulisci_schermo();

    printf("=== ZAINO DI %s ===\n", g->nome);

    int num_oggetti = 0;
    for (int i = 0; i < 3; i++)
    {
        if (g->zaino[i] != nessun_oggetto)
        {
            int utilizzabile = 1;
            if (in_combattimento)
            {
                if (g->zaino[i] == bicicletta || g->zaino[i] == bussola)
                {
                    utilizzabile = 0;
                }
            }
            if (utilizzabile)
                printf("[%d] %s\n", i + 1, get_nome_oggetto(g->zaino[i]));
            else
                printf("[%d] %s (Inutilizzabile in scontro)\n", i + 1, get_nome_oggetto(g->zaino[i]));
            num_oggetti++;
        }
        else
        {
            printf("[%d] (Vuoto)\n", i + 1);
        }
    }

    if (num_oggetti == 0)
    {
        printf("Non hai oggetti nello zaino.\n");
        premi_invio();
        return 0;
    }

    int slot;
    printf("Scegli lo slot che desideri (1-3) (0 per annullare)\n");
    do
    {
        printf("Scelta: ");
        if (scanf("%d", &slot) != 1)
        {
            while (getchar() != '\n')
                ;
        }
        else
        {
            while (getchar() != '\n')
                ;
        }
    } while (slot < 0 || slot > 3);

    if (slot == 0)
        return 0;

    if (g->zaino[slot - 1] == nessun_oggetto)
    {
        printf("Slot vuoto");
        premi_invio();
        return 0;
    }

    enum Tipo_oggetto ogg = g->zaino[slot - 1];

    if (in_combattimento && (ogg == bicicletta || ogg == bussola))
    {
        printf("Non puoi usare questo oggetto mentre combatti! Devi prima finire lo scontro.\n");
        premi_invio();
        return 0;
    }

    int azione_eseguita = 0; // 0=No, 1=Consumato, 2=Movimento(Bici)

    switch (ogg)
    {
    case maglietta_fuocoinferno:
        printf("Indossi la maglietta fuocoinferno!\n");
        printf("Effetti attivi per questo scontro:\n");
        printf("- DIFESA PSICHICA +10\n");
        printf("- SPINE DI FUOCO (3 Danni al nemico)\n");
        buff_maglietta_attivo = 1;
        azione_eseguita = 1;
        break;

    case schitarrata_metallica:
        printf("Suoni la chitarra metallica!\n");
        printf("Effetto attivo: NEMICO STORDITO (Danni dimezzati)\n");
        buff_schitarrata_attivo = 1;
        azione_eseguita = 1;
        break;

    case bicicletta:
        printf("Stai usando la bicicletta!\n");
        // Salto 1
        if (g->pos_mondoreale->avanti != NULL)
        {
            g->pos_mondoreale = g->pos_mondoreale->avanti;
            g->pos_soprasotto = g->pos_soprasotto->avanti;
        }
        // Salto 2 (se possibile)
        if (g->pos_mondoreale->avanti != NULL)
        {
            g->pos_mondoreale = g->pos_mondoreale->avanti;
            g->pos_soprasotto = g->pos_soprasotto->avanti;
            printf("Hai saltato due zone!\n");
        }
        else
        {
            printf("Sei arrivato all'ultima zona.\n");
        }

        printf("Perdi 5 HP per la fatica...\n");
        g->hp -= 5;
        if (g->hp <= 0)
            printf("Sei morto per la fatica sulla bici!\n");

        azione_eseguita = 2;
        break;

    case bussola:

        int sc_bus;
        printf("\n=== BUSSOLA ===\n");
        printf("1. Vedi prossima zona\n");
        printf("2. Cerca un oggetto specifico\n");

        do
        {
            printf("Scelta: ");
            if (scanf("%d", &sc_bus) != 1)
            {
                while (getchar() != '\n')
                    ;
                sc_bus = 0;
            }
            else
            {
                while (getchar() != '\n')
                    ;
            }
        } while (sc_bus < 1 || sc_bus > 2);

        if (sc_bus == 1)
        {
            if (g->pos_mondoreale->avanti != NULL)
            {
                struct Zona_mondoreale *prossima_mondo = g->pos_mondoreale->avanti;
                struct Zona_soprasotto *prossima_sopra = g->pos_soprasotto->avanti;
                printf("\nProssima Zona (Reale): %s | Nemico: %s | Oggetto: %s\n",
                       get_nome_zona(prossima_mondo->tipo), get_nome_nemico(prossima_mondo->nemico), get_nome_oggetto(prossima_mondo->oggetto));
                printf("Prossima Zona (Sotto): %s | Nemico: %s\n",
                       get_nome_zona(prossima_sopra->tipo), get_nome_nemico(prossima_sopra->nemico));
            }
            else
            {
                printf("Non c'è niente dopo.\n");
            }
            azione_eseguita = 1;
        }
        else if (sc_bus == 2)
        {
            int sc_ogg;
            printf("Cerca oggetto: 1.Bici 2.Maglietta Fuocoinferno 3.Bussola 4.Schitarrata Metallica\n");
            do
            {
                printf("Scelta: ");

                if (scanf("%d", &sc_ogg) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    sc_ogg = 0;
                }
                else
                {
                    while (getchar() != '\n')
                        ;
                }
            } while (sc_ogg < 1 || sc_ogg > 4);

            enum Tipo_oggetto trov_ogg = sc_ogg;

            struct Zona_mondoreale *zona_temp = g->pos_mondoreale->avanti;
            int distanza = 1;
            int trovato = 0;

            while (zona_temp != NULL)
            {
                if (zona_temp->oggetto == trov_ogg)
                {
                    printf("Rilevato! L'oggetto si trova tra %d zone.\n", distanza);
                    trovato = 1;
                    break;
                }
                zona_temp = zona_temp->avanti;
                distanza++;
            }

            if (!trovato)
                printf("L'oggetto non è stato rilevato davanti a te.\n");
            azione_eseguita = 1;
        }
        break;
    default:
        break;
    }

    if (azione_eseguita > 0)
    {
        g->zaino[slot - 1] = nessun_oggetto;
        printf("\n(Oggetto rimosso dallo zaino)\n");
        premi_invio();
        return azione_eseguita;
    }

    premi_invio();
    return 0;
}

static void combatti(struct Giocatore *g)
{
    pulisci_schermo();

    int tipo_nemico;
    if (g->mondo == 0)
        tipo_nemico = g->pos_mondoreale->nemico;
    else
        tipo_nemico = g->pos_soprasotto->nemico;

    if (tipo_nemico == nessun_nemico)
    {
        printf("Non c'è nessun nemico!");
        premi_invio();
        return;
    }

    int hp_nemico_max;
    int hp_nemico_attuali;
    int atk_nemico;

    if (tipo_nemico == democane)
    {
        hp_nemico_max = 40;
        atk_nemico = 14;
    }
    else if (tipo_nemico == billi)
    {
        hp_nemico_max = 60;
        atk_nemico = 18;
    }
    else
    {
        hp_nemico_max = 90;
        atk_nemico = 20;
    }
    hp_nemico_attuali = hp_nemico_max;

    printf("=== INIZIO COMBATTIMENTO ===\n");
    printf("Tu: %s (HP: %d)\n", g->nome, g->hp);
    printf("Nemico: %s (HP: %d)\n", get_nome_nemico(tipo_nemico), hp_nemico_attuali);
    premi_invio();

    buff_maglietta_attivo = 0;
    buff_schitarrata_attivo = 0;
    oggetto_usato_in_questo_turno = 0;

    while (g->hp > 0 && hp_nemico_attuali > 0)
    {
        int turno_completato = 0;
        while (!turno_completato)
        {
            pulisci_schermo();
            printf("--- %s VS %s ---\n", g->nome, get_nome_nemico(tipo_nemico));
            printf("Tuoi HP: %d  |  HP Nemico: %d\n", g->hp, hp_nemico_attuali);

            printf("Stato: ");
            if (buff_maglietta_attivo)
                printf("[MAGLIETTA ATTIVA] ");
            if (buff_schitarrata_attivo)
                printf("[SCHITARRATA ATTIVA]");
            if (!buff_maglietta_attivo && !buff_schitarrata_attivo)
                printf("Normale");
            printf("\n--------------------------------\n");

            printf("1. Attacca\n");
            if (oggetto_usato_in_questo_turno)
                printf("2. [OGGETTO GIA USATO]\n");
            else
                printf("2. Usa Oggetto\n");

            printf("Scelta: ");
            int sc;
            if (scanf("%d", &sc) != 1)
            {
                while (getchar() != '\n')
                    ;
                sc = 0;
            }
            else
            {
                while (getchar() != '\n')
                    ;
            }

            if (sc == 1)
            {
                int dado_atk = (rand() % 6) + 1;
                int danno_fatto = g->attacco_psichico + dado_atk;

                int dado_fort = rand() % 100;
                if (dado_fort < g->fortuna)
                {
                    danno_fatto *= 2;
                    printf("SEI FORTUNATO! Hai inflitto danno doppio!\n");
                }

                hp_nemico_attuali -= danno_fatto;
                printf("Hai inflitto %d di danno al nemico!\n", danno_fatto);
                turno_completato = 1;
                premi_invio();
            }
            else if (sc == 2)
            {
                if (oggetto_usato_in_questo_turno)
                {
                    printf("Hai gia' usato un oggetto in questo turno!\n");
                    premi_invio();
                }
                else
                {
                    if (utilizza_oggetto(g, 1) == 1)
                    {
                        oggetto_usato_in_questo_turno = 1;
                    }
                }
            }
        }

        if (hp_nemico_attuali <= 0)
            break;

        printf("=== TURNO NEMICO ===\n");

        if (buff_maglietta_attivo)
        {
            printf("Il nemico tocca la tua Maglietta Infuocata! (3 Danni)\n");
            hp_nemico_attuali -= 3;
            if (hp_nemico_attuali <= 0)
            {
                printf("Il nemico muore bruciato!\n");
                break;
            }
        }

        int dado_atk_nemico = (rand() % 6) + 1;
        int danno_nemico = atk_nemico + dado_atk_nemico;

        if (buff_schitarrata_attivo)
        {
            printf("Nemico stordito! Danno dimezzato.\n");
            danno_nemico /= 2;
        }

        int difesa_totale = g->difesa_psichica;
        if (buff_maglietta_attivo)
        {
            difesa_totale += 10;
            printf("(Maglietta: Difesa %d -> %d)\n", g->difesa_psichica, difesa_totale);
        }

        int danno_assorbito = difesa_totale / 2;
        int danno_subito = danno_nemico - danno_assorbito;
        if (danno_subito < 1)
            danno_subito = 1;

        g->hp -= danno_subito;
        printf("Il nemico attacca (Forza %d)! Subisci %d danni.\n", danno_nemico, danno_subito);

        oggetto_usato_in_questo_turno = 0;
        buff_maglietta_attivo = 0;
        buff_schitarrata_attivo = 0;

        premi_invio();
    }

    // Reset finale
    buff_maglietta_attivo = 0;
    buff_schitarrata_attivo = 0;
    oggetto_usato_in_questo_turno = 0;

    pulisci_schermo();
    if (g->hp <= 0)
    {
        printf("\n%s>>> SEI STATO SCONFITTO! <<<%s\n", RED, RESET);
    }
    else
    {
        printf("\n%s>>> NEMICO SCONFITTO! <<<%s\n", GREEN, RESET);
        g->nemico_gia_sconfitto = 1;
        // CASO BOSS FINALE
        if (tipo_nemico == demotorzone)
        {
            printf("\n%s==========================================%s\n", MAGENTA, RESET);
            printf(" %sHAI UCCISO IL DEMOTORZONE!%s\n", GREEN, RESET);
            printf("%s==========================================%s\n", MAGENTA, RESET);

            salva_vincitore(g->nome);
            premi_invio();
            gioco_in_corso = 0;

            return;
        }

        int prob = rand() % 100;
        if (prob < 50)
        {
            printf("Il nemico è scomparso. La zona è sicura\n");
            if (g->mondo == 0)
                g->pos_mondoreale->nemico = nessun_nemico;
            else
                g->pos_soprasotto->nemico = nessun_nemico;
        }
        else
        {
            printf("Il nemico è sconfitto, ma il corpo rimane li'\n");
        }
    }
    premi_invio();
}

static void raccogli_oggetto(struct Giocatore *g)
{
    if (g->pos_mondoreale->nemico != nessun_nemico || g->nemico_gia_sconfitto)
    {
        printf("Per raccogliere l'oggetto devi prima sconfiggere il nemico!\n");
        premi_invio();
        return;
    }

    if (g->pos_mondoreale->oggetto == nessun_oggetto)
    {
        printf("Nessun oggetto presente!\n");
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            if (g->zaino[i] == nessun_oggetto)
            {
                g->zaino[i] = g->pos_mondoreale->oggetto;
                printf("Hai raccolto %s", get_nome_oggetto(g->pos_mondoreale->oggetto));
                g->pos_mondoreale->oggetto = nessun_oggetto;
                premi_invio();
                return;
            }
        }
        printf(RED "Lo zaino è pieno!\n" RESET);
    }

    premi_invio();
}

static void stampa_giocatore(struct Giocatore *g)
{
    printf("\n=== INFO %s%s%s ===\n", CYAN, g->nome, RESET);
    printf("\nStatistiche: HP %d | Atk %d | Dif %d | Fortuna %d\n", g->hp, g->attacco_psichico, g->difesa_psichica, g->fortuna);
    printf("Zaino: [%s] [%s] [%s]\n", get_nome_oggetto(g->zaino[0]), get_nome_oggetto(g->zaino[1]), get_nome_oggetto(g->zaino[2]));
    premi_invio();
}

static void turno_giocatore(struct Giocatore *g)
{
    int turno_in_corso = 1;
    int scelta;
    int ha_mosso = 0;

    do
    {
        pulisci_schermo();
        printf("=== TOCCA A: %s%s%s (HP: %d) ===\n", CYAN, g->nome, RESET, g->hp);

        // --- GESTIONE STATUS ZONA ---
        if (g->mondo == 0) // MONDO REALE
        {
            printf("Posizione: Mondo Reale - %s\n", get_nome_zona(g->pos_mondoreale->tipo));

            if (g->pos_mondoreale->nemico != nessun_nemico)
            {
                if (g->nemico_gia_sconfitto)
                {
                    printf("%s[SICURO] Il corpo del nemico (%s) e' a terra. Puoi passare.%s\n",
                           GREEN, get_nome_nemico(g->pos_mondoreale->nemico), RESET);
                }
                else
                {
                    printf("%sATTENZIONE: Nemico presente! (%s) %s\n",
                           RED, get_nome_nemico(g->pos_mondoreale->nemico), RESET);
                }
            }
            printf("Oggetto: %s\n", get_nome_oggetto(g->pos_mondoreale->oggetto));
        }
        else // SOPRASOTTO
        {
            printf("Posizione: Soprasotto - %s\n", get_nome_zona(g->pos_soprasotto->tipo));

            if (g->pos_soprasotto->nemico != nessun_nemico)
            {
                if (g->nemico_gia_sconfitto)
                {
                    printf("%s[SICURO] Il corpo del nemico (%s) e' a terra. Puoi passare.%s\n",
                           GREEN, get_nome_nemico(g->pos_soprasotto->nemico), RESET);
                }
                else
                {
                    printf("%sATTENZIONE: Nemico presente! (%s) %s\n",
                           RED, get_nome_nemico(g->pos_soprasotto->nemico), RESET);
                }
            }
        }

        printf("\n--- AZIONI ---\n");
        printf("1. Avanza\n");
        printf("2. Indietreggia\n");
        printf("3. Cambia Mondo\n");
        printf("4. Combatti\n");
        printf("5. Raccogli Oggetto\n");
        printf("6. Utilizza Oggetto\n");
        printf("7. Info Giocatore\n");
        printf("8. Passa il turno e recupera HP\n");
        printf("%s0. Abbandona partita (Torna al Menu Principale)%s\n", RED, RESET);

        do
        {
            printf("Scelta: ");
            if (scanf("%d", &scelta) != 1)
            {
                printf("Errore: Inserire solo numeri!\n");
                while (getchar() != '\n')
                    ;
                scelta = -1;
            }
            else
            {
                while (getchar() != '\n')
                    ;
            }
        } while (scelta < 0 || scelta > 8);

        switch (scelta)
        {
        case 1:
            avanza(g, &ha_mosso);
            break;
        case 2:
            indietreggia(g, &ha_mosso);
            break;
        case 3:
            cambia_mondo(g, &ha_mosso);
            break;
        case 4:
            combatti(g);
            break;
        case 5:
            raccogli_oggetto(g);
            break;
        case 6:
            int esito = utilizza_oggetto(g, 0);
            if (esito == 2)
            {
                printf("\n--------------------------------------------------\n");
                printf("          TURNO FINITO (Fatica da Bici)\n");
                printf("--------------------------------------------------\n");
                premi_invio();
                turno_in_corso = 0;
            }
            break;
        case 7:
            stampa_giocatore(g);
            break;
        case 8:
            int recupero = 15; // Recupera 15 HP riposando
            int hp_pre = g->hp;

            g->hp += recupero;
            if (g->hp > 50)
                g->hp = 50;

            printf("%sHai recuperato %d HP! (Ora ne hai %d)%s\n", GREEN, g->hp - hp_pre, g->hp, RESET);
            printf("--------------------------------------------------\n");
            printf("             TURNO FINITO\n");
            printf("--------------------------------------------------\n");
            premi_invio();
            turno_in_corso = 0;
            break;

        case 0:
            char conferma;
            printf("\n%sAttenzione!%s Se esci ora, perderai i progressi attuali.\n", B_RED, RESET);
            printf("Sei sicuro di voler tornare al menu principale? (s/n): ");
            scanf(" %c", &conferma);
            while (getchar() != '\n')
                ;

            if (conferma == 's' || conferma == 'S')
            {
                turno_in_corso = 0;
                gioco_in_corso = 0;
                return;
            }
            else
            {
                printf("Scelta annullata\n");
            }
            break;
        }

        if (g->hp <= 0)
        {
            printf("\n\n%s>>> SEI MORTO! <<<%s\n", RED, RESET);
            turno_in_corso = 0;
            premi_invio();
        }

    } while (turno_in_corso && gioco_in_corso);
}

void gioca()
{
    pulisci_schermo();
    if (!mappa_completa || giocatori[0] == NULL)
    {
        printf("%sATTENZIONE: Devi prima impostare correttamente la mappa!%s\n", RED, RESET);
        premi_invio();
        return;
    }

    // Reset Iniziale Giocatori
    for (int i = 0; i < 4; i++)
    {
        if (giocatori[i] != NULL)
        {
            giocatori[i]->pos_mondoreale = prima_zona_mondoreale;
            giocatori[i]->pos_soprasotto = prima_zona_soprasotto;
            giocatori[i]->mondo = 0;
            giocatori[i]->hp = 50;
            giocatori[i]->nemico_gia_sconfitto = 0;
        }
    }

    struct Giocatore *ordine_giocatori[4];
    gioco_in_corso = 1;
    int numero_round = 1;

    while (gioco_in_corso)
    {
        int vivi = 0;
        for (int i = 0; i < 4; i++)
        {
            if (giocatori[i] != NULL && giocatori[i]->hp > 0)
            {
                ordine_giocatori[vivi] = giocatori[i];
                vivi++;
            }
        }

        if (vivi == 0)
        {
            printf("\n%s>>> TUTTI I GIOCATORI SONO MORTI. GAME OVER <<<%s\n", RED, RESET);
            gioco_in_corso = 0;
            break;
        }

        // Mescola ordine giocatori
        for (int i = vivi - 1; i > 0; i--)
        {
            int j = rand() % (i + 1);
            struct Giocatore *temp = ordine_giocatori[i];
            ordine_giocatori[i] = ordine_giocatori[j];
            ordine_giocatori[j] = temp;
        }

        // --- STAMPA INIZIO ROUND ---
        pulisci_schermo();
        printf("======================================\n");
        printf("      INIZIO ROUND %d\n", numero_round);
        printf("======================================\n");
        printf("Ordine di gioco estratto:\n");
        for (int k = 0; k < vivi; k++)
        {
            printf("%d. %s\n", k + 1, ordine_giocatori[k]->nome);
        }
        premi_invio();

        // --- CICLO TURNI ---
        for (int i = 0; i < vivi; i++)
        {
            struct Giocatore *gioc_attuale = ordine_giocatori[i];

            if (gioc_attuale->hp > 0)
            {
                pulisci_schermo();
                printf("Sta per toccare a: %s%s%s\n", CYAN, gioc_attuale->nome, RESET);
                premi_invio();

                turno_giocatore(gioc_attuale);
            }

            if (!gioco_in_corso)
            {
                elimina_mappa();
                elimina_giocatori();
                break;
            }
        }

        numero_round++;
    }

    premi_invio();
}

void termina_gioco()
{
    printf("\nPulizia memoria e uscita... Arrivederci!\n");
    elimina_mappa();
    elimina_giocatori();
}

void crediti()
{
    pulisci_schermo();
    printf("\n%s=========================================%s\n", CYAN, RESET);
    printf("%s                 CREDITI           %s\n", CYAN, RESET);
    printf("%s=========================================%s\n", CYAN, RESET);

    printf("Sviluppato da: %sLorenzo Mazzoli%s\n", BOLD, RESET);
    printf("Corso: Programmazione Procedurale 2025/26\n");

    printf("\nULTIMI 3 VINCITORI:\n");

    FILE *f = fopen("vincitori.txt", "r");
    if (f == NULL)
    {
        printf("(Nessun vincitore ancora registrato)\n");
    }
    else
    {
        char nome_tmp[32];
        int i = 1;
        while (fscanf(f, "%31s", nome_tmp) == 1)
        {
            printf("  %d. %s%s%s\n", i, YELLOW, nome_tmp, RESET);
            i++;
        }
        fclose(f);
    }

    printf("\n--------------------------------------\n");
    premi_invio();
}
