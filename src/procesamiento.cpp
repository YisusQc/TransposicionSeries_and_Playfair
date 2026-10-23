#include "../include/procesamiento.hpp"

#include <cctype>
#include <string>

namespace procesamiento {

std::string normalizar(const std::string& texto) {
  std::string resultado;
  resultado.reserve(texto.size());

  for (std::size_t i = 0; i < texto.size();) {
    // Los reemplazos UTF-8 se reconocen antes de procesar
    // los caracteres ASCII.
    if (i + 1 < texto.size()) {
      const std::string par = texto.substr(i, 2);

      if (par == "\xC3\xA1" || par == "\xC3\x81") {
        resultado += 'A'; i += 2; continue; // á Á
      }
      if (par == "\xC3\xA9" || par == "\xC3\x89") {
        resultado += 'E'; i += 2; continue; // é É
      }
      if (par == "\xC3\xAD" || par == "\xC3\x8D") {
        resultado += 'I'; i += 2; continue; // í Í
      }
      if (par == "\xC3\xB3" || par == "\xC3\x93") {
        resultado += 'O'; i += 2; continue; // ó Ó
      }
      if (par == "\xC3\xBA" || par == "\xC3\x9A") {
        resultado += 'U'; i += 2; continue; // ú Ú
      }
      if (par == "\xC3\xBC" || par == "\xC3\x9C") {
        resultado += 'U'; i += 2; continue; // ü Ü
      }
      if (par == "\xC3\xB1" || par == "\xC3\x91") {
        resultado += 'N'; i += 2; continue; // ñ Ñ
      }
    }

    const unsigned char c =
      static_cast<unsigned char>(texto[i]);

    if (c < 128) {
      const char mayuscula =
        static_cast<char>(std::toupper(c));

      if (mayuscula >= 'A' && mayuscula <= 'Z') {
        resultado += mayuscula;
      }
      ++i;
    } else {
      // Ignora los bytes de caracteres UTF-8 no reconocidos.
      ++i;
    }
  }

  return resultado;
}

} // namespace procesamiento

