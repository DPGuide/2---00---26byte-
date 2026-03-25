def generiere_galaktische_bytes(max_schritte):
    # Deine Startwerte
    a = 0
    b = 2
    
    print("🚀 Starte die Generierung des 42-Protokolls...\n")
    print("-" * 50)
    
    for schritt in range(1, max_schritte + 1):
        # 1. Die neue Byte-Größe berechnen (Summe der letzten beiden)
        aktuelle_bytes = a + b
        
        # 2. Den Speicherblock mit "00" füllen
        # "00 " wird so oft multipliziert, wie wir Bytes haben. 
        # strip() entfernt nur das allerletzte Leerzeichen am Ende.
        speicher_block = ("00 " * aktuelle_bytes).strip()
        
        # 3. Ausgabe auf der Konsole
        print(f"Schritt {schritt}: {a} + {b} = {aktuelle_bytes} Bytes")
        print(f"Daten: [{speicher_block}]\n")
        
        # 4. Die Werte für die nächste Runde weiterrücken
        a = b
        b = aktuelle_bytes

# Wir lassen das Ganze 7 Schritte laufen, um genau bei der 42 zu landen!
generiere_galaktische_bytes(7)