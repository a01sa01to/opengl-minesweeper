#pragma once

#include <string>

#include "glut.h"

using namespace std;

// x, y, z: •¶š—ñ‚ÌˆÊ’uAw: •¶š—ñ‚Ì•
// ¶‰º‘µ‚¦
inline void drawString(const string str, const double x, const double y, const double z, const double w) {
  const int width = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*) str.c_str());
  glPushMatrix();
  glTranslated(x, y, z);
  const double scale = w / width;
  glScaled(scale, scale, scale);
  for (char c : str) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
  glPopMatrix();
}