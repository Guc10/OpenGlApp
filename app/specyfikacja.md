# Projekt OpenGL — Symulacja ruchu kuli 2D

## Opis projektu

Projekt zakłada stworzenie symulacji ruchu kuli w środowisku 2D, w której kulą można sterować za pomocą strzałek.  
W symulacji działa system **grawitacji** przyciągający obiekt w kierunku podłoża oraz **system kolizji ze ścianami**, zapewniający realistyczne odbicia.  
Interfejs graficzny aplikacji realizowany jest przy użyciu **biblioteki GLFW**, a renderowaniem obiektów zajmuje się **OpenGL 4.5** z obsługą przez **GLAD**.  
Za obliczenia fizyczne i matematyczne odpowiada **biblioteka GLM**.

Aplikacja pozwoli na wizualne przedstawienie podstawowych zjawisk fizycznych, takich jak ruch, grawitacja i zderzenia sprężyste.

---

## Technologie

| Komponent | Technologia |
|------------|--------------|
| Język programowania | C++23 |
| Rendering | OpenGL 4.5 |
|UI|ImGui|
| Zarządzanie oknem i wejściem | GLFW |
| Ładowanie funkcji OpenGL | GLAD |
| Obliczenia wektorowe i macierze | GLM |
| Budowanie projektu | CMake |

---

## Podział zadań

### **Gustaw Grześkowiak — Logika fizyki i ruchu (GLM + kolizje)**

**Zakres odpowiedzialności:**
- Implementacja klasy obsługującej fizykę:
    - wektory prędkości i przyspieszenia,
    - grawitację,
    - kolizje ze ścianami z tłumieniem energii (np. współczynnik odbicia `r = 0.9`).
- Aktualizacja pozycji kuli w czasie (`deltaTime`).
- Detekcja i reakcja na kolizje z granicami okna.

---

### **Filip Radke — Renderowanie i OpenGL (GLAD + shadery)**

**Zakres odpowiedzialności:**
- Inicjalizacja OpenGL 4.5 i GLAD.
- Tworzenie i kompilacja shaderów.
- Tworzenie buforów (VAO, VBO) i rysowanie obiektu (kuli).
- Implementacja klasy renderującej:
    - Rysowanie kuli i tła.
    - Obsługa koloru, pozycji i skalowania w shaderze.
- Optymalizacja renderowania i czyszczenia ekranu.

---

### **Filip Lubka — Interfejs, wejście i główny loop (GLFW / ImGui + integracja)**

**Zakres odpowiedzialności:**
- Inicjalizacja okna i kontekstu OpenGL (`Window`).
- Obsługa wejścia (mysz, klawiatura).
- Sterowanie kulą (strzałki/wsad).
- Pętla główna programu (`main.cpp`):
    - aktualizacja fizyki,
    - renderowanie sceny,
    - obsługa zdarzeń wejściowych.
- Koordynacja integracji komponentów fizyki i renderowania.

---

### 

---

# Etapy realizacji projektu
## (**W razie potrzeb będziemy się wspierać siłą przyjaźni !!!**)

### Etap 1 — Konfiguracja środowiska
- Ustawienie CMakeLists.txt.
- Pobranie bibliotek (GLFW, GLAD, GLM).
- Testowe okno z kolorem tła.

### Etap 2 — Renderowanie (Filip Radke)
- Stworzenie shaderów i buforów OpenGL.
- Renderowanie kuli w środowisku 2D.

### Etap 3 — Implementacja fizyki (Gustaw Grześkowiak)
- Dodanie klasy z implementacją fiyzki.
- Implementacja wektorów ruchu, grawitacji i kolizji.

### Etap 4 — Sterowanie i interfejs (Filip Lubka)
- Implementacja obsługi myszy i klawiatury.
- Dodanie możliwości przeciągania kuli.
- Integracja logiki fizyki i renderingu.

---

## Założenia fizyczne

- Grawitacja: `g = 9.81`
- Czas symulacji liczony na podstawie `deltaTime`
- Kolizje sprężyste z tłumieniem `r = 0.9`
- Współrzędne świata: zakres `[-1, 1]`
- W projekcie nie uwzględniamy oporów powietrza