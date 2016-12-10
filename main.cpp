// vim: set foldmethod=marker

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif

#include "objects.hpp"

// Constantes
//const double PERSPECTIVE_ANGLE = 100.0;
const double PERSPECTIVE_ANGLE = 45.0;
//const double CAMERA_ROTATION_ANGLE = 0.174533; // Ângulo de rotação de 10 graus em radianos
const double CAMERA_ROTATION_ANGLE = 0.0349066; // Ângulo de rotação de 2 graus em radianos

// Globais
GLuint window;
GLuint mini_map;
Matrix<Face> *surface;
Camera *camera;

bool walking = false;
bool rotating_right = false;
bool rotating_left = false;

// {{{ Inicialização
void init() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco

#ifdef __APPLE__
    surface = read_image();
#else
    read_file(&surface, "matrix.txt");
#endif

    camera = new Camera(0, 0, 4, 200, 200, 10);
    camera->set_matrix(surface);
}
// }}}

double window_width;
double window_height;

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

    if (walking) {
        camera->move_forward();
    }

    // Mini map
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
    double minimap_width = window_width / 4;
    double minimap_height = window_height / 4;
    glViewport(window_width - minimap_width, window_height - minimap_height, minimap_width, minimap_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(camera->atx, camera->aty, 90,
            camera->atx, camera->aty, camera->atz, 0, 1, 0);

    surface->render();
    camera->render();
    glutSwapBuffers();
}
// }}}

// {{{
void render_idle_scene() {
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

    if (walking) {
        camera->move_forward();
    }

    if (rotating_right) {
        camera->rotate(-CAMERA_ROTATION_ANGLE);
    } else if (rotating_left) {
        camera->rotate(CAMERA_ROTATION_ANGLE);
    }

    // Mini map
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
    double minimap_width = window_width / 4;
    double minimap_height = window_height / 4;
    glViewport(window_width - minimap_width, window_height - minimap_height, minimap_width, minimap_height);
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
    glutSwapBuffers();
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
            rotating_right = false;
            break;

        case GLUT_KEY_LEFT:
            rotating_left = false;
            break;
    }
    glutPostRedisplay();
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
    }
    glutPostRedisplay();
}
// }}}

// {{{ Main
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 600);
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

    delete camera;
    delete surface;

    return 0;
}
// }}}
