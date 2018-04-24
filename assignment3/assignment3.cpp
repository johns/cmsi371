/***
 Assignment-3: Shading via Lighting and Colors

 Name: Scott, John

 Collaborators (talked with about high-level concepts): Chu, Teddy

 Project Summary: First I implemented the ObjectModel class into my existing code from Assignment 2. Next I added an Object (Stand) to my scene and added a new color to it. Finally I used the definition of illumination to apply shading to the base colors of my Objects. Ran and compiled in Xcode.
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
 *              Object Model Class                *
 *                                                *
 *  Stores the points of the object as a vector   *
 *  along with the colors and normals for each    *
 *  point. Normals are computed from the points.  *
 *                                                *
 *************************************************/
class ObjectModel {
    vector<GLfloat> _points;
    vector<GLfloat> _normals;
    vector<GLfloat> _base_colors;
    vector<GLfloat> _colors;
public:
    ObjectModel() { };
    vector<GLfloat> get_points() { return _points; };
    vector<GLfloat> get_normals() { return _normals; };
    vector<GLfloat> get_base_colors() { return _base_colors; };
    vector<GLfloat> get_colors() { return _colors; };
    void set_points(vector<GLfloat> points) { _points = points; };
    void set_normals(vector<GLfloat> normals) { _normals = normals; };
    void set_base_colors(vector<GLfloat> base_colors) { _base_colors = base_colors; };
    void set_colors(vector<GLfloat> colors) { _colors = colors; };
};


/**************************************************
 *  Rectangular Prisms via Hierarchical Modeling  *
 *                                                *
 *  Using planes as building blocks, build a unit *
 *  cube with transformations that will serve as  *
 *  a primitive for modeling objects in the scene.*
 *                                                *
 *************************************************/

