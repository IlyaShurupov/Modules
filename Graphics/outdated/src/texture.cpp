
#include "texture.h"

#include "map.h"

#include "stringt.h"

#include "shader.h"

#include "GraphicsLibApi.h"

const char* texture_vertex =
#ifdef ENV_OS_ANDROID
"#version 300 es\n"
"precision mediump float;\n"
#else
"#version 330 core\n"
#endif
"layout(location = 0) in vec3 vPos;\n"
"out vec2 UV;\n"
"void main() {\n"
"	gl_Position = vec4(vPos, 1);\n"
"	UV = (vPos.xy + vec2(1, 1)) / 2.0;\n"
"}\n";

const char* texture_fragment =
#ifdef ENV_OS_ANDROID
"#version 300 es\n"
"precision mediump float;\n"
#else
"#version 330 core\n"
#endif
"in vec2 UV;\n"
"out vec4 color;\n"
"uniform sampler2D renderedTexture;\n"
"uniform float time;\n"
"void main() {\n"
"	vec4 texColor = texture(renderedTexture, UV);\n"
" if (texColor.a < 0.2)\n"
"		discard;\n"
" color = texColor;\n"
"}\n";

namespace tp {

	using namespace glw;

	GLuint texture::getid() { return id; }

	texture::texture() {
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	texture::~texture() { glDeleteTextures(1, &id); }

	alni texture::sizeAllocatedMem() {
		return sizeof(GLuint);
	}

	alni texture::sizeUsedMem() {
		return sizeof(GLuint);
	}


	void texture::update(const Array2D<rgba>& buff) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)buff.size().x, (GLsizei)buff.size().y, 0, GL_RGBA, GL_FLOAT, buff.buff());
	}

	void texture::draw(const GLuint& out) {
		draw_texture(out, id);
	}


	struct texture_drawer_data {

		HashMap<GLuint, string> textures;

		GLuint quad_VertexArrayID;
		GLuint quad_vertexbuffer;
		GLuint texID;
		glw::shader shader;

		const GLfloat g_quad_vertex_buffer_data[18] = {
				-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
				-1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
		};

		texture_drawer_data() : shader(texture_vertex, NULL, texture_fragment, false) {
			// The fullscreen quad's FBO
			glGenVertexArrays(1, &quad_VertexArrayID);
			glBindVertexArray(quad_VertexArrayID);

			glGenBuffers(1, &quad_vertexbuffer);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data),
				g_quad_vertex_buffer_data, GL_STATIC_DRAW);

			texID = shader.getu("renderedTexture");
		}

		~texture_drawer_data() {
			glDeleteBuffers(1, &quad_vertexbuffer);
			glDeleteVertexArrays(1, &quad_VertexArrayID);

			for (auto tex : textures) {
				glDeleteTextures(1, &tex->val);
			}
		}
	};

	texture_drawer_data* texdd = NULL;

	void texture::init() {
		if (!texdd) texdd = new texture_drawer_data();
	}

	void texture::deinit() {
		if (texdd) delete texdd;
		texdd = NULL;
	}

	void texture::draw_texture(uint4 out, uint4 in) {
		assert(in);

		// Render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, out);

		// Use our shader
		texdd->shader.bind();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, in);
		// Set our "renderedTexture" sampler to use Texture Unit 0
		glUniform1i(texdd->texID, 0);

		// glUniformMatrix4fv(texdd->rect_mat, 1, GL_FALSE, &tmat[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, texdd->quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Draw the triangles. 2*3 indices starting at 0 -> 2 triangles
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);

		texdd->shader.unbind();
	}

	GLuint load_texture(string name) {
		GLuint tex_2d = 0;
		RelAssert(0 && "incomplete compilation - no SOIL support added");

		if (0) {
			//auto document = lunasvg::Document::loadFromFile("tiger.svg");
			//auto bitmap = document->renderToBitmap();
		}
		else {
			/*
			tex_2d = SOIL_load_OGL_texture(
				name.cstr(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB |
				SOIL_FLAG_COMPRESS_TO_DXT);

			if (0 == tex_2d) {
				printf("SOIL loading error: '%s'\n", SOIL_last_result());
			}
			*/
		}

		return tex_2d;
	}

	GLuint texture::get_tex(const char* TexId) {
		GLuint out = 0;
		alni idx = texdd->textures.presents(TexId);
		if (idx != -1) {
			out = texdd->textures.get(TexId);
		}
		else {
			out = load_texture(TexId);
			texdd->textures.put(TexId, out);
		}
		return out;
	}

	void texture::drawCurcle(vec2f pos, double radius, rgba col) {
#ifndef ENV_OS_ANDROID
		static alni precision = 40;

		glColor4f(col.r, col.g, col.b, col.a);

		double twicePi = 2.0 * 3.142;

		glBegin(GL_TRIANGLE_FAN);  // BEGIN CIRCLE
		glVertex2f(pos.x, pos.y);  // center of circle

		for (alni i = 0; i <= precision; i++) {
			glVertex2f((GLfloat)(pos.x + (radius * cos(i * twicePi / precision))),
				(GLfloat)(pos.y + (radius * sin(i * twicePi / precision))));
		}
		glEnd();  // END
#endif 
	}
};


