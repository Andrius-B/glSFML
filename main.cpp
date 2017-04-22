#include <gl3w.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp> //for image loading
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm> //std::sort
#include <math.h>
#include <vector>
#include <array>
#include <sstream>
#include "Shader.cpp"
#define GLM_FORCE_CXX11
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "objLoader.cpp"
#define _USE_MATH_DEFINES

std::string objfile = "Knife.obj"; //"Knife.obj" yra kitas modelis jei nepatinka suzanne
std::string diffuseTexFile = "KnifeDiffuse.png";
static float rotation=-2.35;
static const int MAXFACES = 50000;
static const int MAXVERTS = MAXFACES*3;//triangles
static GLfloat vertex_data[MAXVERTS*3]; //positional vertex buffer data
static GLfloat normals[MAXVERTS*3];
static GLfloat texCoord[MAXVERTS*2]; //U and V components

static const GLfloat clearColor[] = { 0.3f, 0.3f, 0.3f, 0.f};
static const glm::vec3 lamp(1.0f, 1.0f, 2.0f);;
static glm::mat4 viewMat;
static glm::mat4 modelMat;

glm::vec3 normalize(glm::vec3 in){  //utility: normalize a vec3
    float len = sqrt((in.x*in.x)+(in.y*in.y)+(in.z*in.z)); //calculate length
    glm::vec3 out = in/len; //devide by length, so that length = 1
    return out;
}

glm::vec3 getNormals(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3){//convert to glm returns a normal of a face

    glm::vec3 U = p2-p1;
    glm::vec3 V = p3-p1;
    glm::vec3 normVec;

    normVec[0] = U[1]*V[2] - U[2]*V[1];
    normVec[1] = U[2]*V[0] - U[0]*V[2];
    normVec[2] = U[0]*V[1] - U[1]*V[0];
    normVec = normalize(normVec);
    return normVec;
}

void setView(sf::Window &window, GLuint program){
    viewMat = glm::perspective(70.0f, (float)window.getSize().x/(float)window.getSize().y, 0.01f, 100.0f);//createperspective matrix
    viewMat = viewMat * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.4f, -8.0f));//translate view
    viewMat = viewMat * glm::rotate(glm::mat4(1.0f) ,0.5f, glm::vec3(0.1f, 0.0f, 0.0f));//rotation matrix
    viewMat = viewMat * glm::rotate(glm::mat4(1.0f) ,rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    GLuint modPos = glGetUniformLocation(program, "modelMatrix"); //send the modelmatrix to opengl as uniform
    glUniformMatrix4fv(modPos, 1, GL_TRUE, &modelMat[0][0]);

    GLint matPos =  glGetUniformLocation(program, "viewMatrix"); //send the view matrix to opengl
    glUniformMatrix4fv(matPos, 1, GL_TRUE, &viewMat[0][0]);

    GLuint lampPos = glGetUniformLocation(program, "lamp"); //send lamp position to opengl
    glUniform3fv(lampPos, 1 , &lamp[0]);
    }

OBJ obj; //object for loading obj files

