# [Interfaz] Cifrados C++: Permutación de Series y Playfair

Implementación en C++ de los algoritmos de cifrado y descifrado por Permutación de Series y Playfair mediante
una interfaz interactiva.

## 📂 Estructura del Proyecto

```text
├── include
│   ├── interfaz.hpp
│   ├── permutacion_series.hpp
│   ├── playfair.hpp
│   └── procesamiento.hpp
├── Makefile
└── src
    ├── go_permu_series.cpp
    ├── go_playfair.cpp
    ├── interfaz.cpp
    ├── permutacion_series.cpp
    ├── playfair.cpp
    └── procesamiento.cpp
```

## Requerimientos

- make
- SFML3
- gcc

## Compilación y ejecución

Compilar (`/build` se creará si no existe)

```bash
make
```

Ejecutar por separado

- Algoritmo de Permutación por Series

```bash
./build/go_ps
```

- Algoritmo de Playfair

```bash
./build/go_p
```

## Atajos

Funcionan sobre una celda activada por el mouse:

| Combinación | Acción |
|:-:|-|
| `Ctrl` + `A` | Seleccionar todo |
| `Ctrl` + `Shift` + `Left` / `Right` | Seleccionar |
| `Ctrl` + `C` | Copiar |
| `Ctrl` + `V` | Pegar |

> [!NOTE]
> Redimensionar alguna ventana de interfaz manualmente en ejecución, puede tener efectos como la detección de coordenadas
> incorrectas del mouse sobre los elementos.
