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

#include <FreeImage.h>

//Define the size of the image
unsigned int width = 600, height = 450;


//Read a shader source from a file
//store the shader source in a std::vector<char>
void read_shader_src(const char *fname, std::vector<char> &buffer);

//Compile a shader
GLuint load_and_compile_shader(const char *fname, GLenum shaderType);

//Create a program from two shaders
GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);


// Render scene
void display(GLuint &vao);

// Initialize the data to be rendered
void initialize(GLuint &vao);

void load_image(const char* fname);

void wrap_tests();

GLuint wrap_option = 0;

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

    //Create a Vetex Array Object
    GLuint vao;

    //Initialize the data to be rendered
    initialize(vao);
    
    //Generate a PPM image
    /*

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
    image.write("floortile.ppm");
*/
    //Rendering loop
    int running = GL_TRUE;
    while ( glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && !glfwWindowShouldClose(window) )
    {
        display(vao);
        //Poll for events
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
} 


// Render scene
void display(GLuint &vao) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void initialize(GLuint &vao) {
    // Use a Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // 1 square (made by 2 triangles) to be rendered
    GLfloat vertices_position[8] = {
        -1.0, -1.0,
        1.0, -1.0,
        1.0, 1.0,
        -1.0, 1.0,
    };

    GLfloat texture_coord[8] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
    };

    GLuint indices[6] = {
        0, 1, 2,
        2, 3, 0
    };

    // Create a Vector Buffer Object that will store the vertices on video memory
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Allocate space for vertex positions and texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_position) + sizeof(texture_coord), NULL, GL_STATIC_DRAW);

    // Transfer the vertex positions:
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices_position), vertices_position);

    // Transfer the texture coordinates:
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices_position), sizeof(texture_coord), texture_coord);

    // Create an Element Array Buffer that will store the indices array:
    GLuint eab;
    glGenBuffers(1, &eab);

    // Transfer the data from indices to eab
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eab);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Create a texture
    GLuint texture;
    glGenTextures(1, &texture);

    // Specify that we work with a 2D texture
    glBindTexture(GL_TEXTURE_2D, texture);

    load_image("pictures/floor.ppm");


    //Load Shaders into a shader program
    GLuint shaderProgram = LoadShaders( "shaders/vert.shader", "shaders/frag.shader" );
    glUseProgram(shaderProgram);
    
     // Get the location of the attributes that enters in the vertex shader
    GLint position_attribute = glGetAttribLocation(shaderProgram, "position");

    // Specify how the data for position can be accessed
    glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Enable the attribute
    glEnableVertexAttribArray(position_attribute);

    // Texture coord attribute
    GLint texture_coord_attribute = glGetAttribLocation(shaderProgram, "texture_coord");
    glVertexAttribPointer(texture_coord_attribute, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(vertices_position));
    glEnableVertexAttribArray(texture_coord_attribute);

    }


void load_image(const char *fname) {

    // active only for static linking
    #ifdef FREEIMAGE_LIB
    FreeImage_Initialise();
    #endif

    FIBITMAP *bitmap;
    // Get the format of the image file
    FREE_IMAGE_FORMAT fif =FreeImage_GetFileType(fname, 0);

    // If the format can't be determined, try to guess the format from the file name
    if(fif == FIF_UNKNOWN) {
        fif = FreeImage_GetFIFFromFilename(fname);
    }

    // Load the data in bitmap if possible
    if(fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading(fif)) {
        bitmap = FreeImage_Load(fif, fname);
    }
    else {
        bitmap = NULL;
    }

    // PROCESS IMAGE if bitmap was successfully initialized
    if(bitmap) {
    unsigned int w = FreeImage_GetWidth(bitmap);
    unsigned int h = FreeImage_GetHeight(bitmap);
    unsigned pixel_size = FreeImage_GetBPP(bitmap);

    // Get a pointer to the pixel data
    BYTE *data = (BYTE*)FreeImage_GetBits(bitmap);

    // Process only RGB and RGBA images
    if(pixel_size == 24) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, (GLvoid*)data);
    }
    else if (pixel_size == 32) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)data);
    }
    else {
        std::cerr << "pixel size = " << pixel_size << " don't know how to process this case. I'm out!" << std::endl;
        exit(-1);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //wrap_tests();
    }
    else {
        std::cerr << "Unable to load the image file " << fname << " I'm out!" << std::endl;
        exit(-1);
    }

// Clean bitmap;
        FreeImage_Unload(bitmap);

    // active only for static linking
    #ifdef FREEIMAGE_LIB
    FreeImage_DeInitialise();
    #endif
}


/*void wrap_tests() {
    if (wrap_option == 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
        std::cout << "Using GL_REPEAT" << std::endl;
    }
    else if(wrap_option == 1) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        std::cout << "Using GL_CLAMP_TO_EDGE" << std::endl;
    }
    else if(wrap_option == 2) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        std::cout << "Using GL_CLAMP_TO_BORDER" << std::endl;
    }
    else if(wrap_option == 3) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        std::cout << "Using GL_MIRRORED_REPEAT" << std::endl;
    }   
}
*/

