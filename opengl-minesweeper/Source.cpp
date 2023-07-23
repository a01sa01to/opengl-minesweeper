#include <algorithm>
#include <array>
#include <bitset>
#include <numeric>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "animation.h"
#include "glut.h"
#include "string_util.h"

using namespace std;
using uint = unsigned int;
using ll = long long;

#define rep(i, n) for (int i = 0; i < (n); ++i)

// Settings
constexpr uint Width = 640;
constexpr uint Height = 480;
constexpr uint Interval = 10;

// Keyboard
constexpr uint KeyBit = 6;
constexpr uint Key_Enter = 0;
constexpr uint Key_Space = 1;
constexpr uint Key_Left = 2;
constexpr uint Key_Up = 3;
constexpr uint Key_Right = 4;
constexpr uint Key_Down = 5;

// Grid
constexpr uint GridWidth = 15;
constexpr uint GridHeight = 10;
constexpr uint GridBit = 4;
constexpr uint BombCount = (GridWidth * GridHeight) * 20 / 100;  // 20%
constexpr uint Grid_Open = 0;
constexpr uint Grid_Question = 1;
constexpr uint Grid_Flag = 2;
constexpr uint Grid_Bomb = 3;

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
  bitset<KeyBit> key = 0;
  array<array<bitset<GridBit>, GridWidth>, GridHeight> grid;
  pair<int, int> cursor = { 0, 0 };
  tuple<double, double, double> cameraTo = { 0, 0, 100 };
  tuple<double, double, double> cameraNow = { 0, 0, 100 };
  bool isGridInitialized = false;
};
State state = State();

template<typename T>
inline T sign(T x) {
  if (x == 0) return 0;
  return x / abs(x);
}

void timer(int t) {
  glutPostRedisplay();
  glutTimerFunc(Interval, timer, 20);
}

void display_GameStart() {
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
    drawString("Press Enter to Start", -size / 2.0, -20, 0, size, 1);
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
    drawString("(c) 2023 Asa", -size / 2.0, -30, 0, size, 1);
  }
  glPopMatrix();

  // Press Enter
  if (state.time > 3000 && state.key.test(Key_Enter)) {
    state.gameState = GameStartPlayingTransition;
    state.time = 0;
  }
}

