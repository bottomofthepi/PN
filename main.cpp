#include <cmath>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>


#include "ppm.h"
#include "PerlinNoise.h"
#include "shaders.cpp"


#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

// 4 triangles to be rendered
GLfloat vertices_position[24] = {
0.0, 0.0,
0.5, 0.0,
0.5, 0.5,

0.0, 0.0,
0.0, 0.5,
-0.5, 0.5,

0.0, 0.0,
-0.5, 0.0,
-0.5, -0.5,

0.0, 0.0,
0.0, -0.5,
0.5, -0.5,
};

//Define the size of the image
unsigned int width = 600, height = 450;


//Read a shader source from a file
//store the shader source in a std::vector<char>
void read_shader_src(const char *fname, std::vector<char> &buffer);

//Compile a shader
GLuint load_and_compile_shader(const char *fname, GLenum shaderType);

//Create a program from two shaders
GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);

//Render the scene
void display(GLuint &vao);

//Initialize the data to be rendered
void initialize(GLuint &vao);

static void error_callback( int error, const char* description )
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window , int key, int scancode, int action, int mods)
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main() {
    //Initialize GLFW
    if( !glfwInit() ) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    //Use OpenGL 3.2 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //Open a Window and attach an OpenGL rendering context
    GLFWwindow* window;
    window = glfwCreateWindow(600, 480, "PN", NULL, NULL);
    if( !window )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    //Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();


    //Create Vertex Array Object
    GLuint VertexArrayID;
    glGenVertexArrays( 1, &VertexArrayID );
    glBindVertexArray( VertexArrayID );


    //Load Shaders into a shader program
    GLuint shaderProgram = LoadShaders( "shaders/vert.shader", "shaders/frag.shader" );
    glUseProgram(shaderProgram);

    GLuint VertexBufferID;
    glGenBuffers( 1, &VertexBufferID ); //Generate 1 buffer
    glBindBuffer( GL_ARRAY_BUFFER, VertexBufferID );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices_position), vertices_position, GL_STATIC_DRAW );
    
    GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
    glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray( posAttrib ); // Enable the vertex attrib
    
    GLint colAttrib = glGetAttribLocation( shaderProgram, "position" );
    glVertexAttribPointer( colAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray( colAttrib );


    glEnable(GL_PROGRAM_POINT_SIZE);

    /*
    //Generate a PPM image
    //

    //Create an empty ppm image
    ppm image(width, height);

    //Create a PerlinNoise object with a random permutation vector generated with Perlin Noise
    unsigned int seed = 237;
    PerlinNoise pn(seed);

    unsigned int kk = 0;
    //Visit every pixel of the image and assign a color generated with PN
    for(unsigned int i = 0; i < height; ++i) {      //y
        for(unsigned int j = 0; j < width; ++j) {   //x
            double x = (double)j/((double)width);
            double y = (double)i/((double)height);

            //Typical Perlin Noise
            double n = pn.noise(10 * x, 10 * y, 0.8);

            //Map the values to the [0,255] interval for simplicity (tones of grey)
            image.r[kk] = floor(255 * n);
            image.g[kk] = floor(255 * n);
            image.b[kk] = floor(255 * n);
            kk++;
        }
    }

    //Save the image to a binary ppm file
    image.write("floortile.ppm");*/

    //Rendering loop
    int running = GL_TRUE;
    while ( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window) )
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor( 1.0f, 0.0f, 0.0f, 1.0f);
        glBindVertexArray(VertexArrayID);
        glDrawArrays(GL_POINTS, 0, 12);
        //Poll for events
        glfwPollEvents();
        glfwSwapBuffers(window);

        //Check if the window was closed
    }

    glfwTerminate();
    return 0;
} 