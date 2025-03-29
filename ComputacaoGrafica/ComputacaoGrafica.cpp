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

Cubo criar_cubo(double x, double y, double z, double tamanho);
void desenhar(const Cubo& cubo);
void display();
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void mouse_move(int x, int y);
void redraw(int value);

Cubo cubo;
int delay = 10;
Posicao posMouse = { -1, -1 };

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
    glutSpecialFunc(keyboard_special);
    glutPassiveMotionFunc(mouse_move);
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
        //std::cout << x1 << "\t" << y1 << "\t" << z1 << "\t--->\t" << x2 << "\t" << y2 << "\t" << z2 << std::endl;
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

void rotacionar(Vertice& posicao, double anguloX, double anguloY, double anguloZ)
{
    double x = std::get<0>(posicao);
    double y = std::get<1>(posicao);
    double z = std::get<2>(posicao);

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

    // Atualiza as coordenadas
    std::get<0>(posicao) = x;
    std::get<1>(posicao) = y;
    std::get<2>(posicao) = z;
}

void rotacionar(Cubo& cubo, double anguloX, double anguloY, double anguloZ) {
    const double radX = anguloX * std::numbers::pi / 180.0;
    const double radY = anguloY * std::numbers::pi / 180.0;
    const double radZ = anguloZ * std::numbers::pi / 180.0;

    rotacionar(cubo.posicao, radX, radY, radZ);

    for (Vertice& v : cubo.vertices) {
        rotacionar(v, radX, radY, radZ);
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ': exit(0); break;
        case 'j': escalar(cubo, 1.1, 1.1, 1.1); break;
        case 'k': escalar(cubo, 0.9, 0.9, 0.9); break;
        case 'a': rotacionar(cubo, 5, 0, 0); break;
        case 's': rotacionar(cubo, 0, 5, 0); break;
        case 'd': rotacionar(cubo, 0, 0, 5); break;
    }
}

void keyboard_special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: movimentar(cubo, 0, 5, 0); break;
        case GLUT_KEY_DOWN: movimentar(cubo, 0, -5, 0); break;
        case GLUT_KEY_LEFT: movimentar(cubo, -5, 0, 0); break;
        case GLUT_KEY_RIGHT: movimentar(cubo, 5, 0, 0); break;
    }
}

void mouse_move(int x, int y) {
    if (posMouse.first < 0 || posMouse.second < 0)
        posMouse = { x, y };

    const double offsetX = (x - posMouse.first) / 180 * -1;
    const double offsetY = (y - posMouse.second) / 180 * -1;

    //rotacionar(cubo, offsetY, offsetX, 0);
    std::cout << "\n\nrotacao\n";
    std::cout << x << "\t" << y << std::endl;
    std::cout << offsetX << "\t" << offsetY << std::endl;
}