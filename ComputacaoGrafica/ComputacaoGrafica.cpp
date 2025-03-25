#include <iostream>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

using vertice = std::tuple<double, double, double>;
using lista_vertices = std::vector<vertice>;
using aresta = std::pair<int, int>;
using lista_arestas = std::vector<aresta>;

struct Cubo {
    vertice posicao;
    vertice escala;
    double rotacaoX, rotacaoY;
    lista_vertices vertices;
    lista_arestas arestas;
};

Cubo criar_cubo(double x, double y, double z, double tamanho);
void desenhar(Cubo cubo);
void movimentar(Cubo& cubo, double dx, double dy, double dz);
void escalar(Cubo& cubo, double sx, double sy, double sz);
void rotacionar(Cubo& cubo, double anguloX, double anguloY);
void display();
void keyboard(unsigned char key, int x, int y);
void keyboard_special(int key, int x, int y);
void redraw(int value);

Cubo cubo;
int delay = 10;

glutInitWindowSize(512, 512);
int main(int argc, char** argv) {
    cubo = criar_cubo(0, 0, 0, 50);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Cubo Wireframe");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glOrtho(-100, 100, -100, 100, -100, 100);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutTimerFunc(delay, redraw, 0);
    
    glutMainLoop();
    return 0;
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

Cubo criar_cubo(double x, double y, double z, double tamanho) {
    Cubo novo_cubo;
    novo_cubo.posicao = { x, y, z };
    novo_cubo.escala = { 1, 1, 1 };
    novo_cubo.rotacaoX = 0;
    novo_cubo.rotacaoY = 0;
    
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

void desenhar(Cubo cubo) {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    for (auto& aresta : cubo.arestas) {
        auto [x1, y1, z1] = cubo.vertices[aresta.first];
        auto [x2, y2, z2] = cubo.vertices[aresta.second];
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

void rotacionar(Cubo& cubo, double anguloX, double anguloY) {
    double radX = anguloX * 3.1415926536 / 180.0;
    double radY = anguloY * 3.1415926536 / 180.0;
    for (auto& v : cubo.vertices) {
        double x = std::get<0>(v);
        double y = std::get<1>(v);
        double z = std::get<2>(v);
        
        double novoY = y * cos(radX) - z * sin(radX);
        double novoZ = y * sin(radX) + z * cos(radX);
        std::get<1>(v) = novoY;
        std::get<2>(v) = novoZ;
        
        double novoX = x * cos(radY) + novoZ * sin(radY);
        novoZ = -x * sin(radY) + novoZ * cos(radY);
        std::get<0>(v) = novoX;
        std::get<2>(v) = novoZ;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: exit(0); break;
        case 's': escalar(cubo, 1.1, 1.1, 1.1); break;
        case 'a': escalar(cubo, 0.9, 0.9, 0.9); break;
    }
}

void keyboard_special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: movimentar(cubo, 0, 5, 0); break;
        case GLUT_KEY_DOWN: movimentar(cubo, 0, -5, 0); break;
        case GLUT_KEY_LEFT: movimentar(cubo, -5, 0, 0); break;
        case GLUT_KEY_RIGHT: movimentar(cubo, 5, 0, 0); break;
        case GLUT_KEY_PAGE_UP: rotacionar(cubo, 5, 0); break;
        case GLUT_KEY_PAGE_DOWN: rotacionar(cubo, -5, 0); break;
    }
}
