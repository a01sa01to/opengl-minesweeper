#include <numeric>
#include <string>
#include <tuple>

#include "animation.h"
#include "glut.h"
#include "string_util.h"

using namespace std;
using ll = long long;

// Settings
constexpr int Width = 640;
constexpr int Height = 480;
constexpr int Interval = 10;
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

void display_GameStart() {
  // Model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //gluLookAt(Width / 2, Height / 2, 100, Width / 2, Height / 2, 0, 0, 1, 0);
  gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);

  // Title
  glPushMatrix();
  {
    constexpr int ScrollStart = 2000;
    constexpr int ScrollDuration = 500;
    constexpr int StartSize = 100;
    constexpr int EndSize = 50;
    glColor3d(0, 0, 0);
    double t = (double) (state.time - ScrollStart) / ScrollDuration;
    double y = Animation::easeOut(t);
    double size = Animation::easeOut(t) * (EndSize - StartSize) + StartSize;
    drawString("MineSweeper", -size / 2, y * 20, 0, size);
  }
  glPopMatrix();
}

void display() {
  // Initialize
  glutReshapeWindow(Width, Height);
  glViewport(0, 0, Width, Height);

  // Projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50, (double) Width / Height, 1, 1000);

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

  printf("\r%lld", state.time);

  glutSwapBuffers();
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(Width, Height);
  glutCreateWindow("MineSweeper");
  glutDisplayFunc(display);
  glutTimerFunc(Interval, timer, 10);
  glShadeModel(GL_SMOOTH);
  glutMainLoop();
  return 0;
}