// Initializes a square plane of unit lengths
vector<GLfloat> init_plane() {
    vector<GLfloat> vertices = {
        +0.5,   +0.5,   +0.0,
        -0.5,   +0.5,   +0.0,
        -0.5,   -0.5,   +0.0,
        +0.5,   -0.5,   +0.0
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
 *           Generating Surface Normals           *
 *                                                *
 *  Generate the surface normals of the objects   *
 *  using the cross product between two vectors   *
 *  that lie on the surface (plane) of interest.  *
 *  Recall that the direction of the normal to a  *
 *  surface follows the Right Hand Rule.          *
 *                                                *
 *************************************************/

vector<GLfloat> concat_vector (vector<GLfloat> A, vector<GLfloat> B) {
    for (int i = 0; i < B.size(); i++) {
        A.push_back(B[i]);
    }
    return A;
}


vector<GLfloat> concat_vector (vector<GLfloat> A, ObjectModel B) {
    for (int i = 0; i < B.get_points().size(); i++) {
        A.push_back(B.get_points()[i]);
    }
    return A;
}


vector<GLfloat> add_vector (vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> C = {
        A[0]+B[0], A[1]+B[1], A[2]+B[2]
    };
    return C;
}


vector<GLfloat> sub_vector (vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> C = {
        A[0]-B[0], A[1]-B[1], A[2]-B[2]
    };
    return C;
}


vector<GLfloat> to_unit(vector<GLfloat> n) {
    vector<GLfloat> result;
    GLfloat magnitude = 0.0;

    for (int i = 0; i < n.size(); i++) {
        magnitude += pow(n[i], 2);
    }

    magnitude = sqrt(magnitude);

    for (int i = 0; i < n.size(); i++) {
        result.push_back(n[i]/magnitude);
    }

    return result;

}


// Performs the cross product between two vectors
vector<GLfloat> cross_product(vector<GLfloat> A, vector<GLfloat> B) {
    vector<GLfloat> C = {
        (A[1]*B[2])-(A[2]*B[1]), (A[2]*B[0])-(A[0]*B[2]), (A[0]*B[1])-(A[1]*B[0])
    };
    return C;
}


// Generates the normals to each surface (plane)
vector<GLfloat> generate_normals(vector<GLfloat> points) {
    vector<GLfloat> normals;

    vector<GLfloat> q0, q1, q3, a, b, n;

    for (int i = 0; i < points.size(); i+=12) {
        q0 = {points[i], points[i+1], points[i+2]};
        q1 = {points[i+3], points[i+4], points[i+5]};
        q3 = {points[i+9], points[i+10], points[i+11]};

        a = sub_vector(q1, q0);
        b = sub_vector(q3, q0);

        n = cross_product(a, b);

        normals.push_back(n[0]);
        normals.push_back(n[1]);
        normals.push_back(n[2]);
    }


    return normals;
}


/**************************************************
 *         Shading via Lighting and Color         *
 *                                                *
 *  Generate the set of colors for each face of   *
 *  the planes that compose the objects in the    *
 *  scene. Based on the light source and surface  *
 *  normals, render the colors of the objects by  *
 *  applying the shading equation.                *
 *                                                *
 *************************************************/

// Initializes the base color of a plane to a single color
vector<GLfloat> init_base_color(GLfloat r, GLfloat g, GLfloat b) {
    vector<GLfloat> base_color = {
        r,   g,   b,
        r,   g,   b,
        r,   g,   b,
        r,   g,   b
    };
    return base_color;
}

vector<GLfloat> color_cube(GLfloat r, GLfloat g, GLfloat b) {
    vector<GLfloat> base_color;
    vector<GLfloat> plane1 = init_base_color(r, g, b);
    vector<GLfloat> plane2 = init_base_color(r, g, b);
    vector<GLfloat> plane3 = init_base_color(r, g, b);
    vector<GLfloat> plane4 = init_base_color(r, g, b);
    vector<GLfloat> plane5 = init_base_color(r, g, b);
    vector<GLfloat> plane6 = init_base_color(r, g, b);

    base_color = concat_vector(base_color, plane1);
    base_color = concat_vector(base_color, plane2);
    base_color = concat_vector(base_color, plane3);
    base_color = concat_vector(base_color, plane4);
    base_color = concat_vector(base_color, plane5);
    base_color = concat_vector(base_color, plane6);

    return base_color;
}


// Initializes the base color of a plane by specifying the color of each point
vector<GLfloat> init_base_color(GLfloat r0, GLfloat g0, GLfloat b0, GLfloat r1, GLfloat g1, GLfloat b1,
                                GLfloat r2, GLfloat g2, GLfloat b2, GLfloat r3, GLfloat g3, GLfloat b3) {
    // This enables OpenGL to use interpolation for (Gouraud) shading the plane
    vector<GLfloat> base_color = {
        r0,   g0,   b0,
        r1,   g1,   b1,
        r2,   g2,   b2,
        r3,   g3,   b3
    };
    return base_color;
}


// Performs the dot product between two vectors
GLfloat dot_product(vector<GLfloat> A, vector<GLfloat> B) {

    return (A[0]*B[0] + A[1]*B[1] + A[2]*B[2]);
}


// Generates the colors of a set of surfaces based on the light source,
// surface normals, and base color of the surface
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera) {
    vector<GLfloat> colors;

    object_model.set_colors(colors);
    return object_model;
}


// Allows for ambience (a vector of 3 values), diffusion (vector of 3 x n values) and specular (vector of 3 x n values)
// as input to the shading equation
ObjectModel apply_shading(ObjectModel object_model, vector<GLfloat> light_source, vector<GLfloat> camera,
                          vector<GLfloat> amb, vector<GLfloat> diff, vector<GLfloat> spec) {
    vector<GLfloat> colors, colorX, colorY, colorZ, points, normals, v, h;
    vector<GLfloat> base_colors = object_model.get_base_colors();


    for (int i = 0; i < base_colors.size(); i+=3) {
        points = { object_model.get_points()[i], object_model.get_points()[i+1], object_model.get_points()[i+2] };
        normals = { object_model.get_normals()[i], object_model.get_normals()[i+1], object_model.get_normals()[i+2] };
        v = sub_vector(points, camera);
        h = to_unit(add_vector(v, light_source));

        colors.push_back(base_colors[i]*(amb[0]+diff[0]*(dot_product(normals, light_source)) + base_colors[i]*spec[0]*dot_product(normals, h)));
        colors.push_back(base_colors[i+1]*(amb[1]+diff[1]*(dot_product(normals, light_source)) + base_colors[i+1]*spec[1]*dot_product(normals, h)));
        colors.push_back(base_colors[i+2]*(amb[2]+diff[2]*(dot_product(normals, light_source)) + base_colors[i+2]*spec[2]*dot_product(normals, h)));
    }

    object_model.set_colors(colors);
    return object_model;
}


/**************************************************
 *            Camera and World Modeling           *
 *                                                *
 *  Create a scene by applying transformations to *
 *  the objects built from planes and position    *
 *  the camera to view the scene by using setting *
 *  the projection viewing matrices               *
 *                                                *
 *************************************************/
float theta = 0.0;
float elements = 72.0 * 34.0;             // elements = points in a cube * number of cubes
vector<GLfloat> camera = { 8.0, 6.0, 12.0 };
vector<GLfloat> light_source = { -8.0, 6.0, -12.0 };
vector<GLfloat> amb = { 0.55, 0.55, 0.55 };
vector<GLfloat> diff = { 0.15, 0.15, 0.15 };
vector<GLfloat> spec = { 0.15, 0.15, 0.15 };


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

ObjectModel build_table() {
    vector<GLfloat> result;
    vector<GLfloat> tableTop = to_cartesian_coord(mat_mult(scaling_matrix(4.0, 1.0, 2.0), to_homogenous_coord(build_cube())));
    vector<GLfloat> tableBase = to_cartesian_coord(mat_mult(translation_matrix(0.0, -0.5, 0.0), mat_mult(scaling_matrix(4.1, 0.1, 2.1), to_homogenous_coord(build_cube()))));
    vector<GLfloat> tableLegFL = to_cartesian_coord(mat_mult(translation_matrix(2.0, -1.0, 1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), to_homogenous_coord(build_cube()))));
    vector<GLfloat> tableLegFR = to_cartesian_coord(mat_mult(translation_matrix(-2.0, -1.0, 1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), to_homogenous_coord(build_cube()))));
    vector<GLfloat> tableLegBL = to_cartesian_coord(mat_mult(translation_matrix(2.0, -1.0, -1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), to_homogenous_coord(build_cube()))));
    vector<GLfloat> tableLegBR = to_cartesian_coord(mat_mult(translation_matrix(-2.0, -1.0, -1.0), mat_mult(scaling_matrix(0.1, 1.1, 0.1), to_homogenous_coord(build_cube()))));

    result = concat_vector(result, tableTop);
    result = concat_vector(result, tableBase);
    result = concat_vector(result, tableLegFL);
    result = concat_vector(result, tableLegFR);
    result = concat_vector(result, tableLegBL);
    result = concat_vector(result, tableLegBR);

    vector<GLfloat> color;
    vector<GLfloat> colorTop = color_cube(0.545, 0.271, 0.075);           // saddleBrown
    vector<GLfloat> colorBase = color_cube(0.0, 0.0, 0.0);                // black
    vector<GLfloat> colorLegFL = color_cube(0.0, 0.0, 0.0);
    vector<GLfloat> colorLegFR = color_cube(0.0, 0.0, 0.0);
    vector<GLfloat> colorLegBL = color_cube(0.0, 0.0, 0.0);
    vector<GLfloat> colorLegBR = color_cube(0.0, 0.0, 0.0);

    color = concat_vector(color, colorTop);
    color = concat_vector(color, colorBase);
    color = concat_vector(color, colorLegFL);
    color = concat_vector(color, colorLegFR);
    color = concat_vector(color, colorLegBL);
    color = concat_vector(color, colorLegBR);

    ObjectModel table;

    table.set_points(result);
    table.set_normals(generate_normals(result));
    table.set_base_colors(color);
    table = apply_shading(table, light_source, camera, amb, diff, spec);

    return table;
}


