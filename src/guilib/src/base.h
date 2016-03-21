#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <deque>

template<typename T, typename U> 
T lexical_cast(U in)
{
	std::stringstream ss;
	ss << in;
	T out;
	ss >> out;
	return out;
}

template<class Ch>
std::vector<std::basic_string<Ch> > tokenize(const std::basic_string<Ch>& delim, const std::basic_string<Ch> &src)
{
	std::vector<std::basic_string<Ch> > out_tokens;

	for (size_t end	= 0; end != std::basic_string<Ch>::npos;)
	{
		size_t start = src.find_first_not_of(delim, end);
		end = src.find_first_of(delim, start);

		if (start == std::string::npos) continue;

		size_t end_pos = end == std::string::npos ? end : end - start;
		out_tokens.push_back(src.substr(start, end_pos));
	}
	return out_tokens;
}

template <class T>
class TreeNode : public std::enable_shared_from_this<TreeNode<T>>
{
public:
	typedef T Node;
	typedef std::shared_ptr<Node> node_ptr;
	typedef std::deque<node_ptr> children_t;
	typedef typename children_t::iterator child_iter;
	typedef typename children_t::reverse_iterator child_riter;
	typedef typename children_t::const_iterator child_citer;

	TreeNode() : m_parent(NULL) {}
	explicit TreeNode(const std::string& name) : m_parent(NULL), m_name(name) {}
	virtual ~TreeNode() {
		for (children_t::iterator it = m_children.begin(); it != m_children.end(); ++it)
			(*it)->setParent(NULL);
	}

	virtual bool canHaveChildren() const { return true; }

	void add(node_ptr node)
	{
		add(node, m_children.end());
	}

	void add(node_ptr node, child_iter iwhere)
	{
		if (!canHaveChildren()) return;

		if (node && node.get() != this && node->getParent() != this)
		{
			node->resetParent();

			m_children.insert(iwhere, node);
			onChildAdd(node);
			node->setParent((T*)this);
		}
	}

	void remove(node_ptr node)
	{
		if(!canHaveChildren()) return;

		if (node && node.get() != this && node->getParent() == this)
		{
			m_children.erase(std::remove(m_children.begin(), m_children.end(), node), m_children.end());
			onChildRemove(node);
			node->setParent(0);
		}
	}

	child_iter getOrder(node_ptr node)
	{
		child_iter f = std::find(m_children.begin(), m_children.end(), node);
		if (f == m_children.begin())
			++f;
		else
			--f;
		return f;
	}

	const T* getParent() const  {return m_parent;}
	T*		 getParent()		{return m_parent;}
	T*		 getParentConst() const		{return m_parent;}

	children_t& getChildren() {return m_children;}
	const children_t& getChildren() const {return m_children;}

	/// @node_path is something like 'name.name1.name2'
	node_ptr find(const std::string& node_path)
	{
		std::vector<std::string> names = tokenize<char>(".", node_path);
		return find(names);
	}

	//TODO: optimize it.
	node_ptr find(const std::vector<std::string>& nodes_names)
	{
		node_ptr node = child(*nodes_names.begin());

		if (!node) return node;
		
		if (nodes_names.size() == 1)
			return node;

		std::vector<std::string> name;
		name.resize(nodes_names.size()-1);
		std::copy((++nodes_names.begin()), nodes_names.end(), name.begin());
		return node->find(name);
	}

	node_ptr child(const std::string& name)
	{
		if (m_children.empty()) return node_ptr();

		for (size_t i = 0, size = m_children.size(); i < size; ++i)
		{
			node_ptr &c = m_children[i];
			if (c->getName() == name) return c;
		}

		return node_ptr();
	}

	node_ptr nextSibling()
	{
		if (!m_parent) return node_ptr();

		children_t& children = m_parent->getChildren();

		child_iter it = std::find(children.begin(), children.end(), ptr());
		if (it == children.end())
			return node_ptr();

		return (++it != children.end()) ? (*it) : node_ptr();
	}

	node_ptr prevSibling()
	{
		if (!m_parent) return node_ptr();

		children_t& list = m_parent->getChildren();

		child_iter it = std::find(list.begin(), list.end(), ptr());
		if (it == list.end())
			return node_ptr();

		if (it == list.begin())
			return node_ptr();
		--it;
		return *it;
	}

	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }

	node_ptr ptr() { 
		return std::static_pointer_cast<T>(shared_from_this());
	}

protected:
	void setParent(T* node)
	{
		T* old_parent = m_parent;
		m_parent = node; 
		onParentChange(old_parent);
	}

	void resetParent()
	{
		if (m_parent) {
			m_parent->remove(ptr());
		}
	}

	virtual void onParentChange(T* old_parent)	{old_parent;}
	virtual void onChildAdd(node_ptr& node)	{node;}
	virtual void onChildRemove(node_ptr& node) {node;}

protected:
	std::string m_name;
	children_t m_children;
	T* m_parent;
};
