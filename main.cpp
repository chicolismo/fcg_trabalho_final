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
const double CAMERA_ROTATION_ANGLE = 0.174533; // Ângulo de rotação de 10 graus em radianos

// Globais
GLuint window;
GLuint mini_map;
Matrix<Face> *surface;
Camera *camera;


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

    // Mini map
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco
    //double minimap_width = window_width / 4;
    //double minimap_height = window_height / 4;
    //glViewport(window_width - minimap_width, window_height - minimap_height, minimap_width, minimap_height);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();

    //gluLookAt(camera->atx, camera->aty, camera->atz,
            //camera->tox, camera->toy, camera->toz, 0, 0, 1);

    //surface->render();
    // Fim do mini map

    glutSwapBuffers();
}
// }}}


void special_keys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            camera->move_forward();
            break;

        case GLUT_KEY_DOWN:
            //camera->move_backward();
            break;

        case GLUT_KEY_RIGHT:
            camera->rotate(-CAMERA_ROTATION_ANGLE);
            break;

        case GLUT_KEY_LEFT:
            camera->rotate(CAMERA_ROTATION_ANGLE);
            break;
    }
    glutPostRedisplay();
}

// {{{ Normal keys
void normal_keys(unsigned char key, int x, int y) {
    switch (key) {
        case ']':
            camera->change_velocity(1.0);
            break;
        case '[':
            camera->change_velocity(-1.0);
            break;
    }
    glutPostRedisplay();
}
// }}}

// {{{ Main
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    //window = glutCreateWindow("Janela Principal");
    window = glutCreateWindow("Janela Principal");

    glutDisplayFunc(render_scene);
    glutReshapeFunc(world_reshape);
    glutSpecialFunc(special_keys);
    glutKeyboardFunc(normal_keys);

    init();

    //surface->dump("matrix.txt");
    glutMainLoop();


    delete camera;
    delete surface;

    return 0;
}
// }}}
