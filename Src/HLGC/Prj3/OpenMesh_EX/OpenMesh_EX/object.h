#pragma once

#include <iostream>
#include <vector>
#include "glew//include/GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "LoadShaders.h"

using namespace glm;
class object
{

public:
	object();
	void initialize();
	void setShader(char* vert, char* frag);

	void setPoint(glm::vec3 point);
	void setColor(glm::vec3 color);
	void clear();
	mat4 setTranslate(float x, float y, float z); //����
	mat4 setRotate(float angle, float x, float y, float z); //����
	mat4 setScale(float x, float y, float z); //�Y��
	//T[]*R[]*S[]*M[]
	void render(GLenum type,mat4 projectionMatrix, mat4 modelMatrix, mat4 viewMatrix);
	size_t getPointSize();
private:
	std::vector<glm::vec3> Points;
	std::vector<glm::vec3> PColors;
	GLuint shaderProgram;
	GLuint VAO;
	GLuint pVBOs; //�I�y�� buffer
	GLuint pcVBOs; //�C�� buffer

};
