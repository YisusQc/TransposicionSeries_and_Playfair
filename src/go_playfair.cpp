#include "../include/interfaz.hpp"
#include "../include/playfair.hpp"

#include <exception>
#include <iostream>

int main() {
  try {
    interfaz::ejecutar(
      "Cifrado Playfair",
      playfair::cifrar,
      playfair::descifrar
    );
  } catch (const std::exception& e) {
    std::cerr << "Error al iniciar la interfaz: "
              << e.what() << '\n';
    return 1;
  }

  return 0;
}

