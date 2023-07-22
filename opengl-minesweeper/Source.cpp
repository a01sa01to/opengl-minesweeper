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
  gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);

  // Title
  glPushMatrix();
  {
    constexpr int ScrollStart = 1500;
    constexpr int ScrollDuration = 500;
    constexpr int StartSize = 100;
    constexpr int EndSize = 50;
    glColor3d(0, 0, 0);
    double t = (double) (state.time - ScrollStart) / ScrollDuration;
    double s = Animation::easeOut(t);
    double size = s * (EndSize - StartSize) + StartSize;
    drawString("MineSweeper", -size / 2, s * 20, 0, size, 2);
  }
  glPopMatrix();

  // Button
  glPushMatrix();
  {
    constexpr int ShowStart = 2500;
    constexpr int OpacityDuration = 250;
    constexpr int size = 36;
    double t = (double) (state.time - ShowStart) / OpacityDuration;
    double opacity = Animation::easeOut(t);
    glColor4d(0, 0, 0, opacity);
    drawString("Press Enter to Start", -size / 2, -20, 0, size, 1);
  }

  // Information
  glPushMatrix();
  {
    constexpr int ShowStart = 2600;
    constexpr int OpacityDuration = 250;
    constexpr int size = 16;
    double t = (double) (state.time - ShowStart) / OpacityDuration;
    double opacity = Animation::linear(t);
    glColor4d(0, 0, 1, opacity);
    drawString("(c) 2023 Asa", -size / 2, -30, 0, size, 1);
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

  glShadeModel(GL_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glutDisplayFunc(display);
  glutTimerFunc(Interval, timer, 10);
  glutMainLoop();
  return 0;
}