#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <sstream>
#include <array>
class OBJ{
    public:
    std::vector<glm::vec3> readVerts(std::string filepath){
        std::vector<glm::vec3> verts;
        std::ifstream objFile(filepath.c_str(), std::ios::in);
        if(objFile.is_open())
            {
                std::string Line = "";
                while(getline(objFile, Line)){
                    if(Line[0]=='v' && Line[1]==' '){
                        std::stringstream ss;
                        ss.str(Line);
                        char c;
                        float vert1, vert2, vert3;
                        ss>>c>>vert1>>vert2>>vert3;
                        glm::vec3 vert;
                        vert[0] = vert1;
                        vert[1] = vert2;
                        vert[2] = vert3;
                        verts.push_back(vert);
                    }
                }
                objFile.close();
            }
            return verts;
    }

    std::vector<glm::vec3> readFaces(std::string filepath){
        std::vector<glm::vec3> faces;
        std::ifstream objFile(filepath.c_str(), std::ios::in);
        if(objFile.is_open())
            {
                std::string Line = "";
                while(getline(objFile, Line)){
                    if(Line[0]=='f' && Line[1]==' '){
                                std::stringstream ss;
                                ss.str(Line);
                                char c;
                                int vert1, vert2, vert3; //each line of a triangulated obj has 1 face and 3 verts that are included in the face
                                int uv;
                                int face;
                                ss>>c>>vert1>>c>>uv>>c>>face>>vert2>>c>>uv>>c>>face>>vert3>>c>>uv>>c>>face;//"f 1/1/1 5/2/1 6/3/1" reading the obj formatting
                                glm::vec3 poly;
                                poly[0] = vert1-1; //obj files count 1 and up, whilst computers count from 0
                                poly[1] = vert2-1;
                                poly[2] = vert3-1;
                                //printf("%f, %f, %f\n",poly[0],poly[1],poly[2]);
                                faces.push_back(poly);
                        }
                }
                objFile.close();
            }
            return faces;
    }
std::vector<int> readVerTex(std::string filepath){
    std::vector<int> vertToTexCoord;
        std::ifstream objFile(filepath.c_str(), std::ios::in);
        if(objFile.is_open()){
                std::string Line = "";
                while(getline(objFile, Line)){
                    if(Line[0]=='f' && Line[1]==' '){
                                std::stringstream ss;
                                ss.str(Line);
                                char c;
                                int vert1, vert2, vert3;
                                int uv1, uv2, uv3;
                                int face;
                                ss>>c>>vert1>>c>>uv1>>c>>face>>vert2>>c>>uv2>>c>>face>>vert3>>c>>uv3>>c>>face;
                                vertToTexCoord.push_back(uv1-1);
                                vertToTexCoord.push_back(uv2-1);
                                vertToTexCoord.push_back(uv3-1);
                        }
                }
                objFile.close();
            }
            return vertToTexCoord;
    }


std::vector<glm::vec2> readUV(std::string filepath){
        std::vector<glm::vec2> UVs;
        std::ifstream objFile(filepath.c_str(), std::ios::in);
        if(objFile.is_open()){
                std::string Line = "";
                while(getline(objFile, Line)){
                    if(Line[0]=='v' && Line[1]=='t' && Line[2]==' '){
                                std::stringstream ss;
                                ss.str(Line);
                                char c;
                                float x, y; //each line contains "vt x y"
                                ss>>c>>c>>x>>y;
                                glm::vec2 vt(x, y);
                                UVs.push_back(vt);
                        }
                }
                objFile.close();
            }
            return UVs;
    }
};
