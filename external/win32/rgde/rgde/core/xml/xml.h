///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.3
// --------------------------------------------------------
// Copyright (C) 2006-2007, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <istream>
#include <exception>

#include <cstddef>
#include <cstring>

#include "xml_options.h"
#include "xml_forward.h"

#include <rgde/core/file_system_forward.h>

/// XML Parser namespace.
namespace xml
{
	using rgde::core::vfs::istream_ptr;

	/**
	 * A light-weight wrapper for manipulating attributes in DOM tree.
	 * Note: attribute does not allocate any memory for the attribute it wraps; it only wraps a
	 * pointer to existing attribute.
	 */
	class attribute
	{
		/// \internal Safe bool type
		typedef attribute_struct* attribute::*unspecified_bool_type;
	public:
		attribute();
		
    	inline operator unspecified_bool_type() const;
    	inline bool operator!() const;
		inline bool operator==(const attribute& r) const;
		inline bool operator!=(const attribute& r) const;
		inline bool operator<(const attribute& r) const;
		inline bool operator>(const attribute& r) const;
		inline bool operator<=(const attribute& r) const;
		inline bool operator>=(const attribute& r) const;

    	attribute next_attribute() const;
    	attribute previous_attribute() const;

		int as_int() const;
		double as_double() const;
		float as_float() const;
		bool as_bool() const;

		unsigned int document_order() const;

		attribute& operator=(const char* rhs);
		attribute& operator=(int rhs);
		attribute& operator=(double rhs);
		attribute& operator=(bool rhs);

		bool set_name(const char* rhs);
		bool set_value(const char* rhs);

		inline bool empty() const;

		const char* name() const;

		const char* value() const;

	private:
		friend class attribute_iterator;
		friend class node;

		attribute_struct* _attr;
		explicit attribute(attribute_struct* attr);
	};

	/// Tree node classification.
	enum node_type
	{
		node_null,			///< Undifferentiated entity
		node_document,		///< A document tree's absolute root.
		node_element,		///< E.g. '<...>'
		node_pcdata,		///< E.g. '>...<'
		node_cdata,			///< E.g. '<![CDATA[...]]>'
		node_comment,		///< E.g. '<!--...-->'
		node_pi				///< E.g. '<?...?>'
	};

	/**
	 * A light-weight wrapper for manipulating nodes in DOM tree.
	 * Note: node does not allocate any memory for the node it wraps; it only wraps a pointer to
	 * existing node.
	 */
	class node
	{
		friend class node_iterator;
		/// \internal Safe bool type
		typedef node_struct* node::*unspecified_bool_type;

	public:
		node();

		inline operator unspecified_bool_type() const;
		
		inline bool operator!() const;	
		bool operator==(const node& r) const;
		bool operator!=(const node& r) const;
		bool operator<(const node& r) const;
		bool operator>(const node& r) const;
		bool operator<=(const node& r) const;
		bool operator>=(const node& r) const;

		/**
		 * Node iterator type (for child nodes).
		 * \see node_iterator
		 */
		typedef node_iterator iterator;

		/**
		 * Node iterator type (for child nodes).
		 * \see attribute_iterator
		 */
		typedef attribute_iterator attribute_iterator;

		iterator begin() const;
		iterator end() const;

		attribute_iterator attributes_begin() const;
		attribute_iterator attributes_end() const;

		inline bool empty() const;

		node_type type() const;

		const char* name() const;

		const char* value() const;

		node child(const char* name) const;
		node child_w(const char* name) const;

		node operator()(const std::string& name) const
		{
			return child(name.c_str());
		}

		node operator()(const char* name) const 
		{
			return child(name);
		}

		attribute operator[](const char* name) const 
		{
			return find_attribute(name);
		}

		attribute attr(const char* name) const
		{
			return find_attribute(name);
		}

		attribute find_attribute(const char* name) const;
		attribute find_attribute_w(const char* name) const;

		node next_sibling(const char* name) const;
		node next_sibling_w(const char* name) const;
		node next_sibling() const;

		node previous_sibling(const char* name) const;
		node previous_sibling_w(const char* name) const;
		node previous_sibling() const;

		node parent() const;

		node root() const;

		const char* child_value() const;
		const char* child_value(const char* name) const;
		const char* child_value_w(const char* name) const;

		bool set_name(const char* rhs);		

		bool set_value(const char* rhs);

		attribute append_attribute(const char* name);

		attribute insert_attribute_after(const char* name, const attribute& attr);
		attribute insert_attribute_before(const char* name, const attribute& attr);

		node append_child(node_type t = node_element);
		node append_child(const char* name, node_type t = node_element);

		node insert_child_after(node_type t, const node& node);
		node insert_child_before(node_type t, const node& node);

		void remove_attribute(const attribute& a);
		void remove_attribute(const char* name);

		void remove_child(const node& n);
		void remove_child(const char* name);

		attribute first_attribute() const;
        attribute last_attribute() const;

		template <typename OutputIterator> 
		void all_elements_by_name(const char* name, OutputIterator it) const;

		template <typename OutputIterator> 
		void all_elements_by_name_w(const char* name, OutputIterator it) const;

		node first_child() const;
        node last_child() const;

		template <typename Predicate> 
		attribute find_attribute(Predicate pred) const;

		template <typename Predicate> 
		node find_child(Predicate pred) const;

		template <typename Predicate> 
		node find_node(Predicate pred) const;

		std::string path(char delimiter = '/') const;

		node first_element_by_path(const char* path, char delimiter = '/') const;

