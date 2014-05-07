#include "stdafx.h"

#include <GL/gl.h>

#include "render_gl.h"
#include "opengl.h"


#include <gli/gli.hpp>
#include <gli/core/texture2d.hpp>// Requires OpenGL >= 1.1 to be included before this include



#define BUFFER_OFFSET(i) ((char *)NULL + (i))

int get_gl_shader_type(int type) {
	static const int gl_types[] = {
		GL_VERTEX_SHADER, 
		GL_FRAGMENT_SHADER,
		GL_GEOMETRY_SHADER
	};
	return gl_types[type];
}

const char* get_shader_typename(int type) {
	static const char* names[] = {
		"VERTEX", 
		"FRAGMENT",
		"GEOMETRY"
	};
	return names[type];
}

bool gpu_program::load(shader_desc* desc) {
	if (!desc->code) return false;

	bool fail = false;

	prog.id = glCreateProgram();

	for(;desc->code;++desc) {
		int shader_type = get_gl_shader_type(desc->type);
		GLuint s = glCreateShader(shader_type);		
		glShaderSource(s, 1, &desc->code, NULL);
		glCompileShader(s);

		GLint result;

		glGetShaderiv(s, GL_COMPILE_STATUS, &result);
		if (!result){
			static char infoLog[2048];
			GLint len;
			glGetShaderInfoLog(s, sizeof(infoLog), &len, infoLog);
			printf("%s Shader compile errors:\n %s", get_shader_typename(desc->type), infoLog);
			fail = true;
		}
		else {
			glAttachShader(prog.id, s);
		}
		glDeleteShader(s);
	}

	if (fail) {
		glDeleteProgram(prog.id);
		prog.id = -1;
		return false;
	}

	glLinkProgram(prog.id);

	GLint num_attribs = -1, num_uniforms = -1;
	glGetProgramiv(prog.id, GL_ACTIVE_ATTRIBUTES,  &num_attribs);
	glGetProgramiv(prog.id, GL_ACTIVE_UNIFORMS, &num_uniforms);

	printf("Program info:\nAtrribs:\n");
	for(int i = 0; i < num_attribs; ++i){
		char infoLog[2048];
		GLsizei length;
		GLint attrib_size;
		GLenum attrib_type;

		glGetActiveAttrib(prog.id, i, sizeof(infoLog), &length, &attrib_size, &attrib_type, infoLog);

		//const char* param_type_str = to_str(attrib_type);
		printf("#:%d name:%s type:%s size:%d\n", i, infoLog, "", attrib_size);
	}

	printf("Uniforms:\n");
	for(int i = 0; i < num_uniforms; ++i){
		char infoLog[2048];
		GLsizei length;
		GLint attrib_size;
		GLenum attrib_type;

		glGetActiveUniform(prog.id, i, sizeof(infoLog), &length, &attrib_size, &attrib_type, infoLog);

		//const char* param_type_str = to_str(attrib_type);
		printf("name:%s type:%s size:%d\n", infoLog, "", attrib_size);
	}
	return true;
}

gpu_program::gpu_program(const char* vertex, const char* fragment) {
	shader_desc desc[] = {
		{shader_desc::vertex, vertex, 0},
		{shader_desc::fragment, fragment, 0},
		{}
	};

	if (!load(desc))
		prog.p = 0;
}

gpu_program::~gpu_program() {
	if (is_valid())
		glDeleteProgram(prog.id);
}

gpu_program::handle gpu_program::get_handle(const char* name) {
	handle p;
	p.p = this;
	p.id = glGetUniformLocation(prog.id, name);
	p.name = name;
	return p;
}

void gpu_program::set(const char* name, const float* v, size_t size) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	glUniform1fv(p.id, size, v);
}

void gpu_program::set(const char* name, const glm::mat4& m) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	glUniformMatrix4fv(p.id, 1, GL_FALSE,  &m[0][0]);
}

void gpu_program::set(const char* name, const glm::vec4& v) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	glUniform4fv(p.id, 1, &v[0]);
}

void gpu_program::set(const char* name, const glm::vec2& v) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	glUniform2fv(p.id, 2, &v[0]);
}


void gpu_program::set(handle p, const glm::mat4& m) {
	if (p.id < 0) return;
	glUniformMatrix4fv(p.id, 1, GL_FALSE,  &m[0][0]);
}

void gpu_program::set(handle p, const glm::vec4& v) {
	if (p.id < 0) return;
	glUniform4fv(p.id, 1, &v[0]);
}

void gpu_program::set(handle p, const glm::vec2& v) {
	if (p.id < 0) return;
	glUniform2fv(p.id, 2, &v[0]);
}

