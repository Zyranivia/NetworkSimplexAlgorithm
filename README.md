# NetworkSimplexAlgorithm
Coding the network simplex algorithm and finding bad instances

## Übersicht:
Die Network-Klasse händelt den Graphen über die Klasse Edge und die interne Klasse Node.
Die Algorithm-Klasse implementiert den Network-Simplex-Algorithmus und nutzt dazu Circle-Klasse, um mögliche Iterationen abzuspeichern.
Des Weiteren bekommt der Algorithmus eine Funktion aus Pivotalgorithms.h zugewiesen.
Der zweite Teil der BA, sprich Grapherzeugung, ist bisher rudimentär implementiert.

## To-Do-Liste
Hohe Priorität:
- [ ] Null-Initialisierung (statt extrem teure artifical edges)
- [ ] random-noise Funktion

Mittlere Priorität:
- [ ] Laufzeitoptimierung (vor allem Berechnung von c.costPerFlow)
- [ ] Fehler in Network::clean() finden und beseitigen
- [ ] Network -> txt und zurück implementieren

Niedrige Priorität:
- [ ] Mehr Pivot-Algorithmen

## Fahrplan
1. Algorithmus vollenden und testen
   - Veränderungen durch verschiedene Initialisierungen und Pivot-Algorithmen klassifizieren
2. random-noise-Funktion implementieren und Auswirkungen betrachten
   - neue Funktion überlegen, sollten die Auswirkungen nicht überzeugen
3. schlechte Instanzen finden