void display_Playing() {
  if (state.gameState == GameStartPlayingTransition && state.time < 750) return;
  if (state.gameState == GameStartPlayingTransition) {
    state.gameState = GamePlaying;
    rep(i, GridHeight) rep(j, GridWidth) state.grid[i][j] = bitset<GridBit>(0);
  }
  if (state.key.test(Key_Enter) && !state.isGridInitialized) {
    state.isGridInitialized = true;
    rep(i, GridHeight) rep(j, GridWidth) {
      state.grid[i][j].reset(Grid_Flag);
      state.grid[i][j].reset(Grid_Question);
    }
    pair<int, int> opened = state.cursor;
    vector<pair<int, int>> bombs;
    rep(i, GridHeight) rep(j, GridWidth) {
      int diff = abs(i - opened.first) + abs(j - opened.second);
      if (diff >= 3) bombs.emplace_back(i, j);
    }
    random_device seed_gen;
    mt19937 engine(seed_gen());
    shuffle(bombs.begin(), bombs.end(), engine);
    while (bombs.size() > BombCount) bombs.pop_back();
    for (auto&& [i, j] : bombs) state.grid[i][j].set(Grid_Bomb);
  }

  constexpr int StartX = -50;
  constexpr int StartY = 40;
  constexpr int EndX = 50;
  constexpr int EndY = -30;
  constexpr double SquareWidth = (double) (EndX - StartX) / GridWidth;
  constexpr double SquareHeight = (double) (EndY - StartY) / GridHeight;
  constexpr double CharWidth = SquareWidth / 1.5;

  // Draw Grid
  rep(i, GridHeight) rep(j, GridWidth) {
    double x = StartX + SquareWidth * j;
    double y = StartY + SquareHeight * i;
    if (!state.grid[i][j].test(Grid_Open)) {
      glPushMatrix();
      {
        glColor3d(0.5, 0.5, 0.5);
        glTranslated(x, y, 0);
        glTranslated(SquareWidth / 2, SquareHeight / 2, 0);
        glScaled(1, (double) SquareHeight / SquareWidth, 1.0 / SquareWidth);
        glutSolidCube(SquareWidth);
      }
      glPopMatrix();
      if (!state.isGridInitialized) continue;
      if (state.grid[i][j].test(Grid_Question)) {
        glPushMatrix();
        glColor3d(0, 0, 1);
        glTranslated(SquareWidth / 4, -SquareHeight / 5, 0);
        drawMonoString("?", x, y + SquareHeight, 0, CharWidth, 2);
        glPopMatrix();
      }
      else if (state.grid[i][j].test(Grid_Flag)) {
        glPushMatrix();
        glColor3d(1, 0, 0);
        glTranslated(SquareWidth / 2.5, -SquareHeight / 5, 0);
        drawMonoString("!", x, y + SquareHeight, 0, CharWidth, 2);
        glPopMatrix();
      }

      // Debug
      if (state.grid[i][j].test(Grid_Bomb)) {
        glPushMatrix();
        glColor3d(0, 0, 1);
        glTranslated(SquareWidth / 4, -SquareHeight / 5, 0);
        drawMonoString("B", x, y + SquareHeight, 0, CharWidth, 2);
        glPopMatrix();
      }
    }
    else {
    }
  }

  // Draw Grid Lines
  glPushMatrix();
  {
    glLineWidth(1);
    glColor3d(0.3, 0.3, 0.3);
    rep(i, GridWidth + 1) {
      double x = StartX + SquareWidth * i;
      glBegin(GL_LINES), glVertex3d(x, StartY, 0), glVertex3d(x, EndY, 0), glEnd();
    }
    rep(i, GridHeight + 1) {
      double y = StartY + SquareHeight * i;
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

  // Camera
  if (state.key.test(Key_Space)) {
    double x = StartX + SquareWidth * state.cursor.second + SquareWidth / 2;
    double y = StartY + SquareHeight * state.cursor.first + SquareHeight / 2;
    state.cameraTo = make_tuple(x, y, 30);
  }
}

void display() {
  // Initialize
  glutReshapeWindow(Width, Height);
  glViewport(0, 0, Width, Height);

  // Projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50, (double) Width / Height, 1, 1000);

  // Model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  {
    auto&& [tox, toy, toz] = state.cameraTo;
    auto&& [nowx, nowy, nowz] = state.cameraNow;
    auto [diffx, diffy, diffz] = make_tuple(tox - nowx, toy - nowy, toz - nowz);
    double norm = sqrt(diffx * diffx + diffy * diffy + diffz * diffz);
    constexpr int Speed = 7;
    constexpr double Eps = 1e-9;
    if (abs(norm) < Eps) norm = Speed;
    auto [dx, dy, dz] = make_tuple(diffx / norm * Speed, diffy / norm * Speed, diffz / norm * Speed);
    nowx += sign(dx) * min(abs(diffx), abs(dx));
    nowy += sign(dy) * min(abs(diffy), abs(dy));
    nowz += sign(dz) * min(abs(diffz), abs(dz));
    gluLookAt(nowx, nowy, nowz, nowx, nowy, 0, 0, 1, 0);
  }

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

  // Timer
  state.time += Interval;
  if (state.time > 3000) {
    state.time %= 1'000'000'000'000'000'000LL;  // 1e18
    state.time += 3000;
  }

  // Scaling
  if (state.key.none()) {
    state.cameraTo = { 0, 0, 100 };
  }

  glutSwapBuffers();
}

void handleKeyboardSp(int key, int _x, int _y) {
  switch (key) {
    case GLUT_KEY_UP:
      state.key.set(Key_Up);
      state.cursor.first--;
      break;
    case GLUT_KEY_DOWN:
      state.key.set(Key_Down);
      state.cursor.first++;
      break;
    case GLUT_KEY_LEFT:
      state.key.set(Key_Left);
      state.cursor.second--;
      break;
    case GLUT_KEY_RIGHT:
      state.key.set(Key_Right);
      state.cursor.second++;
      break;
  }
  state.cursor.first = max(0, min(state.cursor.first, (int) GridHeight - 1));
  state.cursor.second = max(0, min(state.cursor.second, (int) GridWidth - 1));
}
void handleKeyboardSpUp(int key, int _x, int _y) {
  switch (key) {
    case GLUT_KEY_UP: state.key.reset(Key_Up); break;
    case GLUT_KEY_DOWN: state.key.reset(Key_Down); break;
    case GLUT_KEY_LEFT: state.key.reset(Key_Left); break;
    case GLUT_KEY_RIGHT: state.key.reset(Key_Right); break;
  }
}
void handleKeyboard(unsigned char key, int _x, int _y) {
  auto&& [i, j] = state.cursor;
  switch (key) {
    case 32: state.key.set(Key_Space); break;
    case 13:
      state.grid[i][j].reset(Grid_Question);
      state.grid[i][j].reset(Grid_Flag);
      state.grid[i][j].set(Grid_Open);
      state.key.set(Key_Enter);
      break;
    case 'F':
    case 'f':
      state.grid[i][j].reset(Grid_Question);
      state.grid[i][j].flip(Grid_Flag);
      break;
    case 'Q':
    case 'q':
      state.grid[i][j].reset(Grid_Flag);
      state.grid[i][j].flip(Grid_Question);
      break;
  }
}
void handleKeyboardUp(unsigned char key, int _x, int _y) {
  switch (key) {
    case 32: state.key.reset(Key_Space); break;
    case 13: state.key.reset(Key_Enter); break;
  }
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
  glutIgnoreKeyRepeat(GL_TRUE);

  // Temporary
  state.gameState = GamePlaying;

  glutDisplayFunc(display);
  glutKeyboardFunc(handleKeyboard);
  glutKeyboardUpFunc(handleKeyboardUp);
  glutSpecialFunc(handleKeyboardSp);
  glutSpecialUpFunc(handleKeyboardSpUp);
  glutTimerFunc(Interval, timer, 10);
  glutMainLoop();
  return 0;
}