int main()
{
    // create the window
    sf::Window window(sf::VideoMode(800, 600), "glSFML", sf::Style::Default, sf::ContextSettings(32));
    //window.setVerticalSyncEnabled(true);
    gl3wInit();
    glEnable(GL_DEPTH_TEST);//enable depth testing

    std::vector<glm::vec3> verts = obj.readVerts(objfile); //read verts form obj
    std::vector<glm::vec3> faces = obj.readFaces(objfile); //read face-vertex list from obj
    std::vector<glm::vec2> vt = obj.readUV(objfile);
    std::vector<int> verTexRaw = obj.readVerTex(objfile);

    std::vector<int> vertList;//a list ordered to send to opengl
    for(int f = 0; f<faces.size();f++){//we have face.size() faces
        for(int v = 0; v<3;v++){ //each face has 3 verts;
            vertList.push_back((int)faces[f][v]); //list them by nuber in vector "verts"
        }
    }
    for(int v = 0; v<vertList.size();v++){ //writing vertex data to the array
        for(int i=0;i<3;i++)
            vertex_data[(v*3)+i] = verts[vertList[v]][i];
    }

    std::vector<glm::vec3> faceNormals;
    faceNormals.resize(faces.size());
    for(int f = 0; f<faces.size();f++){
            faceNormals[f] = getNormals(verts[faces[f][0]], verts[faces[f][1]], verts[faces[f][2]]); //arrange face normals in a list

    }

    std::vector<glm::vec3> vecNormals;
    for(int v = 0; v<verts.size(); v++){
        std::vector<int> appearedIn;
        for(int f = 0; f<faces.size();f++){
            for(int i = 0; i<3; i++){
                if(faces[f][i]==v){
                appearedIn.push_back(f); //find out in what faces a vector appeared in and list them
                }
            }
        }

        glm::vec3 sum(0.0f);
        for(int s = 0; s<appearedIn.size();s++){
            sum = sum + faceNormals[appearedIn[s]]; //add all the face normals to a single 0 vec
        }
        sum = normalize(sum); //normalize it and we have our vec normal
        vecNormals.push_back(sum);
    }

    for(int v = 0; v<vertList.size();v++){ //writing normal data to the array
            normals[v*3] = vecNormals[vertList[v]].x;
            normals[v*3+1] = vecNormals[vertList[v]].y;
            normals[v*3+2] = vecNormals[vertList[v]].z;
    }

    for(int i = 0; i<verTexRaw.size();i++){
        int n = verTexRaw[i];
        texCoord[i*2] = vt[n].x;
        texCoord[i*2+1] = vt[n].y;
    }


    Shader shader;
    shader.loadVertexShader("vertexShader.txt");
    shader.loadFragmentShader("fragmentShader.txt");        //Shader set-up
    GLuint program = shader.getProgram();
    glUseProgram(program); //linking current render program

    setView(window, program); //applay perspective

    GLuint vao; //vao setup
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint diffuseTex;
    glGenTextures(1, &diffuseTex);
    glBindTexture(GL_TEXTURE_2D, diffuseTex);//bind texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //set to linear scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    sf::Image diffuseTexture;
    if (!diffuseTexture.loadFromFile(diffuseTexFile.c_str()))   printf("failed to load %s image", diffuseTexFile.c_str());
    printf("%s texture size: %ix%i",diffuseTexFile.c_str(), diffuseTexture.getSize().x, diffuseTexture.getSize().y);
    diffuseTexture.flipVertically();
    const sf::Uint8*image = diffuseTexture.getPixelsPtr();

    glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     diffuseTexture.getSize().x,
                     diffuseTexture.getSize().y,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     image
                 );

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    glVertexAttribPointer(
               0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               0            // array buffer offset
            );

    GLuint normalbuffer; //create a buffer for vec normals
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);

    glVertexAttribPointer( //set up the buffer pointer
               1,                  // attribute 1.
               3,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               0            // array buffer offset
            );

    GLuint texcoordbuffer; //create a buffer for vec normals
    glGenBuffers(1, &texcoordbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordbuffer);

    glVertexAttribPointer( //set up the buffer pointer
               2,                  // attribute 2.
               2,                  // size
               GL_FLOAT,           // type
               GL_FALSE,           // normalized?
               0,                  // stride
               0            // array buffer offset
            );
            glEnableVertexAttribArray(0); //enable all buffers and their layout values
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);


    bool running = true;
    sf::Clock timer;
    sf::Time time;
    while (running)
    {
        // handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                //running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                //adjust the viewport when the window is resized
                glViewport(0, 0, event.size.width, event.size.height);
                setView(window, program);//adjust perspective matrix
            }
        }

        time = timer.getElapsedTime();
        rotation = sin(time.asSeconds()/2)*2;        //update viewmatrix variable to rotate the view
        setView(window, program); //apply rotation to view matrix

            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_data), vertex_data); //change data while the buffer is bind

            // Draw the triangle !
            glClearBufferfv(GL_COLOR, 0, clearColor);
            glClear(GL_DEPTH_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, faces.size()*3); // Starting from vertex 0; 3 vertices total -> 1 triangle

        window.display();
    }
    //should also delete vao and buffers (add to to-do list)
    glDeleteProgram(program);
    return 0;
}
