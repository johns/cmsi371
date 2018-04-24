/***
 Assignment-2: Geometric Modeling of a Scene

 Name: Scott, John

 Collaborators (talked with about high-level concepts): Chami, Sam

 Project Summary: Implemented matrix multiplication by having a matrix, A, of size 4x4 multiples by a matrix, B, of size 4x4 or 24x4. Used matrix multiplication to build cubes and manipulate it into different rectangular shapes. Added color values to a vertex then converted to an array to add color to rectangular shapes. Formed a scene of a room with a rug, two chairs, a table, and a couch. Ran and compiled in Xcode.
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

#include <math.h>
#include <vector>
using namespace std;

/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene *
 *                                                *
 *************************************************/

// Initializes a square plane of unit lengths
vector<GLfloat> init_plane() {
    vector<GLfloat> vertices = {
        +0.5,   +0.5,   +0.0,
        -0.5,   +0.5,   +0.0,
        -0.5,   -0.5,   +0.0,
        +0.5,   -0.5,   +0.0,
    };
    return vertices;
}

// Converts a vector to an array
GLfloat* vector2array(vector<GLfloat> vec) {
    GLfloat* arr = new GLfloat[vec.size()];
    for (int i = 0; i < vec.size(); i++) {
        arr[i] = vec[i];
    }
    return arr;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_homogenous_coord(vector<GLfloat> cartesian_coords) {
    vector<GLfloat> result;
    for (int i = 0; i < cartesian_coords.size(); i += 3) {
        result.push_back(cartesian_coords[i]);
        result.push_back(cartesian_coords[i+1]);
        result.push_back(cartesian_coords[i+2]);
        result.push_back(1.0);
    }
    return result;
}

// Converts Cartesian coordinates to homogeneous coordinates
vector<GLfloat> to_cartesian_coord(vector<GLfloat> homogenous_coords) {
    vector<GLfloat> result;
    for (int i = 0; i < homogenous_coords.size(); i += 4) {
        result.push_back(homogenous_coords[i]);
        result.push_back(homogenous_coords[i+1]);
        result.push_back(homogenous_coords[i+2]);
    }
    return result;
}

// Definition of a translation matrix
vector<GLfloat> translation_matrix (float dx, float dy, float dz) {
    vector<GLfloat> translate_mat = {
        1.0,   0.0,   0.0,    dx,
        0.0,   1.0,   0.0,    dy,
        0.0,   0.0,   1.0,    dz,
        0.0,   0.0,   0.0,   1.0,
    };
    return translate_mat;
}

// Definition of a scaling matrix
vector<GLfloat> scaling_matrix (float sx, float sy, float sz) {
    vector<GLfloat> scale_mat = {
         sx,   0.0,   0.0,   0.0,
        0.0,    sy,   0.0,   0.0,
        0.0,   0.0,    sz,   0.0,
        0.0,   0.0,   0.0,   1.0,
    };
    return scale_mat;
}

// Definition of a rotation matrix along the x-axis theta degrees
vector<GLfloat> rotation_matrix_x (float theta) {
    vector<GLfloat> rotate_mat_x = {
        1.0,           0.0,            0.0,   0.0,
        0.0,   cosf(theta),   -sinf(theta),   0.0,
        0.0,   sinf(theta),    cosf(theta),   0.0,
        0.0,           0.0,            0.0,   1.0,
    };
    return rotate_mat_x;
}


// Definition of a rotation matrix along the y-axis by theta degrees
vector<GLfloat> rotation_matrix_y (float theta) {
    vector<GLfloat> rotate_mat_y = {
         cosf(theta),   0.0,    sinf(theta),   0.0,
                 0.0,   1.0,            0.0,   0.0,
        -sinf(theta),   0.0,    cosf(theta),   0.0,
                 0.0,   0.0,            0.0,   1.0,
    };
    return rotate_mat_y;
}


// Definition of a rotation matrix along the z-axis by theta degrees
vector<GLfloat> rotation_matrix_z (float theta) {
    vector<GLfloat> rotate_mat_z = {
        cosf(theta),   -sinf(theta),   0.0,   0.0,
        sinf(theta),    cosf(theta),   0.0,   0.0,
                0.0,            0.0,   1.0,   0.0,
                0.0,            0.0,   0.0,   1.0,
    };
    return rotate_mat_z;
}

// Perform matrix multiplication for A B
vector<GLfloat> mat_mult(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> result;
    for (int i = 0; i < B.size(); i += 4) {
        for (int j = 0; j < 16; j += 4) {
            result.push_back((A[j+0]*B[i+0])+(A[j+1]*B[i+1])+(A[j+2]*B[i+2])+(A[j+3]*B[i+3]));
        }
    }
    return result;
}

// Builds a unit cube centered at the origin
vector<GLfloat> build_cube() {
    vector<GLfloat> result;

    vector<GLfloat> A1 = to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.0, 0.5), to_homogenous_coord(init_plane())));
    vector<GLfloat> A2 = to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.0, -0.5), mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(init_plane()))));
    vector<GLfloat> A3 = to_cartesian_coord(mat_mult(translation_matrix(0.5, 0.0, 0.0), mat_mult(rotation_matrix_y(M_PI/2.0), to_homogenous_coord(init_plane()))));
    vector<GLfloat> A4 = to_cartesian_coord(mat_mult(translation_matrix(-0.5, 0.0, 0.0), mat_mult(rotation_matrix_y(-M_PI/2.0), to_homogenous_coord(init_plane()))));
    vector<GLfloat> A5 = to_cartesian_coord(mat_mult(translation_matrix(0.0, -0.5, 0.0), mat_mult(rotation_matrix_x(-M_PI/2.0), to_homogenous_coord(init_plane()))));
    vector<GLfloat> A6 = to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.5, 0.0), mat_mult(rotation_matrix_x(M_PI/2.0), to_homogenous_coord(init_plane()))));

    result.insert(result.end(), A1.begin(), A1.end());
    result.insert(result.end(), A2.begin(), A2.end());
    result.insert(result.end(), A3.begin(), A3.end());
    result.insert(result.end(), A4.begin(), A4.end());
    result.insert(result.end(), A5.begin(), A5.end());
    result.insert(result.end(), A6.begin(), A6.end());

    return result;
}