ObjectModel build_rug() {
    vector<GLfloat> result;
    vector<GLfloat> rugComplete = to_cartesian_coord(mat_mult(translation_matrix(0.0, -1.5, 0.0), mat_mult(scaling_matrix(15.0, 0.1, 15.0), to_homogenous_coord(build_cube()))));

    result = concat_vector(result, rugComplete);

    vector<GLfloat> color;
    vector<GLfloat> colorComplete = color_cube(0.184, 0.310, 0.310);         // darkSlateGray

    color = concat_vector(color, colorComplete);

    ObjectModel rug;

    rug.set_points(result);
    rug.set_normals(generate_normals(result));
    rug.set_base_colors(color);
    rug = apply_shading(rug, light_source, camera, amb, diff, spec);

    return rug;
}


ObjectModel build_chairR() {
    vector<GLfloat> result;
    vector<GLfloat> chairRBase = to_cartesian_coord(mat_mult(translation_matrix(5.75, 0.0, 0.0), mat_mult(scaling_matrix(3.5, 1.0, 3.0), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairRBack = to_cartesian_coord(mat_mult(translation_matrix(7.2, 1.0, 0.0), mat_mult(rotation_matrix_z(M_PI*3.6/8.0), mat_mult(scaling_matrix(2.5, 1.0, 3.0), to_homogenous_coord(build_cube())))));
    vector<GLfloat> chairRArmL = to_cartesian_coord(mat_mult(translation_matrix(6.25, 1.0, 1.25), mat_mult(scaling_matrix(2.5, 1.0, 0.5), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairRArmR = to_cartesian_coord(mat_mult(translation_matrix(6.25, 1.0, -1.25), mat_mult(scaling_matrix(2.5, 1.0, 0.5), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairRLegFL = to_cartesian_coord(mat_mult(translation_matrix(4.5, -1.0, 1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairRLegFR = to_cartesian_coord(mat_mult(translation_matrix(4.5, -1.0, -1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairRLegBL = to_cartesian_coord(mat_mult(translation_matrix(7.25, -1.0, 1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairRLegBR = to_cartesian_coord(mat_mult(translation_matrix(7.25, -1.0, -1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));

    result = concat_vector(result, chairRBase);
    result = concat_vector(result, chairRBack);
    result = concat_vector(result, chairRArmL);
    result = concat_vector(result, chairRArmR);
    result = concat_vector(result, chairRLegFL);
    result = concat_vector(result, chairRLegFR);
    result = concat_vector(result, chairRLegBL);
    result = concat_vector(result, chairRLegBR);

    vector<GLfloat> color;
    vector<GLfloat> colorBase = color_cube(0.690, 0.769, 0.871);            // lightSteelBlue
    vector<GLfloat> colorBack = color_cube(0.690, 0.769, 0.871);
    vector<GLfloat> colorArmL = color_cube(0.690, 0.769, 0.871);
    vector<GLfloat> colorArmR = color_cube(0.690, 0.769, 0.871);
    vector<GLfloat> colorLegFL = color_cube(0.961, 0.871, 0.702);           // wheat
    vector<GLfloat> colorLegFR = color_cube(0.961, 0.871, 0.702);
    vector<GLfloat> colorLegBL = color_cube(0.961, 0.871, 0.702);
    vector<GLfloat> colorLegBR = color_cube(0.961, 0.871, 0.702);

    color = concat_vector(color, colorBase);
    color = concat_vector(color, colorBack);
    color = concat_vector(color, colorArmL);
    color = concat_vector(color, colorArmR);
    color = concat_vector(color, colorLegFL);
    color = concat_vector(color, colorLegFR);
    color = concat_vector(color, colorLegBL);
    color = concat_vector(color, colorLegBR);

    ObjectModel chairR;

    chairR.set_points(result);
    chairR.set_normals(generate_normals(result));
    chairR.set_base_colors(color);
    chairR = apply_shading(chairR, light_source, camera, amb, diff, spec);

    return chairR;
}


ObjectModel build_chairL() {
    vector<GLfloat> result;
    vector<GLfloat> chairLBase = to_cartesian_coord(mat_mult(translation_matrix(-5.75, 0.0, 0.0), mat_mult(scaling_matrix(3.5, 1.0, 3.0), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairLBack = to_cartesian_coord(mat_mult(translation_matrix(-7.2, 1.0, 0.0), mat_mult(rotation_matrix_z(M_PI*4.4/8.0), mat_mult(scaling_matrix(2.5, 1.0, 3.0), to_homogenous_coord(build_cube())))));
    vector<GLfloat> chairLArmL = to_cartesian_coord(mat_mult(translation_matrix(-6.25, 1.0, 1.25), mat_mult(scaling_matrix(2.5, 1.0, 0.5), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairLArmR = to_cartesian_coord(mat_mult(translation_matrix(-6.25, 1.0, -1.25), mat_mult(scaling_matrix(2.5, 1.0, 0.5), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairLLegFL = to_cartesian_coord(mat_mult(translation_matrix(-4.5, -1.0, 1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairLLegFR = to_cartesian_coord(mat_mult(translation_matrix(-4.5, -1.0, -1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairLLegBL = to_cartesian_coord(mat_mult(translation_matrix(-7.25, -1.0, 1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));
    vector<GLfloat> chairLLegBR = to_cartesian_coord(mat_mult(translation_matrix(-7.25, -1.0, -1.25), mat_mult(scaling_matrix(0.2, 1.0, 0.2), to_homogenous_coord(build_cube()))));

    result = concat_vector(result, chairLBase);
    result = concat_vector(result, chairLBack);
    result = concat_vector(result, chairLArmL);
    result = concat_vector(result, chairLArmR);
    result = concat_vector(result, chairLLegFL);
    result = concat_vector(result, chairLLegFR);
    result = concat_vector(result, chairLLegBL);
    result = concat_vector(result, chairLLegBR);

    vector<GLfloat> color;
    vector<GLfloat> colorBase = color_cube(0.690, 0.769, 0.871);            // lightSteelBlue
    vector<GLfloat> colorBack = color_cube(0.690, 0.769, 0.871);
    vector<GLfloat> colorArmL = color_cube(0.690, 0.769, 0.871);
    vector<GLfloat> colorArmR = color_cube(0.690, 0.769, 0.871);
    vector<GLfloat> colorLegFL = color_cube(0.961, 0.871, 0.702);           // wheat
    vector<GLfloat> colorLegFR = color_cube(0.961, 0.871, 0.702);
    vector<GLfloat> colorLegBL = color_cube(0.961, 0.871, 0.702);
    vector<GLfloat> colorLegBR = color_cube(0.961, 0.871, 0.702);

    color = concat_vector(color, colorBase);
    color = concat_vector(color, colorBack);
    color = concat_vector(color, colorArmL);
    color = concat_vector(color, colorArmR);
    color = concat_vector(color, colorLegFL);
    color = concat_vector(color, colorLegFR);
    color = concat_vector(color, colorLegBL);
    color = concat_vector(color, colorLegBR);

    ObjectModel chairL;

    chairL.set_points(result);
    chairL.set_normals(generate_normals(result));
    chairL.set_base_colors(color);
    chairL = apply_shading(chairL, light_source, camera, amb, diff, spec);

    return chairL;
}


ObjectModel build_couch() {
    vector<GLfloat> result;
    vector<GLfloat> couchBase = to_cartesian_coord(mat_mult(translation_matrix(0.0, -0.5, -4.5), mat_mult(scaling_matrix(8.0, 2.0, 3.0), to_homogenous_coord(build_cube()))));
    vector<GLfloat> couchBack = to_cartesian_coord(mat_mult(translation_matrix(0.0, 0.5, -5.75), mat_mult(rotation_matrix_x(M_PI*3.8/8.0), mat_mult(scaling_matrix(8.0, 0.75, 3.5), to_homogenous_coord(build_cube())))));
    vector<GLfloat> couchArmL = to_cartesian_coord(mat_mult(translation_matrix(3.5, 0.75, -4.5), mat_mult(scaling_matrix(1.0, 1.0, 3.0), to_homogenous_coord(build_cube()))));
    vector<GLfloat> couchArmR = to_cartesian_coord(mat_mult(translation_matrix(-3.5, 0.75, -4.5), mat_mult(scaling_matrix(1.0, 1.0, 3.0), to_homogenous_coord(build_cube()))));
    vector<GLfloat> couchPillowL = to_cartesian_coord(mat_mult(translation_matrix(0.85, 0.85, -5.0), mat_mult(rotation_matrix_x(M_PI*3.8/8.0), mat_mult(scaling_matrix(1.5, 0.5, 1.25), to_homogenous_coord(build_cube())))));
    vector<GLfloat> couchPillowR = to_cartesian_coord(mat_mult(translation_matrix(-0.85, 0.85, -5.0), mat_mult(rotation_matrix_x(M_PI*3.8/8.0), mat_mult(scaling_matrix(1.5, 0.5, 1.25), to_homogenous_coord(build_cube())))));

    result = concat_vector(result, couchBase);
    result = concat_vector(result, couchBack);
    result = concat_vector(result, couchArmL);
    result = concat_vector(result, couchArmR);
    result = concat_vector(result, couchPillowL);
    result = concat_vector(result, couchPillowR);

    vector<GLfloat> color;
    vector<GLfloat> colorBase = color_cube(1.0, 1.0, 1.0);           // white
    vector<GLfloat> colorBack = color_cube(1.0, 1.0, 1.0);
    vector<GLfloat> colorArmL = color_cube(1.0, 1.0, 1.0);
    vector<GLfloat> colorArmR = color_cube(1.0, 1.0, 1.0);
    vector<GLfloat> colorPillowL = color_cube(0.827, 0.827, 0.827); // lightGray
    vector<GLfloat> colorPillowR = color_cube(0.827, 0.827, 0.827);

    color = concat_vector(color, colorBase);
    color = concat_vector(color, colorBack);
    color = concat_vector(color, colorArmL);
    color = concat_vector(color, colorArmR);
    color = concat_vector(color, colorPillowL);
    color = concat_vector(color, colorPillowR);

    ObjectModel couch;

    couch.set_points(result);
    couch.set_normals(generate_normals(result));
    couch.set_base_colors(color);
    couch = apply_shading(couch, light_source, camera, amb, diff, spec);

    return couch;
}


ObjectModel build_stand() {
    vector<GLfloat> result;
    vector<GLfloat> standTop = to_cartesian_coord(mat_mult(translation_matrix(-5.25, 1.25, -4.5), mat_mult(scaling_matrix(2.25, 0.2, 2.25), to_homogenous_coord(build_cube()))));
    vector<GLfloat> standLegFL = to_cartesian_coord(mat_mult(translation_matrix(-5.75, -0.15, -3.75), mat_mult(rotation_matrix_z(M_PI*(-0.4)/8.0), mat_mult(scaling_matrix(0.2, 2.75, 0.2), to_homogenous_coord(build_cube())))));
    vector<GLfloat> standLegFR = to_cartesian_coord(mat_mult(translation_matrix(-4.75, -0.15, -3.75), mat_mult(rotation_matrix_z(M_PI*0.4/8.0), mat_mult(scaling_matrix(0.2, 2.75, 0.2), to_homogenous_coord(build_cube())))));
    vector<GLfloat> standLegBL = to_cartesian_coord(mat_mult(translation_matrix(-5.75, -0.15, -5.25), mat_mult(rotation_matrix_z(M_PI*(-0.4)/8.0), mat_mult(scaling_matrix(0.2, 2.75, 0.2), to_homogenous_coord(build_cube())))));
    vector<GLfloat> standLegBR = to_cartesian_coord(mat_mult(translation_matrix(-4.75, -0.15, -5.25), mat_mult(rotation_matrix_z(M_PI*0.4/8.0), mat_mult(scaling_matrix(0.2, 2.75, 0.2), to_homogenous_coord(build_cube())))));

    result = concat_vector(result, standTop);
    result = concat_vector(result, standLegFL);
    result = concat_vector(result, standLegFR);
    result = concat_vector(result, standLegBL);
    result = concat_vector(result, standLegBR);

    vector<GLfloat> color;
    vector<GLfloat> colorTop = color_cube(0.871, 0.722, 0.529);           // burlyWood
    vector<GLfloat> colorLegFL = color_cube(0.871, 0.722, 0.529);
    vector<GLfloat> colorLegFR = color_cube(0.871, 0.722, 0.529);
    vector<GLfloat> colorLegBL = color_cube(0.871, 0.722, 0.529);
    vector<GLfloat> colorLegBR = color_cube(0.871, 0.722, 0.529);

    color = concat_vector(color, colorTop);
    color = concat_vector(color, colorLegFL);
    color = concat_vector(color, colorLegFR);
    color = concat_vector(color, colorLegBL);
    color = concat_vector(color, colorLegBR);

    ObjectModel stand;

    stand.set_points(result);
    stand.set_normals(generate_normals(result));
    stand.set_base_colors(color);
    stand = apply_shading(stand, light_source, camera, amb, diff, spec);


    return stand;
}


// Construct the scene using objects built from cubes/prisms
GLfloat* init_scene() {
    vector<GLfloat> scene;

    scene = concat_vector(scene, build_table().get_points());
    scene = concat_vector(scene, build_rug().get_points());
    scene = concat_vector(scene, build_chairR().get_points());
    scene = concat_vector(scene, build_chairL().get_points());
    scene = concat_vector(scene, build_couch().get_points());
    scene = concat_vector(scene, build_stand().get_points());

    return vector2array(scene);
}


// Construct the color mapping of the scene
GLfloat* init_color() {
    vector<GLfloat> colors;

    colors = concat_vector(colors, build_table().get_colors());
    colors = concat_vector(colors, build_rug().get_colors());
    colors = concat_vector(colors, build_chairR().get_colors());
    colors = concat_vector(colors, build_chairL().get_colors());
    colors = concat_vector(colors, build_couch().get_colors());
    colors = concat_vector(colors, build_stand().get_colors());


    return vector2array(colors);
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // World model parameters
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(theta, 0.0, 1.0, 0.0);
    // glRotatef(theta, 1.0, 0.0, 0.0);

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
    glutCreateWindow("Assignment 3");

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
