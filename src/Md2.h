#pragma once

#define GLEW_STATIC
#ifdef __APPLE__
#include <glad/glad.h>
#else
#include "GL/glew.h"
#endif
#include "GLFW/glfw3.h"


#include <cstdio>
#include <cstdlib>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <map>
#include <memory>

class ShaderProgram;
class Texture2D;

namespace md2model
{
struct header
{
    int id;
    int version;
    int twidth;
    int theight;
    int framesize;
    int textures;
    int vNum;
    int tNum;
    int fNum;
    int numGLcmds;
    int Number_Of_Frames;
    int offsetSkins;
    int offsetTCoord;
    int offsetIndx;
    int offsetFrames;
    int offsetGLcmds;
    int offsetEnd;
};

struct textcoord
{
    float s;
    float t;
};

struct textindx
{
    short s;
    short t;
};

struct framePoint_t
{
    unsigned char v[3];
    unsigned char normalIndex;
};

struct frame
{
    float scale[3];
    float translate[3];
    char name[16];
    framePoint_t fp[1];
};

struct mesh
{
    unsigned short meshIndex[3];
    unsigned short stIndex[3];
};

struct vector
{
    float point[3];
};

struct modData
{
    int numFrames;
    int numPoints;
    int numTriangles;
    int numST;
    int frameSize;
    int twidth;
    int theight;
    int currentFrame;
    int nextFrame;
    float interpol;
    mesh *triIndx;
    textcoord *st;
    md2model::vector *pointList;
    float x, y, z;
    float nextX, nextY, nextZ;
    float radius;
    float dist_to_player;
    int state;
    float speed;
};

class Md2
{
  private:
    void LoadModel(char *md2FileName);
    void LoadTexture(char* textureFileName);
    void InitBuffer();

    std::unique_ptr<modData> m_model;
    std::unique_ptr<Texture2D> m_texture;
    std::vector<GLuint> m_vaoIndices;
	std::vector<GLuint> m_vboIndices;
    std::unique_ptr<ShaderProgram> m_shaderProgram;
    std::map<int, std::pair<int,int>> m_frameIndices;
    bool m_pause;
    glm::vec3 m_position;
    bool m_modelLoaded;
    bool m_textureLoaded;
    bool m_bufferInitialized;

  public:
    Md2(char *md2FileName, char* textureFileName);
    ~Md2();
    //The frame parameter start at 0
    void Draw(int frame, float angle, float interpolation, glm::mat4 &view, glm::mat4 &projection);
    void SetPause(bool pause){ m_pause = pause; }
};
} // namespace md2model
