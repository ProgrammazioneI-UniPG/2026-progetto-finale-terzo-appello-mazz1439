// --- DEFINIZIONE COLORI E FORMATTAZIONE ---
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"
#define B_WHITE "\033[1;37m"  
#define B_RED "\033[1;31m"  

enum Tipo_zona {
    bosco,
    scuola,
    laboratorio,
    caverna,
    strada,
    giardino,
    supermercato,
    centrale_elettrica,
    deposito_abbandonato,
    stazione_polizia
};

enum Tipo_nemico {
    nessun_nemico,
    billi,
    democane,
    demotorzone
};

enum Tipo_oggetto {
    nessun_oggetto,
    bicicletta,
    maglietta_fuocoinferno,
    bussola,
    schitarrata_metallica
};

typedef struct Zona_mondoreale {
    enum Tipo_zona tipo;
    enum Tipo_nemico nemico;
    enum Tipo_oggetto oggetto;
    struct Zona_mondoreale *avanti;
    struct Zona_mondoreale *indietro;
    struct Zona_soprasotto *link_soprasotto;

} Zona_mondoreale;

typedef struct Zona_soprasotto {
    enum Tipo_zona tipo;
    enum Tipo_nemico nemico;
    struct Zona_soprasotto *avanti;
    struct Zona_soprasotto *indietro;
    struct Zona_mondoreale *link_mondoreale;
} Zona_soprasotto;

typedef struct Giocatore
{
    char nome[32];
    int mondo;
    struct Zona_mondoreale *pos_mondoreale;
    struct Zona_soprasotto *pos_soprasotto;
    int attacco_psichico;
    int difesa_psichica;
    int fortuna;
    int hp;
    enum Tipo_oggetto zaino[3];
    int nemico_gia_sconfitto;
} Giocatore;


void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();
