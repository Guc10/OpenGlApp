Dobra. Teraz wchodzimy poziom wyżej. Z kulki robimy ciecz, z prostych odbić robimy SPH, a z projektu szkolnego robimy coś, co brzmi jak mini-silnik fizyczny.

---

# Projekt OpenGL — Symulacja cieczy 2D (SPH)

## Opis projektu

Projekt zakłada stworzenie interaktywnej symulacji cieczy w środowisku 2D, opartej na metodzie **Smoothed Particle Hydrodynamics (SPH)**.

Ciecz reprezentowana jest jako zbiór cząstek, które oddziałują na siebie poprzez siły:

* ciśnienia,
* lepkości,
* grawitacji.

System uwzględnia:

* kolizje z granicami sceny,
* interakcję z przeszkodami,
* dynamiczne generowanie nowych cząstek.

Renderowanie realizowane jest przy użyciu **OpenGL 4.5**, z obsługą przez **GLAD**, a interfejs użytkownika oparty jest na **GLFW** i **ImGui**.
Za obliczenia matematyczne i wektorowe odpowiada **biblioteka GLM**.

Aplikacja umożliwia wizualne przedstawienie zjawisk takich jak przepływ cieczy, oddziaływania międzycząsteczkowe oraz wpływ parametrów fizycznych na zachowanie płynu.

---

## Technologie

| Komponent                    | Technologia |
| ---------------------------- | ----------- |
| Język programowania          | C++23       |
| Rendering                    | OpenGL 4.5  |
| UI                           | ImGui       |
| Zarządzanie oknem i wejściem | GLFW        |
| Ładowanie funkcji OpenGL     | GLAD        |
| Obliczenia wektorowe         | GLM         |
| Budowanie projektu           | CMake       |

---

# Podział projektu na zadania

---



* Implementacja struktury `Particle`:

  * pozycja,
  * prędkość,
  * siły,
  * gęstość,
  * ciśnienie.
* Obliczanie gęstości (kernel Poly6).
* Obliczanie siły ciśnienia (Spiky gradient).
* Obliczanie lepkości (Laplacian).
* Implementacja całkowania numerycznego (Euler / semi-implicit).
* Stabilizacja symulacji (substeps, ograniczenie dt).

---



* Inicjalizacja OpenGL 4.5 i GLAD.
* Tworzenie i kompilacja shaderów (vertex + fragment).
* Implementacja renderowania instancyjnego cząstek.
* Tworzenie VAO/VBO.
* Renderowanie do FBO (opcjonalnie).
* Kolorowanie cząstek w zależności od prędkości.
* Optymalizacja wydajności przy dużej liczbie cząstek.

---



* Inicjalizacja okna i kontekstu OpenGL.
* Implementacja klasy `MainWindow`.
* Główna pętla programu:

  * aktualizacja fizyki,
  * renderowanie,
  * obsługa zdarzeń.
* Integracja modułu fizyki z renderingiem.
* Implementacja panelu sterowania (ImGui):

  * zmiana grawitacji,
  * lepkości,
  * intensywności generowania cząstek,
  * reset symulacji.

---

# Etapy realizacji projektu

*(W razie potrzeby wspieramy się wzajemnie, używając siły przyjaźni.)*

---

### Etap 1 — Konfiguracja środowiska

* Ustawienie `CMakeLists.txt`.
* Integracja GLFW, GLAD, GLM, ImGui.
* Testowe okno z kolorem tła.

---

### Etap 2 — Renderowanie cząstek

* Implementacja shaderów.
* Renderowanie pojedynczej cząstki.
* Wdrożenie instancing.

---

### Etap 3 — Implementacja silnika SPH

* Struktura cząstek.
* Obliczanie gęstości i ciśnienia.
* Siły lepkości i grawitacji.
* Stabilizacja symulacji.

---

### Etap 4 — Integracja i UI

* Połączenie fizyki z renderingiem.
* Panel sterowania parametrami.
* Testy stabilności i optymalizacja.

---

## Założenia fizyczne

* Grawitacja: regulowana (domyślnie `g = 9.81`)
* Metoda: SPH (Smoothed Particle Hydrodynamics)
* Kernel wygładzający o promieniu `h`
* Czas symulacji oparty na `deltaTime`
* Kolizje z granicami z tłumieniem energii (`r ≈ 0.8–0.95`)
* Symulacja 2D
* Maksymalna liczba cząstek ograniczona ze względów wydajnościowych

---

