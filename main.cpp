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

// Prototypes
void clean_up();
void info();
void quit_game();
void start_game();

// Constantes
const double PERSPECTIVE_ANGLE = 45.0;
const double CAMERA_ROTATION_ANGLE = 0.0349066; // Ângulo de rotação de 2 graus em radianos
const double timeout = 150.0f; // Máximo de tempo para terminar o jogo

// Globais
clock_t c_start;
double last_second = 0;
double seconds = 0.0f;

double window_width;
double window_height;
long long n_changed_direction;
GLuint window;
GLuint info_window;
GLuint final_window;
GLuint path_window;
Matrix<Face> *surface;
Camera *camera;
std::vector<Enemy> enemies;
std::vector<Point> coordinates;

int n_enemies;
bool end_status;
int enemies_left;
bool walking;
bool rotating_right;
bool rotating_left;

void initialize_global_values() {
    c_start = clock();
    n_changed_direction = 0ll;
    seconds = 0.0f;
    last_second = 0.0f;
    n_enemies = 5 + (rand() % 20);
    enemies_left = n_enemies;
    end_status = false;
    walking = false;
    rotating_right = false;
    rotating_left = false;
    coordinates.clear();
    enemies.clear();
}

void final_normal_keys(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
        if (end_status) {
            clean_up();
            exit(0);
        }
        break;
    case 'r':
        if (end_status) {
            start_game();
        }
    }
}

void init_info() {
    //glEnable(GL_DEPTH_TEST); // Maldita linha que estava faltando.

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
}

void init_objects() {
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

// {{{ Inicialização
void init() {
    glEnable(GL_DEPTH_TEST); // Maldita linha que estava faltando.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
}
// }}}

void walked_path() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, surface->width, surface->height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, surface->width, 0, surface->height, 0, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 500, 0, 0, 0, 0, 1, 0);

    surface->render();

    int tam = coordinates.size();
    for (int i = 0; i < tam - 1; i++) {
        glLineWidth(1.5);
        glColor3f(1, 0.0, 1.0);
        glBegin(GL_LINES);
        glVertex3f(coordinates[i].x, coordinates[i].y, 400);
        glVertex3f(coordinates[i + 1].x, coordinates[i + 1].y, 400);
        glEnd();
    }

    glutSwapBuffers();
}

void clean_up() {
    //for (auto &p : coordinates) {
    //std::cout << p.to_string() << '\n';
    //}
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
        if (e.is_touching(camera) && e.alive) {
            e.die();
            --enemies_left;
            if (enemies_left == 0) {
                quit_game();
            }
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
    if (seconds >= timeout) {
        quit_game();
    } else if (!end_status) {
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

        glutSetWindow(path_window);
        walked_path();
    }
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

// {{{ Quit game
void quit_game() {
    end_status = true;
    glutDestroyWindow(window);
    glutSetWindow(info_window);
}
// }}}

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
        quit_game();
        break;
    }
    glutPostRedisplay();
}
// }}}


// {{{ Display Text
void displayText(float x, float y, int r, int g, int b, const char *string) {
    int j = strlen(string);

    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (int i = 0; i < j; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
    }
}
// }}}


// {{{ Info
void info() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int str_len{ 20 };
    char str[str_len];
    seconds = ((double)(clock() - c_start)) / CLOCKS_PER_SEC;

    if (seconds - last_second > 0.1) {
        last_second = seconds;
        coordinates.push_back(Point(camera->atx, camera->aty, camera->atz));
    }

    // Número de objetos
    displayText(-0.95, 0.90, 0, 0, 0, "Total de objetos: ");
    snprintf(str, str_len, "%d", n_enemies);
    displayText(0.20, 0.90, 0, 0, 0, str);

    // Objetos restantes
    displayText(-0.95, 0.83, 0, 0, 0, "Objetos restantes: ");
    snprintf(str, str_len, "%d", enemies_left);
    displayText(0.20, 0.83, 0, 0, 0, str);

    // Objetos capturados
    displayText(-0.95, 0.76, 0, 0, 0, "Objetos adquiridos: ");
    snprintf(str, str_len, "%d", n_enemies - enemies_left);
    displayText(0.20, 0.76, 0, 0, 0, str);

    //direcao
    displayText(-0.95, 0.69, 0, 0, 0, "Mudancas de direcao: ");
    snprintf(str, str_len, "%lld", n_changed_direction);
    displayText(0.20, 0.69, 0, 0, 0, str);

    //tempo
    displayText(-0.95, 0.62, 0, 0, 0, "Tempo decorrido: ");
    snprintf(str, str_len, "%.0lf", seconds);
    int i = 0;
    while (str[i] != '\0') {
        ++i;
    }
    str[i] = 's';
    str[i + 1] = '\0';
    displayText(0.20, 0.62, 0, 0, 0, str);

    glutSwapBuffers();
}
// }}}

void start_game() {
    initialize_global_values();
    init_objects();

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Info Window
    glutInitWindowPosition(900, 40);
    glutInitWindowSize(300, 300);
    info_window = glutCreateWindow("Info");
    glutDisplayFunc(info);
    glutKeyboardFunc(final_normal_keys);
    init_info();

    // Walked Path Window
    glutInitWindowPosition(900, 400);
    glutInitWindowSize(surface->width, surface->height);
    path_window = glutCreateWindow("Walked Path");
    glutKeyboardFunc(final_normal_keys);
    glutDisplayFunc(walked_path);
    init_info();

    // Main Window
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(20, 40);
    window = glutCreateWindow("Main Window");
    glutDisplayFunc(render_scene);
    glutReshapeFunc(world_reshape);
    glutSpecialFunc(special_keys);
    glutSpecialUpFunc(special_keys_up);
    glutKeyboardFunc(normal_keys);
    glutIdleFunc(render_idle_scene);
    init();
}

// {{{ Main
int main(int argc, char **argv) {
    std::srand(std::time(0));
    glutInit(&argc, argv);
    start_game();
    glutMainLoop();
    return 0;
}
// }}}
