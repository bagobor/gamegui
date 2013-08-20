///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.3
// --------------------------------------------------------
// Copyright (C) 2007, by Denis Ovod (denis.ovod@gmail.com)
// This work is based on the pugixml parser[Pug Improved XML Parser - Version 0.3]
// , which is:
// Copyright (C) 2006-2007, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

/// XML Parser namespace.
namespace xml
{
	/**
	* Safe bool conversion.
	* Allows attribute to be used in a context where boolean variable is expected, such as 'if (node)'.
	*/
	attribute::operator attribute::unspecified_bool_type() const
	{
		return empty() ? 0 : &attribute::_attr;
	}

	bool attribute::operator!() const
	{
		return empty();
	}

	/**
	* Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
	*
	* \param r - value to compare to
	* \return comparison result
	*/
	bool attribute::operator==(const attribute& r) const
	{
		return (_attr == r._attr);
	}

	/**
	* Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
	*
	* \param r - value to compare to
	* \return comparison result
	*/
	bool attribute::operator!=(const attribute& r) const
	{
		return (_attr != r._attr);
	}

	/**
	* Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
	*
	* \param r - value to compare to
	* \return comparison result
	*/
	bool attribute::operator<(const attribute& r) const
	{
		return (_attr < r._attr);
	}

	/**
	* Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
	*
	* \param r - value to compare to
	* \return comparison result
	*/
	bool attribute::operator>(const attribute& r) const
	{
		return (_attr > r._attr);
	}

	/**
	* Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
	*
	* \param r - value to compare to
	* \return comparison result
	*/
	bool attribute::operator<=(const attribute& r) const
	{
		return (_attr <= r._attr);
	}

	/**
	* Compare wrapped pointer to the attribute to the pointer that is wrapped by \a r.
	*
	* \param r - value to compare to
	* \return comparison result
	*/
	bool attribute::operator>=(const attribute& r) const
	{
		return (_attr >= r._attr);
	}



	/**
	* Check if attribute is empty.
	*
	* \return true if attribute is empty, false otherwise
	*/
	bool attribute::empty() const
	{
		return (_attr == 0);
	}

	/**
	* Safe bool conversion.
	* Allows node to be used in a context where boolean variable is expected, such as 'if (node)'.
	*/
	node::operator node::unspecified_bool_type() const
	{
		return empty() ? 0 : &node::_root;
	}

	/// Borland C++ workaround
	bool node::operator!() const
	{
		return empty();
	}



	/**
	* Check if node is empty.
	*
	* \return true if node is empty, false otherwise
	*/
	bool node::empty() const
	{
		return (_root == 0);
	}

	/**
	* Check if this iterator is equal to \a rhs
	*
	* \param rhs - other iterator
	* \return comparison result
	*/
	bool node_iterator::operator==(const node_iterator& rhs) const
	{
		return (_wrap == rhs._wrap);
	}

	/**
	* Check if this iterator is not equal to \a rhs
	*
	* \param rhs - other iterator
	* \return comparison result
	*/
	bool node_iterator::operator!=(const node_iterator& rhs) const
	{
		return (_wrap != rhs._wrap);
	}

	/**
	* Dereferencing operator
	*
	* \return reference to the node iterator points at
	*/
	node& node_iterator::operator*()
	{
		return _wrap;
	}

	/**
	* Member access operator
	*
	* \return poitner to the node iterator points at
	*/
	node* node_iterator::operator->()
	{
		return &_wrap;
	}

	/**
	* Post-increment operator
	*
	* \return old value
	*/
	node_iterator node_iterator::operator++(int)
	{
		node_iterator temp = *this;
		++*this;
		return temp;
	}


	/**
	* Post-decrement operator
	*
	* \return old value
	*/
	node_iterator node_iterator::operator--(int)
	{
		node_iterator temp = *this;
		--*this;
		return temp;
	}


	bool attribute_iterator::operator==(const attribute_iterator& rhs) const
	{
		return (_wrap == rhs._wrap);
	}

	bool attribute_iterator::operator!=(const attribute_iterator& rhs) const
	{
		return (_wrap != rhs._wrap);
	}

	attribute& attribute_iterator::operator*()
	{
		return _wrap;
	}

	attribute* attribute_iterator::operator->()
	{
		return &_wrap;
	}

	attribute_iterator attribute_iterator::operator++(int)
	{
		attribute_iterator temp = *this;
		++*this;
		return temp;
	}

	attribute_iterator attribute_iterator::operator--(int)
	{
		attribute_iterator temp = *this;
		--*this;
		return temp;
	}

	/**
	* Get all elements from subtree with given name
	*
	* \param name - node name
	* \param it - output iterator (for example, std::back_insert_iterator (result of std::back_inserter))
	*/
	template <typename OutputIterator> 
	void node::all_elements_by_name(const char* name, OutputIterator it) const
	{
		if (empty()) return;

		for (node node = first_child(); node; node = node.next_sibling())
		{
			if (node.type() == node_element)
			{
				if (!strcmp(name, node.name()))
				{
					*it = node;
					++it;
				}

				if (node.first_child()) node.all_elements_by_name(name, it);
			}
		}
	}

	/**
	* Get all elements from subtree with name that matches given pattern
	*
	* \param name - node name pattern
	* \param it - output iterator (for example, std::back_insert_iterator (result of std::back_inserter))
	*/
	template <typename OutputIterator> 
	void node::all_elements_by_name_w(const char* name, OutputIterator it) const
	{
		if (empty()) return;

		for (node node = first_child(); node; node = node.next_sibling())
		{
			if (node.type() == node_element)
			{
				if (!impl::strcmpwild(name, node.name()))
				{
					*it = node;
					++it;
				}

				if (node.first_child()) node.all_elements_by_name_w(name, it);
			}
		}
	}

	/**
	* Find attribute using predicate
	*
	* \param pred - predicate, that takes attribute and returns bool
	* \return first attribute for which predicate returned true, or empty attribute
	*/
	template <typename Predicate> 
	inline attribute node::find_attribute(Predicate pred) const
	{
		if (!empty())
			for (attribute attrib = first_attribute(); attrib; attrib = attrib.next_attribute())
				if (pred(attrib))
					return attrib;

		return attribute();
	}

	/**
	* Find child node using predicate
	*
	* \param pred - predicate, that takes node and returns bool
	* \return first child node for which predicate returned true, or empty node
	*/
	template <typename Predicate> 
	inline node node::find_child(Predicate pred) const
	{
		if (!empty())
			for (node node = first_child(); node; node = node.next_sibling())
				if (pred(node))
					return node;

		return node();
	}

	/**
	* Find node from subtree using predicate
	*
	* \param pred - predicate, that takes node and returns bool
	* \return first node from subtree for which predicate returned true, or empty node
	*/
	template <typename Predicate> 
	inline node node::find_node(Predicate pred) const
	{
		if (!empty())
			for (node n = first_child(); n; n = n.next_sibling())
			{
				if (pred(n))
					return n;

				if (n.first_child())
				{
					node found = n.find_node(pred);
					if (found) return found;
				}
			}

			return node();
	}
}