#include <string>
#include <gl3w.h>
#include <iostream>
#include <fstream>
#include <vector>



class Shader{ //a utility class to load and compile shaders
    public:
    Shader(){
    program = glCreateProgram();
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //for now only vertex and frag shaders
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    };

        void loadVertexShader(std::string filepath){

            GLint Result = GL_FALSE; //compile testing variables
            int InfoLogLength;


            std::string VertexShaderCode; //loading source form a file
            std::ifstream VertexShaderStream(filepath.c_str(), std::ios::in);
            if(VertexShaderStream.is_open())
            {
                std::string Line = "";
                while(getline(VertexShaderStream, Line))    //formating the code
                    VertexShaderCode += "\n" + Line;
                VertexShaderStream.close();
            }

            printf("Compiling shader : %s\n", filepath.c_str());
            char const * VertexSourcePointer = VertexShaderCode.c_str();
            glShaderSource(vertexShader, 1, &VertexSourcePointer , NULL);
            glCompileShader(vertexShader);


            // Check Vertex Shader
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<char> VertexShaderErrorMessage(InfoLogLength);  //log errors if any
            glGetShaderInfoLog(vertexShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            if(&VertexShaderErrorMessage[0])fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
            else printf("Success\n");
        }

        void loadFragmentShader(std::string filepath){

            GLint Result = GL_FALSE; //compile testing variables
            int InfoLogLength;

            // Read the Fragment Shader code from the file
            std::string FragmentShaderCode;
            std::ifstream FragmentShaderStream(filepath.c_str(), std::ios::in);
            if(FragmentShaderStream.is_open()){
                std::string Line = "";
                while(getline(FragmentShaderStream, Line))
                FragmentShaderCode += "\n" + Line;
                FragmentShaderStream.close();
            }

            // Compile Fragment Shader
            printf("Compiling shader : %s\n", filepath.c_str());
            char const * FragmentSourcePointer = FragmentShaderCode.c_str();
            glShaderSource(fragmentShader, 1, &FragmentSourcePointer , NULL);
            glCompileShader(fragmentShader);


            // Check Fragment Shader
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
            glGetShaderInfoLog(fragmentShader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            if(&FragmentShaderErrorMessage[0])fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
            else printf("Success\n");
        }

        GLuint getProgram(){

            GLint Result = GL_FALSE; //compile testing variables
            int InfoLogLength;

            // Link the program
            printf("Linking program\n");
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            glLinkProgram(program);

            // Check the program
            glGetProgramiv(program, GL_LINK_STATUS, &Result);
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<char> ProgramErrorMessage(std::max(InfoLogLength, int(1)) );
            glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return program;
        }
    private:
        GLuint vertexShader;
        GLuint fragmentShader;
        GLuint program;
};
