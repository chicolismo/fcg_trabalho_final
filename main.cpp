// vim: set foldmethod=marker

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#include <windows.h>
#endif

#include "objects.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>

// Constantes
//const double PERSPECTIVE_ANGLE = 100.0;
const double PERSPECTIVE_ANGLE = 45.0;
//const double CAMERA_ROTATION_ANGLE = 0.174533; // Ângulo de rotação de 10 graus em radianos
const double CAMERA_ROTATION_ANGLE = 0.0349066; // Ângulo de rotação de 2 graus em radianos

// Globais
double window_width;
double window_height;
long long n_changed_direction = 0;
GLuint window;
GLuint info_window;
Matrix<Face> *surface;
Camera *camera;
int n_enemies = 15;
int enemies_left = n_enemies;
std::vector<Enemy> enemies;
std::vector<Point> coordinates;

bool walking = false;
bool rotating_right = false;
bool rotating_left = false;

void info();

void init_info() {
    //glEnable(GL_DEPTH_TEST); // Maldita linha que estava faltando.

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
}

// {{{ Inicialização
void init() {
    glEnable(GL_DEPTH_TEST); // Maldita linha que estava faltando.

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco

    #ifdef __APPLE__
    surface = read_image();
    #else
    read_file(&surface, "matrix.txt");
    #endif

    int w = surface->width;
    int h = surface->height;
    enemies.reserve(n_enemies);
    for (int i = 0; i < n_enemies; ++i) {
        enemies.push_back(Enemy(surface, std::rand() % w, std::rand() % h, 0));
    }

    camera = new Camera(0, 0, 4, 200, 200, 10);
    camera->set_matrix(surface);
}
// }}}

void clean_up() {
    for (auto &p : coordinates) {
        std::cout << p.to_string() << '\n';
    }
    delete surface;
    delete camera;
}

// {{{ World Reshape
void world_reshape(GLsizei width, GLsizei height) {
    if (height == 0) {
        ++height;
    }
    window_height = height;
    window_width = width;
    camera->f_aspect = (GLfloat) width / (GLfloat) height;
    glutPostRedisplay();
}
// }}}

void mini_map_reshape(GLsizei width, GLsizei height) {
    if (height == 0) {
        ++height;
    }
    glViewport(0, 0, width, height);
    camera->f_aspect = (GLfloat) width / (GLfloat) height;
    glutPostRedisplay();
}

void test_colisions() {
    for (Enemy &e : enemies) {
        float d = e.distance_from(camera->atx, camera->aty, camera->atz);
        if (e.is_touching(camera)) {
            e.die();
            --enemies_left;
        }
    }
}

// {{{ Render Scene
void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera->atx, camera->aty, camera->atz,
              camera->tox, camera->toy, camera->toz, 0, 0, 1);
    surface->render();

    for (auto &e : enemies) {
        e.render();
        e.move();
    }

    if (walking) {
        camera->move_forward();
    }

    // Mini map
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
    double minimap_width = window_width / 4;
    double minimap_height = window_height / 4;
    glViewport(window_width - minimap_width, window_height - minimap_height, minimap_width,
               minimap_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(camera->atx, camera->aty, 90,
              camera->atx, camera->aty, camera->atz, 0, 1, 0);

    surface->render();
    camera->render();
    for (auto &e : enemies) {
        e.render();
    }


    test_colisions();

    glutSwapBuffers();
}
// }}}

