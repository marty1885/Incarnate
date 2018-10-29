#ifndef SURFACE_H
#define SURFACE_H

#include "GLdef.hpp"

class Surface
{
public:
	Surface();
	virtual ~Surface();
	void setTexture(const float* buffer, int width, int height);

	void render();
protected:
	bool directUpdateable(GLint type, int width, int height);
	GLuint textureID = 0;

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
};

#endif
