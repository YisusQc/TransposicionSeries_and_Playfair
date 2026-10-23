#include "../include/playfair.hpp"
#include "../include/procesamiento.hpp"

#include <array>
#include <stdexcept>
#include <string>

namespace playfair {

namespace {

using Matriz = std::array<std::array<char, 5>, 5>;

struct Posicion {
  std::size_t fila;
  std::size_t columna;
};

std::string unificar_ij(std::string texto) {
  for (char& c : texto) {
    if (c == 'J') {
      c = 'I';
    }
  }
  return texto;
}

std::string preparar_clave(const std::string& clave_original) {
  std::string clave = unificar_ij(procesamiento::normalizar(clave_original));

  if (clave.empty()) {
    throw std::invalid_argument(
      "La clave de Playfair debe contener letras A-Z."
    );
  }

  return clave;
}

Matriz construir_matriz(const std::string& clave) {
  Matriz matriz{};
  std::array<bool, 26> usada{};
  usada['J' - 'A'] = true;

  std::string alfabeto = clave + 
    "ABCDEFGHIKLMNOPQRSTUVWXYZ";

  std::size_t indice = 0;

  for (char c : alfabeto) {
    if (c < 'A' || c > 'Z') {
      continue;
    }

    const std::size_t letra = c - 'A';

    if (usada[letra]) {
      continue;
    }

    usada[letra] = true;
    matriz[indice / 5][indice % 5] = c;
    ++indice;
  }

  return matriz;
}

Posicion buscar(
  const Matriz& matriz,
  char letra
) {
  if (letra == 'J') {
    letra = 'I';
  }

  for (std::size_t fila = 0; fila < 5; ++fila) {
    for (std::size_t columna = 0; columna < 5; ++columna) {
      if (matriz[fila][columna] == letra) {
        return {fila, columna};
      }
    }
  }

  throw std::invalid_argument(
    "Caracter no valido en Playfair."
  );
}

char relleno_para(char letra) {
  // Evita formar un par idéntico cuando la letra es X.
  return letra == 'X' ? 'Q' : 'X';
}

std::string preparar_pares(const std::string& texto_original) {
  const std::string texto =
    unificar_ij(procesamiento::normalizar(texto_original));

  if (texto.empty()) {
    throw std::invalid_argument(
      "El texto debe contener letras A-Z."
    );
  }

  std::string preparado;
  preparado.reserve(texto.size() + 2);

  std::size_t i = 0;

  while (i < texto.size()) {
    const char primero = texto[i];

    if (i + 1 >= texto.size()) {
      preparado += primero;
      preparado += relleno_para(primero);
      ++i;
    } else if (texto[i] == texto[i + 1]) {
      preparado += primero;
      preparado += relleno_para(primero);
      ++i;
    } else {
      preparado += primero;
      preparado += texto[i + 1];
      i += 2;
    }
  }

  return preparado;
}

std::string transformar(
  const std::string& texto,
  const Matriz& matriz,
  bool cifrando
) {
  if (texto.empty() || texto.size() % 2 != 0) {
    throw std::invalid_argument(
      "Playfair necesita un texto no vacio "
      "con una cantidad par de letras."
    );
  }

  std::string resultado;
  resultado.reserve(texto.size());

  const int desplazamiento = cifrando ? 1 : 4;

  for (std::size_t i = 0; i < texto.size(); i += 2) {
    const Posicion a = buscar(matriz, texto[i]);
    const Posicion b = buscar(matriz, texto[i + 1]);

    if (a.fila == b.fila) {
      // Misma fila: derecha al cifrar, izquierda al descifrar.
      resultado += matriz[a.fila]
        [(a.columna + desplazamiento) % 5];

      resultado += matriz[b.fila]
        [(b.columna + desplazamiento) % 5];
    } else if (a.columna == b.columna) {
      // Misma columna: abajo al cifrar, arriba al descifrar.
      resultado += matriz
        [(a.fila + desplazamiento) % 5][a.columna];

      resultado += matriz
        [(b.fila + desplazamiento) % 5][b.columna];
    } else {
      // Rectángulo: intercambiar las columnas.
      resultado += matriz[a.fila][b.columna];
      resultado += matriz[b.fila][a.columna];
    }
  }

  return resultado;
}

} // namespace

std::string cifrar(
  const std::string& texto,
  const std::string& clave_original
) {
  const std::string clave = preparar_clave(clave_original);
  const std::string preparado = preparar_pares(texto);
  const Matriz matriz = construir_matriz(clave);

  return transformar(preparado, matriz, true);
}

std::string descifrar(
  const std::string& texto,
  const std::string& clave_original
) {
  const std::string clave = preparar_clave(clave_original);
  const std::string cifrado = unificar_ij(procesamiento::normalizar(texto));

  if (cifrado.empty()) {
    throw std::invalid_argument(
      "El texto cifrado no debe estar vacio."
    );
  }

  const Matriz matriz = construir_matriz(clave);
  return transformar(cifrado, matriz, false);
}

} // namespace playfair

