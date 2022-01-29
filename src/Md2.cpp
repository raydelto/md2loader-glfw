#include "Md2.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

using namespace md2model;

Md2::Md2(char *md2FileName, char* textureFileName): m_texture(std::make_unique<Texture2D>()) , 
													m_shaderProgram(std::make_unique<ShaderProgram>()),
													m_pause(false),
													m_position(glm::vec3(0.0f, 0.0f, -25.0f)),
													m_modelLoaded(false),
													m_textureLoaded(false),
													m_bufferInitialized(false)
{
	LoadModel(md2FileName);
	LoadTexture(textureFileName);
	InitBuffer();
	m_shaderProgram->loadShaders("shaders/basic.vert", "shaders/basic.frag");	
}

Md2::~Md2()
{
	// Clean up
	for(size_t i = 0 ; i < m_vaoIndices.size(); i++)
	{
		glDeleteVertexArrays(1, &m_vaoIndices[i]);
		glDeleteBuffers(1, &m_vboIndices[i]);
	}
}

void Md2::Draw(int frame, float angle, float interpolation, glm::mat4 &view, glm::mat4 &projection)
{
		assert(m_modelLoaded && m_textureLoaded && m_bufferInitialized);
		m_texture->bind(0);
		glm::mat4 model;

		// Rotates around the cube center
		model = glm::translate(model, m_position) * glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(model, glm::vec3(0.3,0.3,0.3));

		m_shaderProgram->use();
		m_shaderProgram->setUniform("model", model);
		m_shaderProgram->setUniform("view", view);
		m_shaderProgram->setUniform("projection", projection);
		m_shaderProgram->setUniform("modelView",  view * model);

		glBindVertexArray(m_vaoIndices[frame]);
		//  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto count = m_frameIndices[frame].second - m_frameIndices[frame].first + 1;
		m_shaderProgram->setUniform("interpolation", interpolation);
		glDrawArrays(GL_TRIANGLES, 0,count);
		glBindVertexArray(0);
}

void Md2::LoadTexture(char* textureFileName)
{
	m_texture->loadTexture(textureFileName, true);
	m_textureLoaded = true;
}

