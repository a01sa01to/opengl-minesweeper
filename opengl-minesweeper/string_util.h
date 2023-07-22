#pragma once

#include <string>

#include "glut.h"

using namespace std;

// x, y, z: ������̈ʒu�Aw: ������̕�
// ��������
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