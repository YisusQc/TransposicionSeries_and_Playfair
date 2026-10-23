#include "../include/transposicion.hpp"
#include "../include/procesamiento.hpp"

#include <algorithm>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace transposicion {

namespace {

// Convierte una clave como "3 1 4 2" o "3,1,4,2"
// en una permutacion de posiciones numeradas desde 1.
std::vector<std::size_t> validar_clave(
  const std::string& clave
) {
  // Aceptar solamente numeros separados por espacios
  // o comas, sin texto adicional.
  static const std::regex formato(R"([0-9]+(?:[\s,]+[0-9]+)*)");

  if (!std::regex_match(clave, formato)) {
    throw std::invalid_argument(
      "Clave invalida. Usa posiciones separadas por "
      "espacios o comas, por ejemplo: 3 1 4 2."
    );
  }

  std::istringstream entrada(clave);
  std::vector<std::size_t> posiciones;
  std::string token;

  // La expresion regular ya valido el formato.
  // Reemplazamos las comas por espacios para leer
  // todos los numeros con un flujo.
  std::string clave_separada = clave;

  for (char& c : clave_separada) {
    if (c == ',') {
      c = ' ';
    }
  }

  std::istringstream numeros(clave_separada);
  while (numeros >> token) {
    std::size_t posicion = 0;

    try {
      std::size_t caracteres_leidos = 0;
      posicion = std::stoull(token, &caracteres_leidos);

      if (caracteres_leidos != token.size()) {
        throw std::invalid_argument("Numero invalido.");
      }
    } catch (const std::exception&) {
      throw std::invalid_argument(
        "La clave contiene una posicion numerica invalida."
      );
    }

    posiciones.push_back(posicion);
  }

  const std::size_t n = posiciones.size();

  if (n < 2) {
    throw std::invalid_argument(
      "La clave debe contener al menos 2 posiciones."
    );
  }

  // Una permutacion valida debe contener exactamente
  // los numeros 1, 2, ..., N, sin repeticiones.
  std::vector<std::size_t> ordenadas = posiciones;
  std::sort(ordenadas.begin(), ordenadas.end());

  for (std::size_t i = 0; i < n; ++i) {
    if (ordenadas[i] != i + 1) {
      throw std::invalid_argument(
        "La clave debe contener una vez cada numero "
        "del 1 al N, sin repetir posiciones."
      );
    }
  }

  // Convertir las posiciones de base 1 a base 0.
  for (std::size_t& posicion : posiciones) {
    --posicion;
  }

  return posiciones;
}

// Aplica la permutacion por bloques.
//
// En el ultimo bloque solo se consideran las posiciones
// existentes. No se agrega ningun caracter de relleno.
std::string transformar(
  const std::string& texto,
  const std::vector<std::size_t>& permutacion,
  bool inversa
) {
  const std::size_t n = permutacion.size();
  std::string resultado;
  resultado.reserve(texto.size());

  for (std::size_t inicio = 0;
    inicio < texto.size();
    inicio += n) {

    const std::size_t longitud = std::min(n, texto.size() - inicio);

    // Conservar, en el orden de la clave, solo las
    // posiciones que existen en este bloque.
    std::vector<std::size_t> orden;

    for (std::size_t posicion : permutacion) {
      if (posicion < longitud) {
        orden.push_back(posicion);
      }
    }

    if (!inversa) {
      // C[i] = M[pi(i)]
      for (std::size_t posicion : orden) {
        resultado += texto[inicio + posicion];
      }
    } else {
      // Reconstruir el bloque original.
      std::string bloque_original(longitud, '\0');

      for (std::size_t i = 0; i < orden.size(); ++i) {
        bloque_original[orden[i]] = texto[inicio + i];
      }

      resultado += bloque_original;
    }
  }

  return resultado;
}

} // namespace

std::string cifrar(
  const std::string& texto,
  const std::string& clave
) {
  const std::string mensaje = procesamiento::normalizar(texto);
  const auto permutacion = validar_clave(clave);

  if (mensaje.empty()) {
    throw std::invalid_argument(
      "El texto no debe estar vacio."
    );
  }

  return transformar(mensaje, permutacion, false);
}

std::string descifrar(
  const std::string& texto,
  const std::string& clave
) {
  const std::string mensaje = procesamiento::normalizar(texto);
  const auto permutacion = validar_clave(clave);

  if (mensaje.empty()) {
    throw std::invalid_argument(
      "El texto no debe estar vacio."
    );
  }

  return transformar(mensaje, permutacion, true);
}

} // namespace transposicion

