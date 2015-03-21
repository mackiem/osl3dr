/*
 main

 Copyright 2012 Thomas Dalling - http://tomdalling.com/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

// third-party libraries
#include <windows.h>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <bitset>

// tdogl classes
#include "tdogl/Program.h"
#include "tdogl/Texture.h"


//stb
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// constants
const glm::vec2 SCREEN_SIZE(1280, 800);

// globals
tdogl::Texture* gTexture = NULL;
tdogl::Program* gProgram = NULL;
GLuint gVBO = 0;
GLuint gVBO2 = 0;
GLuint gVAO = 0;

bool getBit(int num, int bit) {
	return ((num & (1 << bit)) != 0);
}

int setBit(int num, int bit, int val) {
	return num | (1 << bit);
}

int cColor(int num, int thr, int offset, int bits) {
	int greenVal = 0;
	for (int i = bits-1; i >= 0; --i) {
		//std::cout << thr - 1 - i - offset;
		int bit = getBit(num, thr - 1 -i - offset);
		//std::cout << bit;
		if (bit == 1) {
			greenVal = setBit(greenVal, 7 - i, bit);
		}
	}
	return greenVal;
}

glm::vec3 createColors(int num, int thr) {
	std::cout << std::bitset<16>(num) << std::endl;

	int r, g, b;
	r = g = b = thr / 3;
	if (thr % 3 == 1) {
		r++;
	}
	else if (thr % 3 == 2) {
		r++;
		b++;
	}

	int offset = 0;
	const int NUM_OF_BITS = 8;

	int rBit, gBit, bBit;
	rBit = gBit = bBit = 7;
	for (int i = 0; i < thr; ++i) {
		if (rBit) {

		}
	}
    // green val
	offset += 0;
	int greenVal = cColor(num, thr, offset, g);

    // blue val
	offset += g;
	int blueVal = cColor(num, thr, offset, b);

    // red val
	offset += b;
	int redVal = cColor(num, thr, offset, r);

	std::cout << std::endl << std::bitset<8>(redVal) << std::endl;
	std::cout << std::bitset<8>(greenVal) << std::endl;
	std::cout << std::bitset<8>(blueVal) << std::endl;

	std::cout << std::endl << (redVal) << std::endl;
	std::cout << (greenVal) << std::endl;
	std::cout << (blueVal) << std::endl;

	return glm::vec3((float) redVal/255.f, (float) greenVal/255.f, (float)blueVal/255.f);
}


// returns the full path to the file `fileName` in the resources directory of the app bundle
static std::string ResourcePath(std::string fileName) {
    char executablePath[1024] = {'\0'};
    DWORD charsCopied = GetModuleFileName(NULL, executablePath, 1024);
    if(charsCopied > 0 && charsCopied < 1024)
        return std::string(executablePath) + "\\..\\" + fileName;
    else
        throw std::runtime_error("GetModuleFileName failed a bit");
}


// loads the vertex shader and fragment shader, and links them to make the global gProgram
static void LoadShaders() {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("vertex-shader.txt"), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("fragment-shader.txt"), GL_FRAGMENT_SHADER));
    gProgram = new tdogl::Program(shaders);
}

int vtcSize = 0;

void createProjBars(std::vector<glm::vec3> colors) {

    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    
	const float w = 5.f;
	const float h = 100.f;
	const float z = 20.f;
	const int totalColors = colors.size() / 3;
	const float startPntX = -1.f * totalColors * w / 2.f;
	std::vector<glm::vec3> vtcs;
	std::vector<glm::vec3> vtcColors;

	//const int start = std::ceil((float)(colors.size()) / 3.f);
	//const int start = std::ceil((float)(colors.size() * 2) / 3.f);
	const int start = 0;
	//for (int i = start; i < std::floor(((float)(colors.size() * 2) / 3.f)); ++i) {
	//for (int i = start; i < std::floor(((float)(colors.size() * 3) / 3.f)); ++i) {
	//for (int i = start; i < std::floor(((float)(colors.size() * 1) / 3.f)); ++i) {
	//for (int i = 0; i < colors.size() / 2; ++i) {
	for (int i = start; i < colors.size(); ++i) {
		// 1st triangle
		vtcs.push_back(glm::vec3(startPntX + w * (i - start), -h / 2.f, z));
		vtcs.push_back(glm::vec3(startPntX + w * (i+1 - start), -h / 2.f, z));
		vtcs.push_back(glm::vec3(startPntX + w * (i - start), h / 2.f, z));
		
		//2nd triangle
		vtcs.push_back(glm::vec3(startPntX + w * (i+1 - start), -h / 2.f, z));
		vtcs.push_back(glm::vec3(startPntX + w * (i+1 - start), h / 2.f, z));
		vtcs.push_back(glm::vec3(startPntX + w * (i - start), h / 2.f, z));

		for (int j = 0; j < 6; ++j) {
			vtcColors.push_back(colors[i]);
		}
		std::cout << i << " ";
	}

	vtcSize = vtcs.size();

	glBufferData(GL_ARRAY_BUFFER, vtcs.size() * sizeof(glm::vec3), &vtcs[0], GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vert"));
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);
        
    // make and bind the VBO
    glGenBuffers(1, &gVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO2);

	glBufferData(GL_ARRAY_BUFFER, vtcColors.size() * sizeof(glm::vec3), &vtcColors[0], GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vertColor"));
    glVertexAttribPointer(gProgram->attrib("vertColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    //glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
    //glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);
}

void saveImage() {
	const int w = SCREEN_SIZE.x;
	const int h = SCREEN_SIZE.y;
	const int size = SCREEN_SIZE.x * SCREEN_SIZE.y * 3;
	unsigned char *img = new unsigned char[size];
	glReadPixels(0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y, GL_RGB, GL_UNSIGNED_BYTE, img);
	stbi_write_bmp("osl-00.bmp", w, h, 3, img);
}

static void handleKeyBoard() {
	if (glfwGetKey('S') == GLFW_PRESS) {
		saveImage();
	}
}


// loads a triangle into the VAO global
static void LoadTriangle() {
    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    
    // Put the three triangle vertices (XYZ) and texture coordinates (UV) into the VBO
    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
         0.0f, 0.8f, 0.0f,   0.5f, 1.0f,
        -0.8f,-0.8f, 0.0f,   0.0f, 0.0f,
         0.8f,-0.8f, 0.0f,   1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vert"));
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);
        
    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gProgram->attrib("vertTexCoord"));
    glVertexAttribPointer(gProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);
}


// loads the file "hazard.png" into gTexture
static void LoadTexture() {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath("hazard.png"));
    bmp.flipVertically();
    gTexture = new tdogl::Texture(bmp);
}


// draws a single frame
static void Render() {
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT);
    
    // bind the program (the shaders)
    gProgram->use();
        
    // bind the texture and set the "tex" uniform in the fragment shader
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, gTexture->object());
    //gProgram->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0

    // bind the VAO (the triangle)
    glBindVertexArray(gVAO);
    
    // draw the VAO
	glm::mat4 model(1.f);
	glUniformMatrix4fv(gProgram->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, -60.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(gProgram->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(45.0f, 8.f / 6.f, 0.1f, 100.f);
	glUniformMatrix4fv(gProgram->uniform("proj"), 1, GL_FALSE, glm::value_ptr(proj));

    glDrawArrays(GL_TRIANGLES, 0, vtcSize);

	handleKeyBoard();
    
    // unbind the VAO, the program and the texture
    glBindVertexArray(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    gProgram->stopUsing();
    
	glfwPollEvents();
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers();
}

// the program starts here
void AppMain() {
    // initialise GLFW
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");
    
    // open a window with GLFW
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if(!glfwOpenWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
        throw std::runtime_error("glfwOpenWindow failed. Can your hardware handle OpenGL 3.2?");
    
    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");

    // load vertex and fragment shaders into opengl
    LoadShaders();

    // load the texture
    LoadTexture();

    // create buffer and fill it with the points of the triangle
	std::vector<int> nums;
	std::vector<glm::vec3> colors;
	//for (int i = 25; i < 51; ++i) {
	//    colors.push_back(createColors(i, 10));
	//}
	for (int i = 8; i < 17; ++i) {
	    colors.push_back(createColors(i, 7));
	}


	//std::vector<glm::vec3> colors;
	//colors.push_back(glm::vec3(1.f, 0.f, 0.f));
	//colors.push_back(glm::vec3(0.f, 1.f, 0.f));
	//colors.push_back(glm::vec3(0.f, 0.f, 1.f));
	createProjBars(colors);
    //LoadTriangle();

    // run while the window is open
    while(glfwGetWindowParam(GLFW_OPENED)){
        // draw one frame
        Render();
    }

    // clean up and exit
    glfwTerminate();
}

int main(int argc, char *argv[]) {
    try {
        AppMain();
    } catch (const std::exception& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}