/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by using setting *
 *  the projection viewing matrices               *
 *                                                *
 *************************************************/
float theta = 0.0;
float elements = 0.0;

void setup() {
    // Enable the vertex array functionality
    glEnableClientState(GL_VERTEX_ARRAY);
    // Enable the color array functionality (so we can specify a color for each vertex)
    glEnableClientState(GL_COLOR_ARRAY);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Set up some default base color
    glColor3f(0.5, 0.5, 0.5);
    // Set up white background
    glClearColor(0.95, 0.95, 0.95, 0.00);
}

void init_camera() {
    // Camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a 75 degree field of view, 1:1 aspect ratio, near and far planes at 1 and 30
    gluPerspective(75.0, 1.0, 1.0, 30.0);
    // Position camera at (8, 6, 12), attention at (0, 0, 0), up at (0, 1, 0)
    gluLookAt(8.0, 6.0, 12.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    vector<GLfloat> scene;
    vector<GLfloat> hCube = to_homogenous_coord(build_cube());


    // Table
    vector<GLfloat> tableTop = to_cartesian_coord(mat_mult(scaling_matrix(4.0, 1.0, 2.0), hCube));
    vector<GLfloat> tableBase = to_cartesian_coord(mat_mult(translation_matrix(0.0, -0.5, 0.0), mat_mult(scaling_matrix(4.1, 0.1, 2.1), hCube)));
    vector<GLfloat> tableLegFL = to_cartesian_coord(mat_mult(translation_matrix(2.0, -1.0, 1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), hCube)));
    vector<GLfloat> tableLegFR = to_cartesian_coord(mat_mult(translation_matrix(-2.0, -1.0, 1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), hCube)));
    vector<GLfloat> tableLegBL = to_cartesian_coord(mat_mult(translation_matrix(2.0, -1.0, -1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), hCube)));
    vector<GLfloat> tableLegBR = to_cartesian_coord(mat_mult(translation_matrix(-2.0, -1.0, -1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), hCube)));

    scene.insert(scene.end(), tableTop.begin(), tableTop.end());
    scene.insert(scene.end(), tableBase.begin(), tableBase.end());
    scene.insert(scene.end(), tableLegFL.begin(), tableLegFL.end());
    scene.insert(scene.end(), tableLegFR.begin(), tableLegFR.end());
    scene.insert(scene.end(), tableLegBL.begin(), tableLegBL.end());
    scene.insert(scene.end(), tableLegBR.begin(), tableLegBR.end());

    elements = tableTop.size() + tableBase.size() + tableLegFL.size() + tableLegFR.size() + tableLegBL.size() + tableLegBR.size();


    // Rug
    vector<GLfloat> rug = to_cartesian_coord(mat_mult(translation_matrix(0.0, -1.5, 0.0), mat_mult(scaling_matrix(15.0, 0.1, 15.0), hCube)));

    scene.insert(scene.end(), rug.begin(), rug.end());

    elements += rug.size();


    // Right Chair
    vector<GLfloat> chairRBase = to_cartesian_coord(mat_mult(translation_matrix(5.75, 0.0, 0.0), mat_mult(scaling_matrix(3.5, 1.0, 3.0), hCube)));
    vector<GLfloat> chairRBack = to_cartesian_coord(mat_mult(translation_matrix(7.2, 1.0, 0.0), mat_mult(rotation_matrix_z(M_PI*3.6/8.0), mat_mult(scaling_matrix(2.5, 1.0, 3.0), hCube))));
    vector<GLfloat> chairRArmL = to_cartesian_coord(mat_mult(translation_matrix(6.25, 1.0, 1.25), mat_mult(scaling_matrix(2.5, 1.0, 0.5), hCube)));
    vector<GLfloat> chairRArmR = to_cartesian_coord(mat_mult(translation_matrix(6.25, 1.0, -1.25), mat_mult(scaling_matrix(2.5, 1.0, 0.5), hCube)));
    vector<GLfloat> chairRLegFL = to_cartesian_coord(mat_mult(translation_matrix(4.5, -1.0, 1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), hCube)));
    vector<GLfloat> chairRLegFR = to_cartesian_coord(mat_mult(translation_matrix(4.5, -1.0, -1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), hCube)));
    vector<GLfloat> chairRLegBL = to_cartesian_coord(mat_mult(translation_matrix(7.25, -1.0, 1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), hCube)));
    vector<GLfloat> chairRLegBR = to_cartesian_coord(mat_mult(translation_matrix(7.25, -1.0, -1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), hCube)));

    scene.insert(scene.end(), chairRBase.begin(), chairRBase.end());
    scene.insert(scene.end(), chairRBack.begin(), chairRBack.end());
    scene.insert(scene.end(), chairRArmL.begin(), chairRArmL.end());
    scene.insert(scene.end(), chairRArmR.begin(), chairRArmR.end());
    scene.insert(scene.end(), chairRLegFL.begin(), chairRLegFL.end());
    scene.insert(scene.end(), chairRLegFR.begin(), chairRLegFR.end());
    scene.insert(scene.end(), chairRLegBL.begin(), chairRLegBL.end());
    scene.insert(scene.end(), chairRLegBR.begin(), chairRLegBR.end());

    elements += chairRBase.size() + chairRBack.size() + chairRArmL.size() + chairRArmR.size() + chairRLegFL.size() + chairRLegFR.size() + chairRLegBL.size() + chairRLegBR.size();


    // Left Chair
    vector<GLfloat> chairLBase = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRBase)));
    vector<GLfloat> chairLBack = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRBack)));
    vector<GLfloat> chairLArmL = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRArmL)));
    vector<GLfloat> chairLArmR = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRArmR)));
    vector<GLfloat> chairLLegFL = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRLegFL)));
    vector<GLfloat> chairLLegFR = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRLegFR)));
    vector<GLfloat> chairLLegBL = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRLegBL)));
    vector<GLfloat> chairLLegBR = to_cartesian_coord(mat_mult(rotation_matrix_y(M_PI), to_homogenous_coord(chairRLegBR)));

    scene.insert(scene.end(), chairLBase.begin(), chairLBase.end());
    scene.insert(scene.end(), chairLBack.begin(), chairLBack.end());
    scene.insert(scene.end(), chairLArmL.begin(), chairLArmL.end());
    scene.insert(scene.end(), chairLArmR.begin(), chairLArmR.end());
    scene.insert(scene.end(), chairLLegFL.begin(), chairLLegFL.end());
    scene.insert(scene.end(), chairLLegFR.begin(), chairLLegFR.end());
    scene.insert(scene.end(), chairLLegBL.begin(), chairLLegBL.end());
    scene.insert(scene.end(), chairLLegBR.begin(), chairLLegBR.end());

    elements += chairLBase.size() + chairLBack.size() + chairLArmL.size() + chairLArmR.size() + chairLLegFL.size() + chairLLegFR.size() + chairLLegBL.size() + chairLLegBR.size();


    // Couch
    vector<GLfloat> couchBase = to_cartesian_coord(mat_mult(translation_matrix(0.0, -0.5, -4.5), mat_mult(scaling_matrix(8.0, 2.0, 3.0), hCube)));
    vector<GLfloat> couchBack = to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.5, -5.75), mat_mult(rotation_matrix_x(M_PI*3.8/8.0), mat_mult(scaling_matrix(8.0, 0.75, 3.5), hCube))));
    vector<GLfloat> couchArmL = to_cartesian_coord(mat_mult(translation_matrix(3.5, 0.75, -4.5), mat_mult(scaling_matrix(1.0, 1.0, 3.0), hCube)));
    vector<GLfloat> couchArmR = to_cartesian_coord(mat_mult(translation_matrix(-3.5, 0.75, -4.5), mat_mult(scaling_matrix(1.0, 1.0, 3.0), hCube)));
    vector<GLfloat> couchPillowL = to_cartesian_coord(mat_mult(translation_matrix(0.85, 0.85, -5.0), mat_mult(rotation_matrix_x(M_PI*3.8/8.0), mat_mult(scaling_matrix(1.5, 0.5, 1.25), hCube))));
    vector<GLfloat> couchPillowR = to_cartesian_coord(mat_mult(translation_matrix(-0.85, 0.85, -5.0), mat_mult(rotation_matrix_x(M_PI*3.8/8.0), mat_mult(scaling_matrix(1.5, 0.5, 1.25), hCube))));

    scene.insert(scene.end(), couchBase.begin(), couchBase.end());
    scene.insert(scene.end(), couchBack.begin(), couchBack.end());
    scene.insert(scene.end(), couchArmL.begin(), couchArmL.end());
    scene.insert(scene.end(), couchArmR.begin(), couchArmR.end());
    scene.insert(scene.end(), couchPillowL.begin(), couchPillowL.end());
    scene.insert(scene.end(), couchPillowR.begin(), couchPillowR.end());

    elements += couchBase.size() + couchBack.size() + couchArmL.size() + couchArmR.size() + couchPillowL.size() + couchPillowR.size();

    return vector2array(scene);
}

