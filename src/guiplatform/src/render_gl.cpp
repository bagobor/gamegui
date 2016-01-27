#include "stdafx.h"

#include <GL/gl.h>
#include "opengl.h"

#include "../renderer_ogl.h"
#include "render_gl.h"


void CheckOpenGLError(const char* stmt, const char* fname, int line);

#ifdef _DEBUG
#define GL_CHECK(stmt) do { \
	stmt; \
	CheckOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)
#else
#define GL_CHECK(stmt) stmt
#endif


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

	GL_CHECK(prog.id = glCreateProgram());

	for(;desc->code;++desc) {
		int shader_type = get_gl_shader_type(desc->type);
		GLuint s = 0;
		GL_CHECK(s = glCreateShader(shader_type));
		GL_CHECK(glShaderSource(s, 1, &desc->code, NULL));
		GL_CHECK(glCompileShader(s));

		GLint result = 0;

		GL_CHECK(glGetShaderiv(s, GL_COMPILE_STATUS, &result));
		if (!result){
			static char infoLog[2048];
			GLint len = 0;
			GL_CHECK(glGetShaderInfoLog(s, sizeof(infoLog), &len, infoLog));
			printf("%s Shader compile errors:\n %s", get_shader_typename(desc->type), infoLog);
			fail = true;
		}
		else {
			GL_CHECK(glAttachShader(prog.id, s));
		}
		GL_CHECK(glDeleteShader(s));
	}

	if (fail) {
		GL_CHECK(glDeleteProgram(prog.id));
		prog.id = -1;
		return false;
	}

	GL_CHECK(glLinkProgram(prog.id));

	GLint num_attribs = -1, num_uniforms = -1;
	GL_CHECK(glGetProgramiv(prog.id, GL_ACTIVE_ATTRIBUTES, &num_attribs));
	GL_CHECK(glGetProgramiv(prog.id, GL_ACTIVE_UNIFORMS, &num_uniforms));

	printf("Program info:\nAtrribs:\n");
	for(int i = 0; i < num_attribs; ++i){
		char infoLog[2048];
		GLsizei length;
		GLint attrib_size;
		GLenum attrib_type;

		GL_CHECK(glGetActiveAttrib(prog.id, i, sizeof(infoLog), &length, &attrib_size, &attrib_type, infoLog));

		unsigned location_id = 0;
		GL_CHECK(location_id = glGetAttribLocation(prog.id, infoLog));

		m_attribs[infoLog] = location_id;

		//const char* param_type_str = to_str(attrib_type);
		printf("#:%d name:%s type:%s size:%d\n", i, infoLog, "", attrib_size);
	}

	printf("Uniforms:\n");
	for(int i = 0; i < num_uniforms; ++i){
		char infoLog[2048];
		GLsizei length = 0;
		GLint attrib_size = 0;
		GLenum attrib_type = 0;

		GL_CHECK(glGetActiveUniform(prog.id, i, sizeof(infoLog), &length, &attrib_size, &attrib_type, infoLog));

		unsigned location_id = 0;
		GL_CHECK(location_id = glGetUniformLocation(prog.id, infoLog));
		m_uniforms[infoLog] = location_id;

		//const char* param_type_str = to_str(attrib_type);
		printf("name:%s type:%s size:%d\n", infoLog, "", attrib_size);
	}
	return true;
}

gpu_program::gpu_program(const char* vertex, const char* fragment) 
{
	shader_desc desc[] = {
		{shader_desc::vertex, vertex, 0},
		{shader_desc::fragment, fragment, 0},
		{}
	};

	if (!load(desc))
		prog.p = 0;
}

gpu_program::~gpu_program() {
	if (is_valid()) {
		GL_CHECK(glDeleteProgram(prog.id));
	}
}

gpu_program::handle gpu_program::get_handle(const char* name) {
	handle p;
	p.p = this;
	GL_CHECK(p.id = glGetUniformLocation(prog.id, name));
	p.name = name;
	return p;
}

void gpu_program::set(const char* name, const float* v, size_t size) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	GL_CHECK(glUniform1fv(p.id, size, v));
}

void gpu_program::set(const char* name, const glm::mat4& m) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	GL_CHECK(glUniformMatrix4fv(p.id, 1, GL_FALSE, &m[0][0]));
}

void gpu_program::set(const char* name, const glm::vec4& v) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	GL_CHECK(glUniform4fv(p.id, 1, &v[0]));
}

void gpu_program::set(const char* name, const glm::vec2& v) {
	handle p = get_handle(name);
	if (p.id < 0) return;
	glUniform2fv(p.id, 1, &v[0]);
}


void gpu_program::set(handle p, const glm::mat4& m) {
	if (p.id < 0) return;
	GL_CHECK(glUniformMatrix4fv(p.id, 1, GL_FALSE, &m[0][0]));
}

void gpu_program::set(handle p, const glm::vec4& v) {
	if (p.id < 0) return;
	GL_CHECK(glUniform4fv(p.id, 1, &v[0]));
}

void gpu_program::set(handle p, const glm::vec2& v) {
	if (p.id < 0) return;
	GL_CHECK(glUniform2fv(p.id, 2, &v[0]));
}

void gpu_program::set(handle p, const float* v, size_t size) {
	if (p.id < 0) return;
	GL_CHECK(glUniform1fv(p.id, size, v));
}

void gpu_program::begin() {
	GL_CHECK(glUseProgram(prog.id));
}

