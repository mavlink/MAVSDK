#include <cassert>
#include "rounded_corners.hpp"
#include <iostream>
#include <iomanip>

#if VISUALIZE == 1
#include <GL/glut.h>
#include <GL/gl.h>
#endif

using namespace rounded_corners;

void print_points(const std::vector<Point>& points)
{
    for (const auto& point : points) {
        std::cout << "x: " << point.x << ", y: " << point.y << '\n';
    }
}

#if VISUALIZE == 1
void visualize_points(const std::vector<Point>& lines, const std::vector<Point>& sampled)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_LINES);

    for (auto it = lines.begin(); (it+1) != lines.end(); ++it) {
        const auto& line = *it;
        const auto& next = *(it + 1);
        glVertex2i(line.x*100, line.y*100);
        glVertex2i(next.x*100, next.y*100);
    }

    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);

    // Begin the polygon
    glBegin(GL_LINES);
    for (auto it = sampled.begin(); (it+1) != sampled.end(); ++it) {
        const auto& sample = *it;
        const auto& next = *(it + 1);
        glVertex2i(sample.x*100, sample.y*100);
        glVertex2i(next.x*100, next.y*100);
    }
    glEnd();
    glFlush();
    glutSwapBuffers();
    }
#endif

void sample_straight_line()
{
    auto rc = RoundedCorners{1.0};
    const auto line = std::vector<Point>{{2.0, 0.0, 0.0}, {5.0, 0.0, 0.0}};
    rc.set_points(line);
    auto sampled = rc.sample(0.1);

    print_points(sampled);

    assert(sampled.at(0) == line.at(0));
    assert(sampled.at(1) == Point(2.1, 0.0, 0.0));
    assert(sampled.at(14) == Point(3.5, 0.0, 0.0));
    assert(sampled.at(28) == Point(4.9, 0.0, 0.0));
}

void calculate_corner()
{
    auto rc = RoundedCorners{1.0};
    const auto lines = std::vector<Point>{{2.0, 1.0, 0.0}, {5.0, 1.0, 0.0}, {1.0, 7.0, 0.0}, {-5, -1.0, 0.0}};
    rc.set_points(lines);
    auto sampled = rc.sample(0.1);

    print_points(sampled);
#if VISUALIZE==1
    visualize_points(lines, sampled);
#endif
    assert(sampled.size() == 294);
    assert(sampled.at(0) == lines.at(0));
}

#if VISUALIZE == 1
void myInit()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    double w = glutGet( GLUT_WINDOW_WIDTH );
    double h = glutGet( GLUT_WINDOW_HEIGHT );
    double ar = w / h;
    glOrtho( -2 * ar, 2 * ar, -2, 2, -1, 1);

    // Specify the display area
    gluOrtho2D(0.0, 500.0, 0.0, 500.0);
}

void keyboard(unsigned char key, int x, int y)	// Close on key press
{
    if (key) {
        exit(0);
    }
}

int main(int argc, char** argv)
{
    sample_straight_line();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Rounded Corner Test");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glutDisplayFunc(calculate_corner);
    glutKeyboardFunc(keyboard);
    myInit();
    glutMainLoop();

    return 0;
}
#else
int main(int, char**)
{
    calculate_corner();
    sample_straight_line();
    return 0;
}
#endif