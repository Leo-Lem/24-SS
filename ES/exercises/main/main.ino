#include "projects.h"
#define PROJECT(x)              \
  void setup() { setup##x(); }; \
  void loop() { loop##x(); };

PROJECT(33);