void gpu_program::end() {
	GL_CHECK(glUseProgram(0));
	m_texture_slots.clear();
}

void gpu_program::set(handle p, gui::ogl_platform::TextureOGL* t) {
	if (p.id < 0) return;

	size_t texture_slot = 0;

	auto it = m_texture_slots.find(p.name);
	if (it != m_texture_slots.end()) {
		texture_slot = it->second;
	}
	else {
		texture_slot = m_texture_slots.size();
		m_texture_slots[p.name] = texture_slot;
	}

	t->bind(texture_slot);
	GL_CHECK(glUniform1i(p.id, texture_slot));
}

void gpu_program::set(handle p, gui::ogl_platform::TextureOGL* t, size_t slot) {
	if (p.id < 0) return;
	t->bind(slot);
	GL_CHECK(glUniform1i(p.id, slot));
}

void gpu_program::set(const char* name, gui::ogl_platform::TextureOGL* t, size_t slot) {
	handle h = get_handle(name);
	set(h, t, slot);
}

void gpu_program::set(const char* name, gui::ogl_platform::TextureOGL* t) {
	handle h = get_handle(name);
	set(h, t);
}

index_buffer::index_buffer(size_t size, void* ib_data) : gpu_buffer(size) {
	update(size, ib_data);
}

void index_buffer::update(size_t size, void* ib_data) {
	this->size = size;
	bind();
	if (size > 0) {
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, ib_data, GL_STATIC_DRAW));
	}
	unbind();
}

void index_buffer::bind() {
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, h.id));
}

void index_buffer::unbind() {
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

vertex_buffer::vertex_buffer(size_t size, void* vb_data, bool is_dynamic) : gpu_buffer(size){
	update(size, vb_data, is_dynamic);
}

void vertex_buffer::update(size_t size, void* vb_data, bool is_dynamic) {
	this->size = size;
	bind();
	//TODO: check capacity !
	if (size > 0) {
		GL_CHECK(glBufferData(GL_ARRAY_BUFFER, size, vb_data, is_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	}
	unbind();
}

void vertex_buffer::bind() {
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, h.id));
}

void vertex_buffer::unbind() {
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

gpu_buffer::gpu_buffer(size_t size_in_bytes) : size(size_in_bytes) { 
	h.id = 0; 
	GL_CHECK(glGenBuffers(1, &h.id));
}

gpu_buffer::~gpu_buffer() {
	GL_CHECK(glDeleteBuffers(1, &h.id));
}


vb_ptr vertex_buffer::create(size_t size, void* data, bool is_dynamic) {
	return vb_ptr(new vertex_buffer(size, data, is_dynamic));
}
ib_ptr index_buffer::create(size_t size, void* data) {
	return ib_ptr(new index_buffer(size, data));
}

gpu_program_ptr gpu_program::create(const char* vs, const char* fs) {
	gpu_program_ptr sp(new gpu_program(vs, fs));
	return sp->is_valid() ? sp : gpu_program_ptr();
}

mesh::mesh(const vertex_atrib* va, vb_ptr _vb, ib_ptr _ib) : vb(_vb), ib(_ib) {
	vdecl.p = nullptr;
	if (!ib) update_ib(0, 0);
	if (!vb) update_vb(0, 0);

	for (size_t i = 0; va[i].name; ++i) {
		m_atribs.push_back(va[i]);
	}
}

void mesh::update_ib(size_t size, void* ib_data) {
	if (!ib)
		ib = index_buffer::create(size, ib_data);
	else
		ib->update(size, ib_data);
}

void mesh::update_vb(size_t size, void* vb_data, bool is_dynamic) {
	if (!vb)
		vb = vertex_buffer::create(size, vb_data, is_dynamic);
	else
		vb->update(size, vb_data, is_dynamic);
}

void mesh::bind() {
	GL_CHECK(glBindVertexArray(vdecl.id));
	ib->bind();	
}

void mesh::unbind() {
	ib->unbind();
	GL_CHECK(glBindVertexArray(0));
}

void mesh::setShader(gpu_program_ptr shader) {
	if (this->shader == shader) return;
	this->shader = shader;

	if (vdecl.id > 0) {
		GL_CHECK(glDeleteVertexArrays(1, &vdecl.id));
	}

	GL_CHECK(glGenVertexArrays(1, &vdecl.id));
	bind();
	vb->bind();

	for (auto va : m_atribs) {
		auto it = shader->m_attribs.find(va.name);
		if (it == shader->m_attribs.end()) continue; // skip attribute
		unsigned attrib_index = it->second;
		GL_CHECK(glVertexAttribPointer((GLuint)attrib_index, va.size, va.type, va.norm ? GL_TRUE : GL_FALSE, va.stride, BUFFER_OFFSET(va.offset)));
		GL_CHECK(glEnableVertexAttribArray(attrib_index));
	}

	vb->unbind();
	unbind();
}

void mesh::draw(draw_mode_t mode) {
	static const GLenum gl_modes[] = { GL_POINTS, GL_POINTS, GL_LINES, GL_TRIANGLES };
	size_t num_primitives = (ib->size / sizeof(unsigned short))*mode;
	GLenum gl_mode = gl_modes[mode];
	bind();
	GL_CHECK(glDrawElements(gl_mode, num_primitives, GL_UNSIGNED_SHORT, 0));
	unbind();
}

mesh_ptr mesh::create(const vertex_atrib* va, vb_ptr vb, ib_ptr ib) {
	return mesh_ptr(new mesh(va, vb, ib));
}