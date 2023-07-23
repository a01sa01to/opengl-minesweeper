#include <algorithm>
#include <array>
#include <bitset>
#include <numeric>
#include <string>
#include <tuple>

#include "animation.h"
#include "glut.h"
#include "string_util.h"

using namespace std;
using uint = unsigned int;
using ll = long long;

// Settings
constexpr uint Width = 640;
constexpr uint Height = 480;
constexpr uint Interval = 10;

// Keyboard
constexpr uint Key_Nothing = 0;
constexpr uint Key_Enter = 13;
constexpr uint Key_Space = 32;
constexpr uint Key_Left = 100;
constexpr uint Key_Up = 101;
constexpr uint Key_Right = 102;
constexpr uint Key_Down = 103;

// Grid
constexpr uint GridWidth = 15;
constexpr uint GridHeight = 10;
constexpr uint GridBit = 4;
constexpr uint BombCount = (GridWidth * GridHeight) * 15 / 100;  // 15%
constexpr bitset<GridBit> Grid_Open(0b0001);
constexpr bitset<GridBit> Grid_Question(0b0010);
constexpr bitset<GridBit> Grid_Flag(0b0100);
constexpr bitset<GridBit> Grid_Bomb(0b1000);

// State
enum GameState
{
  GameStart,
  GameStartPlayingTransition,
  GamePlaying,
  GameOver,
  GameClear
};
struct State {
  GameState gameState = GameStart;
  ll time = 0;
  int key = Key_Nothing;
  array<array<bitset<GridBit>, GridWidth>, GridHeight> grid;
  pair<int, int> cursor = { 0, 0 };
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
    double alpha = 1 - (state.gameState == GameStartPlayingTransition ? state.time / 500.0 : 0);
    glColor4d(0, 0, 0, alpha);
    double t = (double) (state.time - ScrollStart) / ScrollDuration;
    if (state.gameState == GameStartPlayingTransition) t = 1;
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
    if (state.gameState == GameStartPlayingTransition) opacity = 1 - (double) state.time / OpacityDuration;
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
    if (state.gameState == GameStartPlayingTransition) opacity = 1 - (double) state.time / OpacityDuration;
    glColor4d(0, 0, 1, opacity);
    drawString("(c) 2023 Asa", -size / 2, -30, 0, size, 1);
  }
  glPopMatrix();

  // Press Enter
  if (state.time > 3000 && state.key == Key_Enter) {
    state.gameState = GameStartPlayingTransition;
    state.time = 0;
  }
}

void display_Playing() {
  // Model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);

  if (state.gameState == GameStartPlayingTransition && state.time < 750) return;
  if (state.gameState == GameStartPlayingTransition) state.gameState = GamePlaying;

  // Draw Grid
  constexpr int StartX = -50;
  constexpr int StartY = 40;
  constexpr int EndX = 50;
  constexpr int EndY = -30;
  constexpr double SquareWidth = (double) (EndX - StartX) / GridWidth;
  constexpr double SquareHeight = (double) (EndY - StartY) / GridHeight;
  glPushMatrix();
  {
    glLineWidth(1);
    glColor3d(0.3, 0.3, 0.3);
    for (int i = 0; i <= GridWidth; i++) {
      double x = StartX + SquareWidth * i;
      glBegin(GL_LINES), glVertex3d(x, StartY, 0), glVertex3d(x, EndY, 0), glEnd();
    }
    for (int j = 0; j <= GridHeight; j++) {
      double y = StartY + SquareHeight * j;
      glBegin(GL_LINES), glVertex3d(StartX, y, 0), glVertex3d(EndX, y, 0), glEnd();
    }
  }
  glPopMatrix();

  // Draw Cursor
  glPushMatrix();
  {
    glLineWidth(3);
    glColor3d(1, 0, 0);
    double x = StartX + SquareWidth * state.cursor.second;
    double y = StartY + SquareHeight * state.cursor.first;
    double px = x + SquareWidth;
    double py = y + SquareHeight;
    glBegin(GL_LINE_LOOP);
    glVertex3d(x, y, 0);
    glVertex3d(px, y, 0);
    glVertex3d(px, py, 0);
    glVertex3d(x, py, 0);
    glEnd();
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
  else if (state.gameState == GameStartPlayingTransition) {
    display_GameStart();
    display_Playing();
  }
  else if (state.gameState == GamePlaying) {
    display_Playing();
  }

  state.time += Interval;
  if (state.time > 3000) {
    state.time %= 1'000'000'000'000'000'000LL;  // 1e18
    state.time += 3000;
  }

  printf("\r%lld", state.time);

  glutSwapBuffers();
}

void handleKeyboard(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      state.key = Key_Up;
      state.cursor.first--;
      break;
    case GLUT_KEY_DOWN:
      state.key = Key_Down;
      state.cursor.first++;
      break;
    case GLUT_KEY_LEFT:
      state.key = Key_Left;
      state.cursor.second--;
      break;
    case GLUT_KEY_RIGHT:
      state.key = Key_Right;
      state.cursor.second++;
      break;
    case 32: state.key = Key_Space; break;
    case 13: state.key = Key_Enter; break;
  }
  state.cursor.first = max(0, min(state.cursor.first, (int) GridHeight - 1));
  state.cursor.second = max(0, min(state.cursor.second, (int) GridWidth - 1));
}
void handleKeyboardUp(int key, int x, int y) { state.key = Key_Nothing; }
void handleKeyboard(unsigned char key, int x, int y) { handleKeyboard((int) key, x, y); }
void handleKeyboardUp(unsigned char key, int x, int y) { handleKeyboardUp((int) key, x, y); }

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
  glutIgnoreKeyRepeat(GL_TRUE);

  // Temporary
  state.gameState = GamePlaying;

  glutDisplayFunc(display);
  glutKeyboardFunc(handleKeyboard);
  glutKeyboardUpFunc(handleKeyboardUp);
  glutSpecialFunc(handleKeyboard);
  glutSpecialUpFunc(handleKeyboardUp);
  glutTimerFunc(Interval, timer, 10);
  glutMainLoop();
  return 0;
}