#ifndef OBJECTS_HPP_
#define OBJECTS_HPP_

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <gl/glut.h>
#endif
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
//#include <opencv2/opencv.hpp>

class Point {
public:
    float x, y, z;

    Point() : x(0), y(0), z(0) {}

    Point(float x, float y, float z) : x(x), y(y), z(z) {}

    std::string to_string() const {
        std::stringstream fmt;
        fmt << " x: " << x
            << ", y: " << y
            << ", z: " << z;
        return fmt.str();
    }
};

class Face {
public:
    std::vector<Point> vertices;

    Face() : vertices(4) {}

    Face(const Face &other) {
        operator=(other);
    }

    Face(Point a, Point b, Point c, Point d) : Face() {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
        vertices[3] = d;
    }

    Face &operator=(const Face &other) {
        for (int i = 0; i < 4; ++i) {
            vertices[i] = other.vertices[i];
        }
        return *this;
    }

    std::string to_string() const {
        std::stringstream fmt;
        fmt << "\nFace" << '\n';
        for (auto &v : vertices) {
            fmt << v.to_string() << '\n';
        }
        return fmt.str();
    }

    void render() {
        for (auto &v : vertices) {
            glVertex3f(v.x, v.y, v.z);
        }
    }
};

template<typename T>
class Matrix {
private:
    std::vector<T> array;

public:
    size_t width;
    size_t height;

    Matrix() {
    }

    Matrix(size_t width, size_t height) : height(height), width(width), array(width * height) {
    }

    T &at(int i, int j) {
        return array[i * width + j];
    }

    void render() {
        glBegin(GL_QUADS);
        glColor3f(0.5f, 0.3f, 0.4f);
        for (auto &obj : array) {
            obj.render();
        }
        glEnd();

        for (auto &obj : array) {
            glBegin(GL_LINE_LOOP);
            glColor3f(0.f, 0.f, 0.f);
                obj.render();
            glEnd();
        }
    }

    void dump(const std::string &filename) {
        std::ofstream file;
        file.open(filename);
        file << width << '\n';
        file << height << '\n';
        for (Face &f : array) {
            for (Point &p : f.vertices) {
                file << p.x << '\n';
                file << p.y << '\n';
                file << p.z << '\n';
            }
        }
        file.close();
    }

};

template<typename T>
void print_matrix(Matrix<T> &m) {
    const size_t height = m.height;
    const size_t width = m.width;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << m.at(i, j).to_string() << '\n';
        }
    }
}

/* TODO: Incluir de volta quando terminar o trabalho */
/*
Matrix<Face> *read_image() {
    //cv::Mat image = cv::imread("image.jpg");
    //cv::Mat image = cv::imread("south_park.png");
    cv::Mat image = cv::imread("heightdata.png");
    cv::Size size = image.size();

    const int width = size.width - 1;
    const int height = size.height - 1;

    Matrix<Face> *matrix = new Matrix<Face>(width, height);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            cv::Vec3b color_a = image.at<cv::Vec3b>(cv::Point(j, i));
            cv::Vec3b color_b = image.at<cv::Vec3b>(cv::Point(j + 1, i));
            cv::Vec3b color_c = image.at<cv::Vec3b>(cv::Point(j + 1, i + 1));
            cv::Vec3b color_d = image.at<cv::Vec3b>(cv::Point(j, i + 1));

            Face f(Point(j, i, 0.05 * (color_a[0] + color_a[1] + color_a[2]) / 3.0),
                   Point(j + 1, i, 0.05 * (color_b[0] + color_b[1] + color_b[2]) / 3.0),
                   Point(j + 1, i + 1, 0.05 * (color_c[0] + color_c[1] + color_c[2]) / 3.0),
                   Point(j, i + 1, 0.05 * (color_d[0] + color_d[1] + color_d[2]) / 3.0));

            matrix->at(i, j) = f;
        }
    }
    return matrix;
}
*/

void read_file(Matrix<Face> **matrix, const std::string &filename) {
    std::ifstream file;
    size_t width, height;
    file.open(filename);
    file >> width;
    file >> height;
    *matrix = new Matrix<Face>(width, height);

    double x, y, z;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file >> x;
            file >> y;
            file >> z;
            Point a(x, y, z);

            file >> x;
            file >> y;
            file >> z;
            Point b(x, y, z);

            file >> x;
            file >> y;
            file >> z;
            Point c(x, y, z);

            file >> x;
            file >> y;
            file >> z;
            Point d(x, y, z);

            Face f(a, b, c, d);
            (*matrix)->at(i, j) = (f);
        }
    }
    file.close();
}

class Camera {
public:
    double velocity = 1.0;

    const double min_velocity = 1.0;
    const double max_velocity = 10;

    GLfloat f_aspect;
    GLdouble atx, aty, atz, tox, toy, toz;

    Matrix<Face> *matrix;

    Camera(GLdouble atx, GLdouble aty, GLdouble atz, GLdouble tox, GLdouble toy, GLdouble toz) :
        atx(atx), aty(aty), atz(atz), tox(tox), toy(toy), toz(toz) {}

    void rotate(double angle) {
        const double sin_theta = std::sin(angle);
        const double cos_theta = std::cos(angle);
        tox = tox * cos_theta - toy * sin_theta;
        toy = tox * sin_theta + toy * cos_theta;
    }

    void set_matrix(Matrix<Face> *matrix) {
        this->matrix = matrix;
    }

    void move_forward() {
        const double x_vector = tox - atx;
        const double y_vector = toy - aty;
        const double norm = std::sqrt(std::pow(x_vector, 2.0) + std::pow(y_vector, 2.0));
        const double normalized_vector_x = (x_vector / norm) * velocity;
        const double normalized_vector_y = (y_vector / norm) * velocity;

        /*
         * TODO: Antes de mover, verificar se a soma to vetor normalizado com a posição atual
         * se encontra dentro do mapa, caso contrário, não mover a câmera.
         */

        const double new_atx = atx + normalized_vector_x;
        const double new_aty = aty + normalized_vector_y;
        const double new_tox = tox + normalized_vector_x;
        const double new_toy = toy + normalized_vector_y;

        if (new_atx <= matrix->width && new_atx >= 0 && new_aty <= matrix->height && new_aty >= 0) {
            atx = new_atx;
            aty = new_aty;
            tox = new_tox;
            toy = new_toy;

            int j = (int) (std::floor(atx));
            int i = (int) (std::floor(aty));
            const Face &f = matrix->at(i, j);
            double z_sum = 0.0;
            for (const Point &v : f.vertices) {
                z_sum += v.z;
            }
            z_sum /= 4;
            atz = z_sum + 4;
        }
    }

    void change_velocity(const double amount) {
        velocity += amount;
        if (velocity > max_velocity) {
            velocity = max_velocity;
        } else if (velocity < min_velocity) {
            velocity = min_velocity;
        }
    }
};


#endif // OBJECTS_HPP_
