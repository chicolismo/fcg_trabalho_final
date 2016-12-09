#ifndef OBJECTS_HPP_
#define OBJECTS_HPP_

#include <GLUT/glut.h>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>

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
    const size_t width, height;

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

Matrix<Face> *read_image() {
    //cv::Mat image = cv::imread("image.jpg");
    cv::Mat image = cv::imread("south_park.png");
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

            Face f(Point(i, j, 0.05 * (color_a[0] + color_a[1] + color_a[2]) / 3.0),
                   Point(i, j + 1, 0.05 * (color_b[0] + color_b[1] + color_b[2]) / 3.0),
                   Point(i + 1, j + 1, 0.05 * (color_c[0] + color_c[1] + color_c[2]) / 3.0),
                   Point(i + 1, j, 0.05 * (color_d[0] + color_d[1] + color_d[2]) / 3.0));

            matrix->at(i, j) = f;
        }
    }
    return matrix;
}

class Camera {
public:
    GLfloat f_aspect, angle;
    GLdouble atx, aty, atz, tox, toy, toz;

    Camera(GLdouble atx, GLdouble aty, GLdouble atz, GLdouble tox, GLdouble toy, GLdouble toz) :
        atx(atx), aty(aty), atz(atz), tox(tox), toy(toy), toz(toz) {}

    void rotate(double angle) {
        const double sin_theta = std::sin(angle);
        const double cos_theta = std::cos(angle);
        tox = tox * cos_theta - toy * sin_theta;
        toy = tox * sin_theta + toy * cos_theta;
    }

    void move_forward() {
        const double x_vector = tox - atx;
        const double y_vector = toy - aty;
        const double norm = std::sqrt(std::pow(x_vector, 2.0) + std::pow(y_vector, 2.0));
        const double normalized_vector_x = x_vector / norm;
        const double normalized_vector_y = y_vector / norm;

        atx += normalized_vector_x;
        aty += normalized_vector_y;
        tox += normalized_vector_x;
        toy += normalized_vector_y;
    }
};


#endif // OBJECTS_HPP_
