#pragma once

#include "opengl.h"

#include <glm/glm.hpp>

union platform_handle {
	void* p;
	size_t id;
};

typedef std::shared_ptr<struct vertex_buffer> vb_ptr;
typedef std::shared_ptr<struct index_buffer> ib_ptr;
typedef std::shared_ptr<struct mesh> mesh_ptr;
typedef std::shared_ptr<struct gpu_program> gpu_program_ptr;

//struct texture {
//	static texture_ptr create(const char* filename);
//	virtual ~texture();
//
//	void bind(size_t slot);
//
//	size_t width, height;
//	platform_handle h;
//};

struct gpu_buffer {
	gpu_buffer(size_t size_in_bytes);
	virtual ~gpu_buffer();
		
	platform_handle h;
	//size in bytes
	size_t size;
};

struct vertex_buffer : public gpu_buffer {
private:
	explicit vertex_buffer(size_t size, void* vb_data = 0);
public:
	static vb_ptr create(size_t size, void* data = 0);
	
	void update(size_t size, void* vb_data);
	void bind();
	void unbind();
};

struct index_buffer : public gpu_buffer {
private:
	explicit index_buffer(size_t size, void* ib_data = 0);
public:
	static ib_ptr create(size_t size, void* data = 0);

	void update(size_t size, void* ib_data);
	void bind();
	void unbind();
};

struct vertex_atrib {
	int index;
	int size;
	GLenum type;
	bool norm;
	int stride;
	size_t offset;
};

struct mesh {
protected:
	explicit mesh(const vertex_atrib* va, vb_ptr vb = vb_ptr(), ib_ptr ib = ib_ptr());
public:
	static mesh_ptr create(const vertex_atrib* va, vb_ptr vb = vb_ptr(), ib_ptr ib = ib_ptr());

	enum draw_mode_t {
		Points = 1, Lines = 2, Triangles = 3
	};

	void update_ib(size_t size, void* ib_data);
	void update_vb(size_t size, void* vb_data);
	void bind();
	void unbind();
	void draw(draw_mode_t mode);

	vb_ptr vb;
	ib_ptr ib;

	platform_handle vdecl; //vao
};

struct gpu_program {
	struct handle {
		gpu_program* p;
		std::string name;
		union { int id; void* data; };
		void set(const float* v, size_t size) {p->set(*this, v, size);}
		void set(const glm::mat4& m) {p->set(*this, m);}
		void set(const glm::vec4& v) {p->set(*this, v);}
		void set(const glm::vec2& v) { p->set(*this, v); }
		void set(float v) {p->set(*this, v);}
		
		//void set(texture_ptr t) {p->set(*this, t);}
		//void set(texture_ptr t, size_t slot) {p->set(*this, t, slot);}
	};

	struct shader_desc {
		enum  shader_type {
			vertex = 0,
			fragment,
			geometry
		};
		shader_type type;
		const char* code;
		size_t line;
	};
	// null terminated array of shaders_desc
	explicit gpu_program(const shader_desc* desc);
	gpu_program(const char* vertex, const char* fragment);	
	~gpu_program();

	static gpu_program_ptr create(const char* vs, const char* fs);
	static gpu_program_ptr create(const shader_desc* desc);

	handle get_handle(const char* name);

	bool is_valid() const {return !(!prog.p);}

	void set(const char* name, const float* v, size_t size);
	void set(const char* name, const glm::mat4& m);
	void set(const char* name, const glm::vec4& v);
	void set(const char* name, const glm::vec2& v);

	void set(handle p, const float* v, size_t size);
	void set(handle p, const glm::mat4& m);
	void set(handle p, const glm::vec4& v);
	void set(handle p, const glm::vec2& v);
	void set(handle p, float value);

	//void set(handle p, texture_ptr t);
	//void set(handle p, texture_ptr t, size_t slot);

	//void set(const char* name, texture_ptr t);
	
	void begin();
	void end();

private:
	bool load(shader_desc* desc);
		
private:
	platform_handle prog;
	int cur_texture_slot;
};


//struct material {
//	
//
//	typedef std::map<std::string, gpu_program> contexts_t;
//	contexts_t contexts;
//};