void gpu_program::set(handle p, const float* v, size_t size) {
	if (p.id < 0) return;
	glUniform1fv(p.id, size, v);
}

void gpu_program::begin() {
	glUseProgram(prog.id);
}

void gpu_program::end() {
	glUseProgram(0);
	cur_texture_slot = 0;
}

//void gpu_program::set(handle p, texture_ptr t) {
//	if (p.id < 0) return;
//	glUniform1i(p.id, cur_texture_slot);
//	t->bind(cur_texture_slot);
//	cur_texture_slot++;
//}
//
//void gpu_program::set(handle p, texture_ptr t, size_t slot) {
//	if (p.id < 0) return;
//	glUniform1i(p.id, slot);
//	t->bind(slot);
//}
//
//void gpu_program::set(const char* name, texture_ptr t){
//	handle h = get_handle(name);
//	set(h, t);
//}


index_buffer::index_buffer(size_t size, void* ib_data) : gpu_buffer(size) {
	update(size, ib_data);
}

void index_buffer::update(size_t size, void* ib_data) {
	this->size = size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, h.id);
	if (size > 0)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, ib_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void index_buffer::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, h.id);
}

void index_buffer::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

vertex_buffer::vertex_buffer(size_t size, void* vb_data) : gpu_buffer(size){
	update(size, vb_data);
}

void vertex_buffer::update(size_t size, void* vb_data) {
	this->size = size;
	glBindBuffer(GL_ARRAY_BUFFER, h.id);
	if (size > 0)
		glBufferData(GL_ARRAY_BUFFER, size, vb_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vertex_buffer::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, h.id);
}

void vertex_buffer::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

gpu_buffer::gpu_buffer(size_t size_in_bytes) : size(size_in_bytes) { 
	h.id = 0; 
	glGenBuffers(1, &h.id);
}

gpu_buffer::~gpu_buffer() {
	glDeleteBuffers(1, &h.id);        
}

//texture::~texture() {
//	glDeleteTextures(1, &h.id);
//	h.id = -1;
//}
//
//void texture::bind(size_t slot){
//	glActiveTexture(GL_TEXTURE0 + slot);
//	glBindTexture(GL_TEXTURE_2D, h.id);
//}

vb_ptr vertex_buffer::create(size_t size, void* data) {
	return vb_ptr(new vertex_buffer(size, data));
}
ib_ptr index_buffer::create(size_t size, void* data) {
	return ib_ptr(new index_buffer(size, data));
}

gpu_program_ptr gpu_program::create(const char* vs, const char* fs) {
	gpu_program_ptr sp(new gpu_program(vs, fs));
	return sp->is_valid() ? sp : gpu_program_ptr();
}



mesh::mesh(const vertex_atrib* va, vb_ptr _vb, ib_ptr _ib) : vb(_vb), ib(_ib) {
	if (!ib) update_ib(0, 0);
	if (!vb) update_vb(0, 0);

	glGenVertexArrays(1, &vdecl.id);
	bind();

	vb->bind();

	while (va->index >= 0) {
		glVertexAttribPointer((GLuint)va->index, va->size, va->type, va->norm ? GL_TRUE : GL_FALSE, va->stride, BUFFER_OFFSET(va->offset));
		glEnableVertexAttribArray(va->index);
		++va;
	}

	vb->unbind();

	unbind();
}

void mesh::update_ib(size_t size, void* ib_data) {
	if (!ib)
		ib = index_buffer::create(size, ib_data);
	else
		ib->update(size, ib_data);
}

void mesh::update_vb(size_t size, void* vb_data) {
	if (!vb)
		vb = vertex_buffer::create(size, vb_data);
	else
		vb->update(size, vb_data);
}

void mesh::bind() {
	glBindVertexArray(vdecl.id);
	ib->bind();
}

void mesh::unbind() {
	ib->unbind();
	glBindVertexArray(0);
}

void mesh::draw(draw_mode_t mode) {
	static const GLenum gl_modes[] = { GL_POINTS, GL_POINTS, GL_LINES, GL_TRIANGLES };
	size_t num_primitives = (ib->size / sizeof(unsigned short))*mode;
	GLenum gl_mode = gl_modes[mode];
	bind();
	glDrawElements(gl_mode, num_primitives, GL_UNSIGNED_SHORT, 0);
	unbind();
}

mesh_ptr mesh::create(const vertex_atrib* va, vb_ptr vb, ib_ptr ib) {
	return mesh_ptr(new mesh(va, vb, ib));
}