void Md2::InitBuffer()
{
	std::vector<float> md2Vertices;
	int startFrame = 0;
	int endFrame = m_model->numFrames - 1;
	md2model::vector *currentFrame;
	md2model::vector *nextFrame;
	m_model->currentFrame = startFrame;
	m_model->interpol = 0.0f;

	int vertexIndex = 0;
	int startVertex = 0;
	
	//fill buffer
	while(m_model->currentFrame <= endFrame)
	{
		currentFrame = &m_model->pointList[m_model->numPoints * m_model->currentFrame];
		nextFrame = m_model->currentFrame == endFrame?
						 &m_model->pointList[m_model->numPoints * startFrame] :
						 &m_model->pointList[m_model->numPoints * (m_model->currentFrame + 1)] ;
		startVertex = vertexIndex;
		for (int index = 0; index < m_model->numTriangles; index++)
		{
			
			//Start of the vertex data
			for(int p = 0 ; p < 3; p++)
			{
				//current frame
				for(int j = 0 ; j < 3; j++)
				{
					//vertices
					md2Vertices.emplace_back(currentFrame[m_model->triIndx[index].meshIndex[p]].point[j]);
				}
				
				//next frame
				for(int j = 0 ; j < 3; j++)
				{
					//vertices
					md2Vertices.emplace_back(nextFrame[m_model->triIndx[index].meshIndex[p]].point[j]);
				}				
				
				//tex coords
				md2Vertices.emplace_back(m_model->st[m_model->triIndx[index].stIndex[p]].s);
				md2Vertices.emplace_back(m_model->st[m_model->triIndx[index].stIndex[p]].t);
				vertexIndex++;
			}
			//End of the vertex data
		}
		m_frameIndices[m_model->currentFrame] = {startVertex, vertexIndex - 1};
		m_model->currentFrame++;
	}


	int frameIndex = startFrame;
	unsigned int vbo,  vao;

	for(int i = 0 ; i < m_model->numFrames; i++)
	{
		glGenBuffers(1, &vbo);					// Generate an empty vertex buffer on the GPU
		glGenVertexArrays(1, &vao);				// Tell OpenGL to create new Vertex Array Object

		auto count = m_frameIndices[frameIndex].second - m_frameIndices[frameIndex].first + 1;		
		glBindBuffer(GL_ARRAY_BUFFER, vbo);		// "bind" or set as the current buffer we are working with
		glBufferData(GL_ARRAY_BUFFER,  count * sizeof(float) * 8, &md2Vertices[m_frameIndices[frameIndex].first * 8 ], GL_STATIC_DRAW);	// copy the data from CPU to GPU

		glBindVertexArray(vao);					// Make it the current one
		// Current Frame Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
		glEnableVertexAttribArray(0);

		// Next  Frame Position attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// Texture Coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		frameIndex++;
		m_vaoIndices.emplace_back(vao);
		m_vboIndices.emplace_back(vbo);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(0);					// unbind to make sure other code doesn't change it
	m_bufferInitialized = true;
}

void Md2::LoadModel(char *md2FileName)
{
	FILE *fp;
	int length;

	char *buffer;

	header *head;
	textindx *stPtr;

	frame *fra;
	md2model::vector *pntlst;
	mesh *triIndex, *bufIndexPtr;

#ifdef WIN32
	fopen_s(&fp, md2FileName, "rb");
#else
	fp = fopen(md2FileName, "rb");
#endif
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer = (char *)malloc(length + 1);
	fread(buffer, sizeof(char), length, fp);

	head = (header *)buffer;
	m_model.reset((modData *)malloc(sizeof(modData)));

	m_model->pointList = (md2model::vector *)malloc(sizeof(md2model::vector) * head->vNum * head->Number_Of_Frames);
	m_model->numPoints = head->vNum;
	m_model->numFrames = head->Number_Of_Frames;
	m_model->frameSize = head->framesize;

	for (int count = 0; count < head->Number_Of_Frames; count++)
	{
		fra = (frame *)&buffer[head->offsetFrames + head->framesize * count];
		pntlst = (md2model::vector *)&m_model->pointList[head->vNum * count];
		for (int count2 = 0; count2 < head->vNum; count2++)
		{
			pntlst[count2].point[0] = fra->scale[0] * fra->fp[count2].v[0] + fra->translate[0];
			pntlst[count2].point[1] = fra->scale[1] * fra->fp[count2].v[1] + fra->translate[1];
			pntlst[count2].point[2] = fra->scale[2] * fra->fp[count2].v[2] + fra->translate[2];
		}
	}

	m_model->st = (textcoord *)malloc(sizeof(textcoord) * head->tNum);
	m_model->numST = head->tNum;
	stPtr = (textindx *)&buffer[head->offsetTCoord];

	for (int count = 0; count < head->tNum; count++)
	{
		m_model->st[count].s = (float)stPtr[count].s / (float)head->twidth;
		m_model->st[count].t = (float)stPtr[count].t / (float)head->theight;
	}

	triIndex = (mesh *)malloc(sizeof(mesh) * head->fNum);
	m_model->triIndx = triIndex;
	m_model->numTriangles = head->fNum;
	bufIndexPtr = (mesh *)&buffer[head->offsetIndx];

	for (int count = 0; count < head->Number_Of_Frames; count++)
	{
		for (int count2 = 0; count2 < head->fNum; count2++)
		{
			triIndex[count2].meshIndex[0] = bufIndexPtr[count2].meshIndex[0];
			triIndex[count2].meshIndex[1] = bufIndexPtr[count2].meshIndex[1];
			triIndex[count2].meshIndex[2] = bufIndexPtr[count2].meshIndex[2];

			triIndex[count2].stIndex[0] = bufIndexPtr[count2].stIndex[0];
			triIndex[count2].stIndex[1] = bufIndexPtr[count2].stIndex[1];
			triIndex[count2].stIndex[2] = bufIndexPtr[count2].stIndex[2];
		}
	}

	m_model->currentFrame = 0;
	m_model->nextFrame = 1;
	m_model->interpol = 0.0;

	fclose(fp);
	m_modelLoaded = true;
}
