/***
 Assignment-1: Cartoonify
   
 Name: Scott, John
   
 Collaborators (talked with about high-level concepts): Alvarez, Marisa; Zapata, Alejandro
  
 Project Summary: I decided to implement the iterative approach to drawing Bezier curves. I did this by giving some set of control points to my generate_points function where it would then call generate_midpoints, a helper function, to find the midpoints of the control and store those in a vector. generate_points then used generate_midpoints again on the found midpoints to find the midpoints of midpoints and store those points in another vector. Finally, generate_points returns a vector containing the first control point, alternate between the midpoints of the control points and the midpoints of the midpoints, and the last control point. draw_curve would then repeat this process on the newly returned vector as many times as the n-iter parameter was given so that the final vector of points would consist of so many points that the lines between them appear as curves. To determine the points I would use for each facial feacture I put an image of Chris Pratt on a Desmos grid and plotted points around the features. Ran and compiled in Xcode.
 ***/


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <vector>
#include <iostream>

using namespace std;

class Vertex {
    GLfloat x, y;
public:
    Vertex(GLfloat, GLfloat);
    GLfloat get_y() { return y; };
    GLfloat get_x() { return x; };
};
Vertex::Vertex(GLfloat X, GLfloat Y) {
    x = X;
    y = Y;
}

void setup() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

vector<Vertex> generate_midpoints(vector<Vertex> control_points) {
    vector<Vertex> midpoints;

    // Generate points for a given Bezier curve iteration
    for (int p = 0; p < control_points.size() - 1; p++) {
        Vertex v_0 = control_points[p];
        Vertex v_1 = control_points[p+1];
        // Midpoint along the line from v_0 and v_1
        GLfloat y1 = (v_0.get_y() + v_1.get_y()) / 2;
        GLfloat x1 = (v_0.get_x() + v_1.get_x()) / 2;
        // push_back simply adds a new element to the back of your vector (list)
        midpoints.push_back(Vertex(x1, y1));
    }

    return midpoints;
}

vector<Vertex> generate_points(vector<Vertex> control_points) {
    vector<Vertex> points;
    vector<Vertex> midpoints = generate_midpoints(control_points);
    vector<Vertex> midpoints2 = generate_midpoints(midpoints);

    points.push_back(control_points[0]);
    for (int i = 0; i < midpoints.size() - 1; i++){
        points.push_back(midpoints[i]);
        points.push_back(midpoints2[i]);
    }
    points.push_back(midpoints[midpoints.size() - 1]);
    points.push_back(control_points[control_points.size() - 1]);

    return points;
}


