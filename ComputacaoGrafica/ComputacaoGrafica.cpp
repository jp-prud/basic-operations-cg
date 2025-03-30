#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <tuple>
#include <numbers>

using Vertice = std::tuple<double, double, double>;
using Escala = std::tuple<double, double, double>;
using Rotacao = std::tuple<double, double, double>;
using ListaVertices = std::vector<Vertice>;
using Aresta = std::pair<int, int>;
using Posicao = std::pair<int, int>;
using ListaArestas = std::vector<Aresta>;

struct Cubo {
    Vertice posicao;
    Escala escala;
    Rotacao rotacao;
    ListaVertices vertices;
    ListaArestas arestas;
};

struct Mouse {
  Posicao posicao = { -1, -1 };
  int botao = -1;
  float sense = .4;
};

Cubo criar_cubo(double x, double y, double z, double tamanho);
void desenhar(const Cubo& cubo);
void display();
void keyboard(unsigned char key, int x, int y);
void mouse_move(int x, int y);
void mouse_click(int button, int state, int x, int y);
void redraw(int value);

Cubo cubo;
int delay = 10;
Mouse mouse;

int main(int argc, char** argv) {
    glutInitWindowSize(512, 512);

    cubo = criar_cubo(0, 0, 0, 50);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Cubo Wireframe");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glOrtho(-100, 100, -100, 100, -100, 100);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(mouse_move);
    glutMouseFunc(mouse_click);

    glutTimerFunc(delay, redraw, 0);
    
    glutMainLoop();
    return 0;
}

Cubo criar_cubo(double x, double y, double z, double tamanho) {
    Cubo novo_cubo;
    novo_cubo.posicao = { x, y, z };
    novo_cubo.escala = { 1, 1, 1 };
    novo_cubo.rotacao = { 0, 0, 0 };

    double t = tamanho / 2.0;
    novo_cubo.vertices = {
        {-t, -t, -t}, { t, -t, -t}, { t,  t, -t}, {-t,  t, -t},
        {-t, -t,  t}, { t, -t,  t}, { t,  t,  t}, {-t,  t,  t}
    };

    novo_cubo.arestas = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };
    return novo_cubo;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    desenhar(cubo);
    glFlush();
}

void redraw(int value) {
    glutPostRedisplay();
    glutTimerFunc(delay, redraw, 0);
}

void desenhar(const Cubo& cubo) {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    for (const Aresta& Aresta : cubo.arestas) {
        const Vertice& v1 = cubo.vertices[Aresta.first];
        const Vertice& v2 = cubo.vertices[Aresta.second];
        double x1, y1, z1, x2, y2, z2;
        std::tie(x1, y1, z1) = v1;
        std::tie(x2, y2, z2) = v2;
        glVertex3f(x1, y1, z1);
        glVertex3f(x2, y2, z2);
    }
    glEnd();
}

void movimentar(Cubo& cubo, double dx, double dy, double dz) {
    std::get<0>(cubo.posicao) += dx;
    std::get<1>(cubo.posicao) += dy;
    std::get<2>(cubo.posicao) += dz;
    for (auto& v : cubo.vertices) {
        std::get<0>(v) += dx;
        std::get<1>(v) += dy;
        std::get<2>(v) += dz;
    }
}

void escalar(Cubo& cubo, double sx, double sy, double sz) {
    for (auto& v : cubo.vertices) {
        std::get<0>(v) *= sx;
        std::get<1>(v) *= sy;
        std::get<2>(v) *= sz;
    }
}

void rotacionar(Cubo& cubo, double anguloX, double anguloY, double anguloZ) {
    const double radX = anguloX * std::numbers::pi / 180.0;
    const double radY = anguloY * std::numbers::pi / 180.0;
    const double radZ = anguloZ * std::numbers::pi / 180.0;

    const double centerX = std::get<0>(cubo.posicao);
    const double centerY = std::get<1>(cubo.posicao);
    const double centerZ = std::get<2>(cubo.posicao);

    for (Vertice& vertice : cubo.vertices) {
      double x = std::get<0>(vertice) - centerX;
      double y = std::get<1>(vertice) - centerY;
      double z = std::get<2>(vertice) - centerZ;

      // Rotação em X
      double novoY = y * cos(anguloX) - z * sin(anguloX);
      double novoZ = y * sin(anguloX) + z * cos(anguloX);
      y = novoY;
      z = novoZ;

      // Rotação em Y
      double novoX = x * cos(anguloY) + z * sin(anguloY);
      z = -x * sin(anguloY) + z * cos(anguloY);
      x = novoX;

      // Rotação em Z
      double novoX2 = x * cos(anguloZ) - y * sin(anguloZ);
      double novoY2 = x * sin(anguloZ) + y * cos(anguloZ);
      x = novoX2;
      y = novoY2;

      // Translação de volta (somando as coordenadas do centro)
      x += centerX;
      y += centerY;
      z += centerZ;

      // Atualiza as coordenadas
      std::get<0>(vertice) = x;
      std::get<1>(vertice) = y;
      std::get<2>(vertice) = z;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+': escalar(cubo, 1.1, 1.1, 1.1); break;
        case '-': escalar(cubo, 0.9, 0.9, 0.9); break;
        case 'r': rotacionar(cubo, 0, 0, .1); break;
        case ' ': exit(0); break;
    }
}

void mouse_click(int button, int state, int x, int y) {
  mouse.botao = button;
  mouse.posicao = { x, y };
}

void mouse_move(int x, int y) {
    if (mouse.posicao.first < 0 || mouse.posicao.second < 0)
      mouse.posicao = { x, y };

    if (mouse.botao == GLUT_LEFT_BUTTON) {
      const double offsetX = (x - mouse.posicao.first) * mouse.sense;
      const double offsetY = (mouse.posicao.second - y) * mouse.sense;

      movimentar(cubo, offsetX, offsetY, 0);
    }
    else if (mouse.botao == GLUT_RIGHT_BUTTON) {
      const double offsetX = x - mouse.posicao.first;
      const double offsetY = y - mouse.posicao.second;
      const double grauX = offsetX / 180 * -1;
      const double grauY = offsetY / 180 * -1;

      rotacionar(cubo, grauY, grauX, 0);
    }

    mouse.posicao = { x, y };
}