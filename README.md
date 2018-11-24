# A graphtheoretical implementation of the network-simplex-algorithm
This code implements the network-simplex-algorithm on integers as well as an exponential instance and some experimental research looking for bad instances. The bachelor thesis contains a German explanation of both the algorithm and the code.

# Eine graphentheoretische Herleitung und Implementierung des Netzwerk-Simplex-Algorithmus
Sehr viele Probleme in der Informatik lassen sich als Lineares Programm darstellen, so
auch das in Abschnitt 2.1 eingeführte Min-Cost-Flow-Problem aus dem Bereich der
Graphentheorie. Logistische und kombinatorische Fragestellungen der Praxis lassen
sich wiederum durch dieses modellieren. Das Simplex-Verfahren löst Lineare Programme in der Anwendung
sehr schnell, obwohl die Worst-Case-Laufzeit nicht polynomiell ist.

Dantzig und Orden vereinfachten in den 1950er Jahren das Simplex-
Verfahren für die konkrete Struktur des Min-Cost-Flow-Problems zum Netzwerk-
Simplex-Algorithmus. Diese Vereinfachung ist um einen Faktor
von 200–300 schneller, außerdem lässt sich der Netzwerk-Simplex-Algorithmus rein
graphentheoretisch definieren.

In dieser Bachelorarbeit wird in Kapitel 2 die graphentheoretische Herleitung mit
Beweis der Korrektheit vollzogen und anschließend in Kapitel 3 die Implementierung
des Algorithmus in C++ dargelegt. Insgesamt wurden sechs Varianten umgesetzt. Für
eine von diesen veröffentlichte Zadeh 1973 eine Familie von exponentiellen
Instanzen; diese wird in Kapitel 4 vorgestellt und verifiziert. Kapitel 5 greift die Frage
auf, ob sich allgemein mit einfachen algorithmischen Ansätzen schlechte Instanzen für
beliebige Versionen des Netzwerk-Simplex-Algorithmus finden lassen. Hierzu muss an
dieser Stelle ein negatives Ergebnis verzeichnet werden. Kapitel 6 skizziert als Abschluss
Vorschläge für eine Vertiefung der Gesamtthematik.