		bool traverse(tree_walker& walker);
				
		unsigned int document_order() const;

		void print(std::ostream& os, const char* indent = "\t", unsigned int flags = format_default, unsigned int depth = 0);
	protected:
		node_struct* _root;
		
		explicit node(node_struct* p);
		
		void precompute_document_order_impl();
		
		allocator& get_allocator() const;
	};

	/**
	 * Child node iterator.
	 * It's a bidirectional iterator with value type 'node'.
	 */
	class node_iterator
		: public std::iterator<std::bidirectional_iterator_tag, node>
	{
		friend class node;

	public:
		node_iterator();
		node_iterator(const node& node);
		node_iterator(node_struct* ref, node_struct* prev);

		inline bool operator==(const node_iterator& rhs) const;
		inline bool operator!=(const node_iterator& rhs) const;
		inline node& operator*();
		inline node* operator->();
		inline const node_iterator& operator++();
		inline node_iterator operator++(int);
		inline const node_iterator& operator--();	
		inline node_iterator operator--(int);

	private:
		node _prev;
		node _wrap;
		
		explicit node_iterator(node_struct* ref);
	};

	/**
	 * Attribute iterator.
	 * It's a bidirectional iterator with value type 'attribute'.
	 */
	class attribute_iterator
	 : public std::iterator<std::bidirectional_iterator_tag, attribute>
	{
		friend class node;

	public:
		/**
		 * Default ctor
		 */
		attribute_iterator();

		/**
		 * Initializing ctor
		 *
		 * \param node - node that iterator will point at
		 */
		attribute_iterator(const attribute& node);

		/**
		 * Initializing ctor (for past-the-end)
		 *
		 * \param ref - should be 0
		 * \param prev - previous node
		 */
		attribute_iterator(attribute_struct* ref, attribute_struct* prev);

		/**
		 * Check if this iterator is equal to \a rhs
		 *
		 * \param rhs - other iterator
		 * \return comparison result
		 */
		inline bool operator==(const attribute_iterator& rhs) const;
		
		/**
		 * Check if this iterator is not equal to \a rhs
		 *
		 * \param rhs - other iterator
		 * \return comparison result
		 */
		inline bool operator!=(const attribute_iterator& rhs) const;

		/**
		 * Dereferencing operator
		 *
		 * \return reference to the node iterator points at
		 */
		inline attribute& operator*();

		/**
		 * Member access operator
		 *
		 * \return poitner to the node iterator points at
		 */
		inline attribute* operator->();

		/**
		 * Pre-increment operator
		 *
		 * \return self
		 */
		inline const attribute_iterator& operator++();

		/**
		 * Post-increment operator
		 *
		 * \return old value
		 */
		inline attribute_iterator operator++(int);
		
		/**
		 * Pre-decrement operator
		 *
		 * \return self
		 */
		inline const attribute_iterator& operator--();
		
		/**
		 * Post-decrement operator
		 *
		 * \return old value
		 */
		inline attribute_iterator operator--(int);

	private:
		attribute _prev;
		attribute _wrap;

		/// \internal Initializing ctor
		explicit attribute_iterator(attribute_struct* ref);
	};

	/**
	 * Abstract tree walker class
	 * \see node::traverse
	 */
	class tree_walker
	{
		friend class node;
	
	public:
		/**
		 * Default ctor
		 */
		tree_walker();

		/**
		 * Virtual dtor
		 */
		virtual ~tree_walker();

		/**
		 * Callback that is called when traversal of node begins.
		 *
		 * \return returning false will abort the traversal
		 */
		virtual bool begin(node&);

		/**
		 * Callback that is called for each node traversed
		 *
		 * \return returning false will abort the traversal
		 */
		virtual bool for_each(node&) = 0;

		/**
		 * Callback that is called when traversal of node ends.
		 *
		 * \return returning false will abort the traversal
		 */
		virtual bool end(node&);

	protected:
		/**
		* Get node depth
		* 
		* \return node depth
		*/
		int depth() const;

	private:
		int _depth;
	};


	/**
	 * Struct used to distinguish parsing with ownership transfer from parsing without it.
	 * \see document::parse
	 */
	struct transfer_ownership_tag {};

	/**
	 * Document class (DOM tree root).
	 * This class has noncopyable semantics (private copy ctor/assignment operator).
	 */
	class document: public node
	{
	public:
		/// \internal Memory block
		struct memory_block
		{
			memory_block();

			memory_block* next;
			size_t size;

			char data[memory_block_size];
		};

		document();
		~document();

		bool load(const char* contents, unsigned int options = parse_default);

		bool load_file(const char* name, unsigned int options = parse_default);
		bool load_file(const wchar_t* name, unsigned int options = parse_default);

		bool parse(char* xmlstr, unsigned int options = parse_default);
		bool parse(const transfer_ownership_tag&, char* xmlstr, unsigned int options = parse_default);
		
		bool load(istream_ptr stream, unsigned int options = parse_default);
		
		bool load(std::istream& stream, unsigned int options = parse_default);
		bool save_file(const char* name, const char* indent = "\t", unsigned int flags = format_default);

		void precompute_document_order();

	private:
		document(const document&);
		const document& operator=(const document&);

		void create();
		void free();

		char*				_buffer;
		memory_block	_memory;
	};

	std::string as_utf8(const wchar_t* str);
	std::wstring as_utf16(const char* str);
}

// Inline implementation

namespace xml
{
	namespace impl
	{
		int strcmpwild(const char*, const char*);
	}
}

#include "xml.inl"