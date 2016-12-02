// vim: set foldmethod=marker
#include <GLUT/glut.h>
#include <opencv2/opencv.hpp>

#include "objects.hpp"

// Constantes
//const double PERSPECTIVE_ANGLE = 100.0;
const double PERSPECTIVE_ANGLE = 45.0;

// Globais
GLuint window;
Matrix<Face> *surface;
Camera *camera;

// {{{ Inicialização
void init() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fundo branco

    surface = read_image();
    //camera = new Camera(500, 500, 200, 0, 0, 0);
    camera = new Camera(0, 0, 2, 200, 200, 10);
}
// }}}

// {{{ World Reshape
void world_reshape(GLsizei width, GLsizei height) {
    if (height == 0) {
        ++height;
    }
    glViewport(0, 0, width, height);
    camera->f_aspect = (GLfloat) width / (GLfloat) height;
    //perspective_viewing();
    glutPostRedisplay();
}
// }}}


double rotation_angle = 0.0f;

// {{{ Render Scene
void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PERSPECTIVE_ANGLE, camera->f_aspect, 0.1, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    gluLookAt(camera->atx, camera->aty, camera->atz,
            camera->tox, camera->toy, camera->toz, 0, 0, 1);

    glPushMatrix();

    glRotatef(rotation_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-camera->atx, -camera->aty, 0);

    surface->render();

    glPopMatrix();

    glutSwapBuffers();
}
// }}}

void special_keys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            break;

        case GLUT_KEY_DOWN:
            break;

        case GLUT_KEY_RIGHT:
            rotation_angle += 10.0f;
            break;

        case GLUT_KEY_LEFT:
            rotation_angle -= 10.0f;
            break;
    }
    glutPostRedisplay();
}

// {{{ Main
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    window = glutCreateWindow("Janela Principal");
    glutDisplayFunc(render_scene);
    glutReshapeFunc(world_reshape);
    glutSpecialFunc(special_keys);

    init();

    glutMainLoop();

    delete surface;
    delete camera;

    return 0;
}
// }}}
