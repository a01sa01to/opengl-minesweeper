#include <glut.h>

using namespace std;

constexpr int Interval = 20;

void timer(int t) {
  glutPostRedisplay();
  glutTimerFunc(Interval, timer, 20);
}

void reshape(int w, int h) {
  // Initialize
  glViewport(0, 0, w, h);

  // Projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50, (double) w / h, 1, 1000);

  // Model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(100, 60, 100, 0, 0, 0, 0, 1, 0);
}

void display() {
  // Clear
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw Axis
  // X
  glColor3d(1, 0, 0), glBegin(GL_LINES), glVertex3d(0, 0, 0), glVertex3d(100, 0, 0), glEnd();
  // Y
  glColor3d(0, 1, 0), glBegin(GL_LINES), glVertex3d(0, 0, 0), glVertex3d(0, 100, 0), glEnd();
  // Z
  glColor3d(0, 0, 1), glBegin(GL_LINES), glVertex3d(0, 0, 0), glVertex3d(0, 0, 100), glEnd();

  glutSwapBuffers();
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(640, 480);
  glutCreateWindow("MineSweeper");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutTimerFunc(Interval, timer, 10);
  glShadeModel(GL_SMOOTH);
  glutMainLoop();
  return 0;
}