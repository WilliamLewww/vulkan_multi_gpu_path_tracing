#include "engine.h"

int main() {
  Engine* engine = new Engine();
  engine->start();

  delete engine;
  return 0;
}