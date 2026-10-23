#include "../include/interfaz.hpp"
#include "../include/transposicion.hpp"

#include <exception>
#include <iostream>

int main() {
  try {
    interfaz::ejecutar(
      "Transposicion por bloques",
      transposicion::cifrar,
      transposicion::descifrar
    );
  } catch (const std::exception& e) {
    std::cerr << "Error al iniciar la interfaz: "
              << e.what() << '\n';
    return 1;
  }

  return 0;
}

