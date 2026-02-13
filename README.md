[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/5fsIc7xe)
# Progetto-finale-2025-Cosestrane
Progetto finale Programmazione Procedurale UniPG Informatica

## Nome: 
Lorenzo

## Cognome: 
Mazzoli

## Matricola: 
390429

## Commenti/modifiche al progetto:

Sono state apportate alcune aggiunte e modifiche personali per rendere l'esperienza di gioco più godibile e bilanciata.

**1. Caratteristiche di ogni nemico** <br>
Le statistiche dei nemici sono state differenziate per tipo, offrendo livelli di sfida crescenti.

* **Democane:** Nemico comune. HP: 40, Attacco: 14.
* **Billi:** Nemico intermedio, presente solo nel Mondo Reale. HP: 60, Attacco: 18.
* **Demotorzone (Boss):** Nemico finale del Soprasotto. HP: 90, Attacco: 20.
* **Meccanica di base:** Tutti i nemici infliggono un danno calcolato sommando il loro valore di attacco base al risultato del lancio di un dado a 6 facce.

**2. Caratteristiche di ogni oggetto e usabilità**<br>
Ogni oggetto possiede un effetto unico e specifico in base alla situazione.

* **Maglietta Fuocoinferno:** Oggetto da combattimento. Conferisce un bonus passivo di +10 alla Difesa Psichica e infligge automaticamente 3 danni da fuoco al nemico alla fine di ogni turno.
* **Schitarrata Metallica:** Oggetto da combattimento. Stordisce il nemico dimezzando i danni inflitti dai suoi attacchi per l'intera durata dello scontro.
* **Bicicletta:** Oggetto di movimento (non usabile in lotta). Permette di avanzare velocemente saltando una zona (due passi totali), al costo di 5 HP per la fatica.
* **Bussola:** Oggetto di esplorazione (non usabile in lotta). Offre due modalità: "Vedi prossima zona" (rivela nemici/oggetti futuri) e "Cerca oggetto" (indica la distanza dall'oggetto desiderato).

**3. Aumento del minimo di attacco, difesa e fortuna** <br>
Per evitare di generare personaggi troppo deboli e ingiocabili, le formule di generazione sono state migliorate rispetto al semplice dado da 20.

* **Attacco e Difesa Psichica:** Il valore minimo è garantito a 8 (range: 8-20). Formula: `rand() % 13 + 8`.
* **Fortuna:** Il valore minimo è garantito a 5 (range: 5-20). Formula: `rand() % 16 + 5`.

**4. Aggiunta del campo HP del giocatore**<br>
Invece di utilizzare il valore della Difesa Psichica come punti vita rimanenti, è stato introdotto un campo dedicato (**hp**) per gestire la salute del personaggio in modo più chiaro.
La Difesa Psichica assume quindi la funzione di scudo per ridurre i danni subiti, mentre gli HP rappresentano la vita effettiva, impostata inizialmente a 50 punti per tutti i giocatori.

**5. Aggiunta campo "nemico_gia_sconfitto"**<br>
È stato aggiunto un flag specifico nella struttura del giocatore. Questo permette al gioco di ricordare se un nemico in una zona è stato appena battuto, evitando che il giocatore sia costretto a combatterlo nuovamente se decide di non muoversi subito.

**6. Sistema danni**<br>
Il sistema di combattimento è stato strutturato per valorizzare le statistiche del giocatore.

* **Attacco:** Il danno inflitto è pari all'Attacco Psichico + 1d6 (dado da 6).
* **Colpo Critico:** Se un tiro percentuale (0-99) è inferiore al valore di Fortuna del giocatore, il danno viene raddoppiato.
* **Difesa:** Il danno subito viene ridotto dalla Difesa Psichica. La formula prevede che la difesa assorba un ammontare di danni pari alla metà del suo valore (Difesa / 2). Il danno minimo subito è comunque sempre 1.

**7. Interfaccia grafica**<br>
L'interfaccia testuale è stata arricchita tramite l'uso di colori ANSI. Questo migliora la leggibilità: i nemici appaiono in rosso, gli oggetti in ciano e i messaggi di vittoria in verde.
È stata inoltre implementata una funzione `pulisci_schermo` per mantenere pulita la console tra le varie fasi di gioco.

**8. Controllo rigoroso input**<br>
Per prevenire crash o comportamenti imprevisti, ogni inserimento da tastiera è protetto da controlli di validazione. Il programma verifica che l'input corrisponda al tipo atteso (numero) e svuota ciclicamente il buffer di memoria (`getchar`) in caso di errore, richiedendo nuovamente l'inserimento finché non è valido.

**9. Sistema ultimi 3 vincitori**<br>
È stata implementata una persistenza dei dati tramite file di testo. Il sistema salva i nomi dei giocatori che sconfiggono il Demotorzone mantenendo una lista dei 3 vincitori più recenti. Quando la lista è piena, il vincitore più vecchio viene rimosso per fare spazio a quello nuovo.