// Construct the color mapping of the scene
GLfloat* init_color() {
    vector<GLfloat> colors;

    // Color Matrics

    vector<GLfloat> black = {

        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,

        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,

        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,

        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,

        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,

        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,
        0.0,    0.0,    0.0,

    };

    vector<GLfloat> white = {

        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,

        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,

        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,

        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,

        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,

        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,
        1.0,    1.0,    1.0,

    };

    vector<GLfloat> saddleBrown = {

        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,

        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,

        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,

        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,

        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,

        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,
        0.545, 0.271, 0.075,

    };

    vector<GLfloat> wheat = {

        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,

        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,

        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,

        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,

        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,

        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,
        0.961, 0.871, 0.702,

    };

    vector<GLfloat> lightSteelBlue = {

        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,

        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,

        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,

        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,

        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,

        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,
        0.690, 0.769, 0.871,

    };

    vector<GLfloat> darkSlateGray = {

        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,

        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,

        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,

        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,

        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,

        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,
        0.184, 0.310, 0.310,

    };

    vector<GLfloat> lightGray = {

        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,

        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,

        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,

        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,

        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,

        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,
        0.827, 0.827, 0.827,

    };


    // tableTop
    colors.insert(colors.end(), saddleBrown.begin(), saddleBrown.end());
    // tableBase
    colors.insert(colors.end(), black.begin(), black.end());
    // tableLegFL
    colors.insert(colors.end(), black.begin(), black.end());
    // tableLegFR
    colors.insert(colors.end(), black.begin(), black.end());
    // tableLegBL
    colors.insert(colors.end(), black.begin(), black.end());
    // tableLegBR
    colors.insert(colors.end(), black.begin(), black.end());
    // rug
    colors.insert(colors.end(), darkSlateGray.begin(), darkSlateGray.end());
    // chairRBase
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairRBack
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairRArmL
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairRArmR
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairRLegFL
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairRLegFR
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairRLegBL
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairRLegBR
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairLBase
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairLBack
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairLArmL
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairLArmR
    colors.insert(colors.end(), lightSteelBlue.begin(), lightSteelBlue.end());
    // chairLLegFL
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairLLegFR
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairLLegBL
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // chairLLegBR
    colors.insert(colors.end(), wheat.begin(), wheat.end());
    // couchBase
    colors.insert(colors.end(), white.begin(), white.end());
    // couchBack
    colors.insert(colors.end(), white.begin(), white.end());
    // couchArmL
    colors.insert(colors.end(), white.begin(), white.end());
    // couchArmR
    colors.insert(colors.end(), white.begin(), white.end());
    // couchPillowL
    colors.insert(colors.end(), lightGray.begin(), lightGray.end());
    // couchPillowR
    colors.insert(colors.end(), lightGray.begin(), lightGray.end());



    return vector2array(colors);

}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // World model parameters
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(theta, 0.0, 1.0, 0.0);
    //glRotatef(theta, 1.0, 0.0, 0.0);

    GLfloat* vertices = init_scene();

    GLfloat* colors = init_color();

    glVertexPointer(3,          // 3 components (x, y, z)
                    GL_FLOAT,   // Vertex type is GL_FLOAT
                    0,          // Start position in referenced memory
                    vertices);  // Pointer to memory location to read from

    //pass the color pointer
    glColorPointer(3,           // 3 components (r, g, b)
                   GL_FLOAT,    // Vertex type is GL_FLOAT
                   0,           // Start position in referenced memory
                   colors);     // Pointer to memory location to read from

    // Draw quad point planes: each 4 vertices
    glDrawArrays(GL_QUADS, 0, elements/3);

    glFlush();            //Finish rendering
    glutSwapBuffers();

    delete(vertices);
    delete(colors);
}

void idle_func() {
    theta = theta+0.3;
    display_func();
}

int main (int argc, char **argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    // Create a window with rendering context and everything else we need
    glutCreateWindow("Assignment 2");

    setup();
    init_camera();

    // Set up our display function
    glutDisplayFunc(display_func);
    glutIdleFunc(idle_func);
    // Render our world
    glutMainLoop();

    // Remember to call "delete" on your dynmically allocated arrays
    // such that you don't suffer from memory leaks. e.g.
    // delete arr;

    return 0;
}
