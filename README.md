# [Interfaz] Cifrados C++: Permutación de Series y Playfair

Implementación en C++ de los algoritmos de cifrado y descifrado por Permutación de Series y Playfair mediante
una interfaz interactiva.

## 📂 Estructura del Proyecto

```text
├── include
│   ├── interfaz.hpp
│   ├── playfair.hpp
│   ├── procesamiento.hpp
│   └── transposicion.hpp
├── Makefile
└── src
    ├── go_playfair.cpp
    ├── go_transp.cpp
    ├── interfaz.cpp
    ├── playfair.cpp
    ├── procesamiento.cpp
    └── transposicion.cpp
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
./build/go_t
```

- Algoritmo de Playfair

```bash
./build/go_t
```

## Atajos

Funcionan sobre una celda activada por el mouse:

| Combinación | Acción |
|:-:|-|
| `Ctrl` + `A` | Seleccionar todo |
| `Ctrl` + `C` | Copiar |
| `Ctrl` + `V` | Pegar |