// {{{
void render_idle_scene() {
    glutSetWindow(window);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera->atx, camera->aty, camera->atz,
              camera->tox, camera->toy, camera->toz, 0, 0, 1);
    surface->render();

    for (auto &e : enemies) {
        e.render();
        e.move();
    }

    if (walking) {
        camera->move_forward();
    }

    if (rotating_right) {
        camera->rotate(-CAMERA_ROTATION_ANGLE);
    }
    else if (rotating_left) {
        camera->rotate(CAMERA_ROTATION_ANGLE);
    }

    // Mini map
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
    double minimap_width = window_width / 4;
    double minimap_height = window_height / 4;
    glViewport(window_width - minimap_width, window_height - minimap_height, minimap_width,
               minimap_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //gluLookAt(camera->atx, camera->aty, camera->atz,
    //camera->tox, camera->toy, camera->toz, 0, 0, 1);

    gluLookAt(camera->atx, camera->aty, 90,
              camera->atx, camera->aty, camera->atz, 0, 1, 0);

    surface->render();
    camera->render();
    for (auto &e : enemies) {
        e.render();
    }

    test_colisions();

    glutSwapBuffers();
    glutSetWindow(info_window);
    info();
}
// }}}

void special_keys_up(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        walking = false;
        break;

    case GLUT_KEY_DOWN:
        //camera->move_backward();
        break;

    case GLUT_KEY_RIGHT:
        ++n_changed_direction;
        rotating_right = false;
        break;

    case GLUT_KEY_LEFT:
        ++n_changed_direction;
        rotating_left = false;
        break;
    }
    glutPostRedisplay();
}

void info_special_keys_up(int key, int x, int y) {
}

void special_keys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        walking = true;
        break;

    case GLUT_KEY_DOWN:
        //camera->move_backward();
        break;

    case GLUT_KEY_RIGHT:
        rotating_right = true;
        break;

    case GLUT_KEY_LEFT:
        rotating_left = true;
        break;
    }
    glutPostRedisplay();
}

void info_special_keys(int key, int x, int y) {
}

// {{{ Normal keys
void normal_keys(unsigned char key, int x, int y) {
    switch (key) {
    case ']':
        camera->change_velocity(0.5);
        break;
    case '[':
        camera->change_velocity(-0.5);
        break;
    case '1':
        camera->atz -= 10;
        break;
    case '2':
        camera->atz += 10;
        break;

    case 'q':
        clean_up();
        exit(0);
        break;
    }
    glutPostRedisplay();
}
// }}}

void info_normal_keys(unsigned char key, int x, int y) {
}

void displayText(float x, float y, int r, int g, int b, const char *string) {
    int j = strlen(string);

    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; i < j; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
    }
}


clock_t c_start;
long last_second = 0;
void info() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    char str[10];
    long seconds = (clock() - c_start) / CLOCKS_PER_SEC;

    if (seconds != last_second) {
        last_second = seconds;
        coordinates.push_back(Point(camera->atx, camera->aty, camera->atz));
    }

    displayText(-0.95, 0.90, 0, 0, 0, "Objetos restantes: -");
    displayText(-0.95, 0.83, 0, 0, 0, "Objetos adquiridos: -");

    //direcao
    displayText(-0.95, 0.76, 0, 0, 0, "Mudancas de direcao: -");
    snprintf(str, 10, "%lld", n_changed_direction);
    displayText(0.20, 0.76, 0, 0, 0, str);

    //tempo
    displayText(-0.95, 0.62, 0, 0, 0, "Tempo decorrido: -");
    snprintf(str, 10, "%ld", seconds);
    displayText(0.20, 0.62, 0, 0, 0, str);

    glutSwapBuffers();
}


// {{{ Main
int main(int argc, char **argv) {
    c_start = clock();
    std::srand(std::time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(900, 40);
    glutInitWindowSize(300, 300);
    info_window = glutCreateWindow("Estatísticas");
    glutDisplayFunc(info);
    //glutIdleFunc(info);
    init_info();

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(20, 40);
    window = glutCreateWindow("Janela Principal");

    glutDisplayFunc(render_scene);
    glutReshapeFunc(world_reshape);
    glutSpecialFunc(special_keys);
    glutSpecialUpFunc(special_keys_up);
    glutKeyboardFunc(normal_keys);
    glutIdleFunc(render_idle_scene);
    init();

    //surface->dump("matrix.txt");
    glutMainLoop();

    return 0;
}
// }}}
