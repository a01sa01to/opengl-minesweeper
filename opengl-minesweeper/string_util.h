#pragma once

#include <string>

#include "glut.h"

using namespace std;

// x, y, z: ¶ñÌÊuAw: ¶ñÌ
// ¶ºµ¦
inline void drawString(const string str, const double x, const double y, const double z, const double width, const double weight) {
  const int w = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*) str.c_str());
  glPushMatrix();
  glTranslated(x, y, z);
  const double scale = width / w;
  glScaled(scale, scale, scale);
  glLineWidth(weight);
  for (char c : str) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
  glPopMatrix();
}

inline void drawMonoString(const string str, const double x, const double y, const double z, const double width, const double weight) {
  const int w = glutStrokeLength(GLUT_STROKE_MONO_ROMAN, (const unsigned char*) str.c_str());
  glPushMatrix();
  glTranslated(x, y, z);
  const double scale = width / w;
  glScaled(scale, scale, scale);
  glLineWidth(weight);
  for (char c : str) glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
  glPopMatrix();
}