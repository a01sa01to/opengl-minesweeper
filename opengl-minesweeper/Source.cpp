#include <string>
#include <tuple>

#include <glut.h>

using namespace std;
using ll = long long;

// Settings
constexpr int Interval = 20;
constexpr pair<tuple<int, int, int>, tuple<int, int, int>> OverviewPos = { { 0, 0, 100 }, { 0, 0, 0 } };

// State
enum GameState
{
  GameStart,
  GamePlaying,
  GameOver,
  GameClear
};
struct State {
  GameState gameState = GameStart;
  ll time = 0;
};
State state = State();

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
}

void display_GameStart() {
  // Model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);

  // Title
  glPushMatrix();
  {
    glColor3d(0, 0, 0);
    if (state.time < 2000) {
      glRasterPos3d(0, 0, 0);
    }
    else {
      double y = min(30.0, (state.time - 1000) / 1000.0 * 30);
      glRasterPos3d(0, y, 0);
    }
    const string title = "MineSweeper";
    for (char c : title) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }
  glPopMatrix();
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

  // Draw
  if (state.gameState == GameStart) {
    display_GameStart();
  }

  state.time += Interval;
  if (state.time > 3000) {
    state.time %= 1'000'000'000'000'000'000LL;  // 1e18
    state.time += 3000;
  }

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