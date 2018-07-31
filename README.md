# NetworkSimplexAlgorithm
Coding the network simplex algorithm and finding bad instances

## Übersicht:
Die Network-Klasse händelt den Graphen über die Klasse Edge und die interne Klasse Node.
Die Algorithm-Klasse implementiert den Network-Simplex-Algorithmus und nutzt dazu Circle-Klasse, um mögliche Iterationen abzuspeichern.
Des Weiteren bekommt der Algorithmus eine Funktion aus Pivotalgorithms.h zugewiesen.
Der zweite Teil der BA, sprich Grapherzeugung, ist bisher rudimentär implementiert. Es können momentan zufällige, „gleichverteilte“ Graphen erzeugt werden, als nächstes wird ein evolutionärer Ansatz umgesetzt.

## To-Do-Liste
Hohe Priorität:
- [ ] evolutionärer Ansatz
- [x] random-noise Funktion

Mittlere Priorität:
- [x] Null-Initialisierung (statt extrem teure artifical edges)
- [x] Fehler in Network::clean() finden und beseitigen
- [ ] Network -> txt und zurück implementieren

Niedrige Priorität:
- [ ] Laufzeitoptimierung (vor allem Berechnung von c.costPerFlow)

## Fahrplan
1. Algorithmus vollenden und testen
   - Veränderungen durch verschiedene Initialisierungen und Pivot-Algorithmen klassifizieren ✓   
2. random-noise-Funktion implementieren und Auswirkungen betrachten
   - neue Funktion überlegen, sollten die Auswirkungen nicht überzeugen
3. schlechte Instanzen finden

## Erste Ergebnisse
### Ergebnisse über 200 Iterationen von Randomgraph (200,100,50):
- MaxRev_LC 3.5 | MaxRev_HC 2.5 | MaxVal_LC 6.3 | MaxVal_HC 2.0 | Random_LC 12 | Random_HC 11.4
- Erkenntnis: MaxVal_LC entspricht auf der ersten Hälfte Random (mit halb so schlechten Ergebnissen)