void draw_curve(vector<Vertex> control_points, int n_iter) {
    glPointSize(0.0f);
    // Draw a Bezier curve based on the given control points
    for (int i = 0; i < n_iter; i++) {
        control_points = generate_points(control_points);
    }

    glBegin(GL_POINTS);
    for (int i = 0; i < control_points.size() - 1; i++){
        glVertex2f(control_points[i].get_x(), control_points[i].get_y());
    }
    glEnd();

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(int i = 0; i < control_points.size() - 2; i++){
        glVertex2f(control_points[i].get_x(), control_points[i].get_y());
        glVertex2f(control_points[i+1].get_x(), control_points[i+1].get_y());
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Set our color to black (R, G, B)
    glColor3f(0.0f, 0.0f, 0.0f);

    // Right eye
    vector<Vertex> eye_r;
    eye_r.push_back(Vertex(0.29f, 0.05f));
    eye_r.push_back(Vertex(0.25f, 0.04f));
    eye_r.push_back(Vertex(0.2f, 0.04f));
    eye_r.push_back(Vertex(0.15f, 0.05f));
    eye_r.push_back(Vertex(0.1f, 0.06f));
    eye_r.push_back(Vertex(0.15f, 0.10f));
    eye_r.push_back(Vertex(0.2f, 0.095f));
    eye_r.push_back(Vertex(0.25f, 0.08f));
    eye_r.push_back(Vertex(0.29f, 0.05f));
    // Left eye
    vector<Vertex> eye_l;
    eye_l.push_back(Vertex(-0.33f, 0.09f));
    eye_l.push_back(Vertex(-0.3f, 0.08f));
    eye_l.push_back(Vertex(-0.25f, 0.07f));
    eye_l.push_back(Vertex(-0.2f, 0.07f));
    eye_l.push_back(Vertex(-0.15f, 0.08f));
    eye_l.push_back(Vertex(-0.2f, 0.12f));
    eye_l.push_back(Vertex(-0.25f, 0.12f));
    eye_l.push_back(Vertex(-0.3f, 0.11f));
    eye_l.push_back(Vertex(-0.33f, 0.09f));
    // Right iris
    vector<Vertex> iris_r;
    iris_r.push_back(Vertex(0.2f, 0.04f));
    iris_r.push_back(Vertex(0.23f, 0.05f));
    iris_r.push_back(Vertex(0.23f, 0.08f));
    iris_r.push_back(Vertex(0.2f, 0.09f));
    iris_r.push_back(Vertex(0.17f, 0.09f));
    iris_r.push_back(Vertex(0.16f, 0.07f));
    iris_r.push_back(Vertex(0.17f, 0.05f));
    iris_r.push_back(Vertex(0.2f, 0.04f));
    // Left iris
    vector<Vertex> iris_l;
    iris_l.push_back(Vertex(-0.25f, 0.07f));
    iris_l.push_back(Vertex(-0.22f, 0.08f));
    iris_l.push_back(Vertex(-0.21f, 0.1f));
    iris_l.push_back(Vertex(-0.22f, 0.11f));
    iris_l.push_back(Vertex(-0.24f, 0.12f));
    iris_l.push_back(Vertex(-0.28f, 0.11f));
    iris_l.push_back(Vertex(-0.29f, 0.1f));
    iris_l.push_back(Vertex(-0.27f, 0.08f));
    iris_l.push_back(Vertex(-0.25f, 0.07f));
    // Right ear
    vector<Vertex> ear_r;
    ear_r.push_back(Vertex(0.5f, 0.05f));
    ear_r.push_back(Vertex(0.55f, 0.06f));
    ear_r.push_back(Vertex(0.58f, 0.0f));
    ear_r.push_back(Vertex(0.57f, -0.09f));
    ear_r.push_back(Vertex(0.53f, -0.26f));
    ear_r.push_back(Vertex(0.42f, -0.31f));
    // Left ear
    vector<Vertex> ear_l;
    ear_l.push_back(Vertex(-0.56f, 0.13f));
    ear_l.push_back(Vertex(-0.61f, 0.15f));
    ear_l.push_back(Vertex(-0.65f, 0.1f));
    ear_l.push_back(Vertex(-0.66f, 0.0f));
    ear_l.push_back(Vertex(-0.64f, -0.15f));
    ear_l.push_back(Vertex(-0.6f, -0.23f));
    ear_l.push_back(Vertex(-0.53f, -0.26f));
    // Nose
    vector<Vertex> nose;
    nose.push_back(Vertex(-0.13f, -0.11f));
    nose.push_back(Vertex(-0.15f, -0.17f));
    nose.push_back(Vertex(-0.14f, -0.21f));
    nose.push_back(Vertex(-0.07f, -0.2f));
    nose.push_back(Vertex(0.01f, -0.21f));
    nose.push_back(Vertex(0.09f, -0.22f));
    nose.push_back(Vertex(0.11f, -0.19f));
    nose.push_back(Vertex(0.09f, -0.12f));
    // Top mouth
    vector<Vertex> mouth_t;
    mouth_t.push_back(Vertex(-0.23f, -0.38f));
    mouth_t.push_back(Vertex(-0.15f, -0.36f));
    mouth_t.push_back(Vertex(-0.09f, -0.35f));
    mouth_t.push_back(Vertex(-0.03f, -0.36f));
    mouth_t.push_back(Vertex(0.03f, -0.35f));
    mouth_t.push_back(Vertex(0.09f, -0.37f));
    mouth_t.push_back(Vertex(0.15f, -0.38f));
    // Bottom mouth
    vector<Vertex> mouth_b;
    mouth_b.push_back(Vertex(0.15f, -0.38f));
    mouth_b.push_back(Vertex(0.09f, -0.43f));
    mouth_b.push_back(Vertex(0.03f, -0.45f));
    mouth_b.push_back(Vertex(-0.03f, -0.46f));
    mouth_b.push_back(Vertex(-0.09f, -0.44f));
    mouth_b.push_back(Vertex(-0.15f, -0.42f));
    mouth_b.push_back(Vertex(-0.23f, -0.38f));
    // Lips
    vector<Vertex> lips;
    lips.push_back(Vertex(-0.23f, -0.38f));
    lips.push_back(Vertex(-0.09f, -0.38f));
    lips.push_back(Vertex(-0.03f, -0.4f));
    lips.push_back(Vertex(0.02f, -0.39f));
    lips.push_back(Vertex(0.08f, -0.4f));
    lips.push_back(Vertex(0.15f, -0.38f));
    // Chin
    vector<Vertex> chin;
    chin.push_back(Vertex(-0.23f, -0.7f));
    chin.push_back(Vertex(-0.16f, -0.74f));
    chin.push_back(Vertex(-0.1f, -0.76f));
    chin.push_back(Vertex(0.0f, -0.75f));
    chin.push_back(Vertex(0.07f, -0.73f));
    chin.push_back(Vertex(0.14f, -0.69f));
    // Face
    vector<Vertex> face;
    face.push_back(Vertex(0.0f, 0.98f));
    face.push_back(Vertex(-0.13f, 0.95f));
    face.push_back(Vertex(-0.25f, 0.89f));
    face.push_back(Vertex(-0.4f, 0.8f));
    face.push_back(Vertex(-0.5f, 0.66f));
    face.push_back(Vertex(-0.56f, 0.5f));
    face.push_back(Vertex(-0.59f, 0.3f));
    face.push_back(Vertex(-0.56f, 0.13f));
    face.push_back(Vertex(-0.53f, -0.26f));
    face.push_back(Vertex(-0.5f, -0.45f));
    face.push_back(Vertex(-0.4f, -0.6f));
    face.push_back(Vertex(-0.3f, -0.7f));
    face.push_back(Vertex(-0.2f, -0.77f));
    face.push_back(Vertex(-0.1f, -0.79f));
    face.push_back(Vertex(0.0f, -0.8f));
    face.push_back(Vertex(0.02f, -0.79f));
    face.push_back(Vertex(0.05f, -0.78f));
    face.push_back(Vertex(0.1f, -0.76f));
    face.push_back(Vertex(0.2f, -0.68f));
    face.push_back(Vertex(0.3f, -0.6f));
    face.push_back(Vertex(0.4f, -0.4f));
    face.push_back(Vertex(0.42f, -0.31f));
    face.push_back(Vertex(0.5f, 0.05f));
    face.push_back(Vertex(0.54f, 0.2f));
    face.push_back(Vertex(0.55f, 0.36f));
    face.push_back(Vertex(0.52f, 0.5f));
    face.push_back(Vertex(0.48f, 0.66f));
    face.push_back(Vertex(0.38f, 0.8f));
    face.push_back(Vertex(0.25f, 0.89f));
    face.push_back(Vertex(0.13f, 0.95f));
    face.push_back(Vertex(0.0f, 0.98f));
    // Hair'

    vector<Vertex> hair;
    hair.push_back(Vertex(-0.53f, -0.05f));
    hair.push_back(Vertex(-0.52f, 0.05f));
    hair.push_back(Vertex(-0.5f, 0.15f));
    hair.push_back(Vertex(-0.49f, 0.25f));
    hair.push_back(Vertex(-0.46f, 0.35f));
    hair.push_back(Vertex(-0.44f, 0.45f));
    hair.push_back(Vertex(-0.4f, 0.55f));
    hair.push_back(Vertex(-0.23f, 0.72f));
    hair.push_back(Vertex(0.0f, 0.65f));
    hair.push_back(Vertex(0.15f, 0.62f));
    hair.push_back(Vertex(0.3f, 0.64f));
    hair.push_back(Vertex(0.38f, 0.53f));
    hair.push_back(Vertex(0.41f, 0.39f));
    hair.push_back(Vertex(0.45f, 0.3f));
    hair.push_back(Vertex(0.44f, 0.2f));
    hair.push_back(Vertex(0.45f, 0.05f));
    hair.push_back(Vertex(0.46f, -0.05f));

    // Draw cartoon
    draw_curve(eye_r, 10);
    draw_curve(eye_l, 10);
    draw_curve(iris_r, 10);
    draw_curve(iris_l, 10);
    draw_curve(ear_r, 10);
    draw_curve(ear_l, 10);
    draw_curve(nose, 10);
    draw_curve(mouth_t, 10);
    draw_curve(mouth_b, 10);
    draw_curve(lips, 10);
    draw_curve(chin, 10);
    draw_curve(face, 10);
    draw_curve(hair, 10);

    glutSwapBuffers();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(725, 725); // Set your own window size
    glutCreateWindow("Assignment 1");
    setup();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
