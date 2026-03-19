# HEX / BIN / ASCII Datei-Editor

## Projektbeschreibung

Dieses Projekt implementiert einen einfachen terminalbasierten Editor zur Untersuchung und Bearbeitung von Binärdateien auf Byte-Ebene.  
Das Programm stellt den Inhalt einer Datei gleichzeitig in drei Darstellungsformen dar:

- Hexadezimal
- Binär (8 Bit pro Byte)
- ASCII / Zeichen

Dadurch können sowohl binäre Strukturen als auch lesbare Textanteile innerhalb einer Datei analysiert werden.

Zusätzlich ermöglicht der Editor das gezielte Ändern einzelner Bytes sowie das Speichern der Änderungen.

Das Programm ist bewusst als Kommandozeilenanwendung umgesetzt und verzichtet auf eine grafische Benutzeroberfläche. Der Fokus liegt auf der Darstellung und Manipulation von Bytewerten sowie auf einer modularen Softwarestruktur.

---

## Architektur

Die Software ist modular aufgebaut und orientiert sich an den in der Konzeptionsphase definierten Hauptkomponenten.

Modul | Aufgabe
----- | ------
main |   Benutzeroberfläche (CLI) und Befehlssteuerung
fileio |   Laden und Speichern von Binärdateien
formatter |   Darstellung der Bytes in HEX / BIN / ASCII
editor |   Bearbeitung einzelner Bytes im Speicherpuffer

Die Datei wird beim Start vollständig in einen Bytepuffer geladen. Alle Bearbeitungen erfolgen zunächst im Arbeitsspeicher und werden erst durch einen Speicherbefehl dauerhaft in die Datei geschrieben.

---

## Projektstruktur

hex-editor

src  
- main.cpp  
- fileio.cpp  
- fileio.h  
- formatter.cpp  
- formatter.h  
- editor.cpp  
- editor.h  

testfiles  
- sample.bin  

README.md  
.gitignore

---

## Kompilieren

Das Programm kann mit einem C++-Compiler (z. B. g++) kompiliert werden.

g++ src/main.cpp src/fileio.cpp src/formatter.cpp src/editor.cpp -o editor

---

## Programm starten

Eine Datei kann entweder direkt beim Start übergeben werden:

./editor testfiles/sample.bin

oder das Programm fragt nach dem Dateipfad.

---

## Befehle

Befehl | Beschreibung
------ | -----------
view |   aktuelle Seite anzeigen
next |   nächste Seite anzeigen
prev |   vorherige Seite anzeigen
goto <offset> |   zu einer bestimmten Adresse springen
set <offset> <wert> |   Byte an einer Position ändern
save |   Datei überschreiben
saveas <dateiname> |   Datei unter neuem Namen speichern
status |   Statusinformationen anzeigen
help |   Befehlsübersicht anzeigen
quit |   Programm beenden

---

## Beispiele

Zu Offset springen

goto 0x40

oder

goto 64

Byte ändern

Hexadezimal

set 10 FF

Binär

set 10 0b10101010

Zeichen

set 10 'A'

---

## Tests

Zur Überprüfung der Implementierung wurden mehrere Testfälle durchgeführt:

- Laden kleiner Textdateien
- Laden größerer Binärdateien
- Navigation mit view, next, prev und goto
- Bearbeitung einzelner Bytes mit set
- Speichern mit save und saveas
- Prüfung fehlerhafter Eingaben

  IIm Ordner testfiles befindet sich eine Beispiel-Datei, mit der das Programm direkt getestet werden kann.
---

## Bekannte Einschränkungen

- Die komplette Datei wird in den Arbeitsspeicher geladen. Sehr große Dateien könnten daher zu Speicherproblemen führen.
- Das Programm unterstützt keine Analyse komplexer Dateiformate wie PNG oder WAV.
- Die Darstellung ist auf eine Terminalausgabe ausgelegt und benötigt für die BIN-Spalte eine ausreichende Fensterbreite.

---

## Ziel des Projekts

Ziel dieses Projekts ist die Umsetzung eines einfachen Byte-Editors mit modularer Struktur sowie die Anwendung grundlegender Konzepte der C++-Programmierung:

- Dateiverarbeitung
- Speicherverwaltung
- modulare Programmentwicklung
- Kommandozeileninteraktion
