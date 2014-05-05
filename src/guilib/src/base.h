#pragma once

//-----------------------------------------------------------------
template<class Ch>
std::list<std::basic_string<Ch> > tokenize(const std::basic_string<Ch>& delim, const std::basic_string<Ch> &src)
{
	std::list<std::basic_string<Ch> > out_tokens;

	for (size_t end	= 0; end != std::basic_string<Ch>::npos;)
	{
		size_t start = src.find_first_not_of(delim, end);
		end = src.find_first_of(delim, start);

		if (start != std::string::npos)
		{
			size_t end_pos = end == std::string::npos ? end : end - start;
			out_tokens.push_back(src.substr(start, end_pos));
		}
	}
	return out_tokens;
}
//-----------------------------------------------------------------
class  RefCounted 
{
public:
	RefCounted():m_refCount(0){}
	virtual ~RefCounted(){}
private: 
	int	m_refCount;
	friend void intrusive_ptr_add_ref(RefCounted*);
	friend void intrusive_ptr_release(RefCounted*);
};

inline void intrusive_ptr_add_ref(RefCounted* p)
{
	++(p->m_refCount);
}

inline void intrusive_ptr_release(RefCounted* p)
{
	--(p->m_refCount);

	if (p->m_refCount == 0)
		delete p;
}
//-----------------------------------------------------------------
class named_object
{
public:
	explicit named_object(const std::string& name);
	virtual ~named_object(){}

	const std::string& getName() const;
	void setName(const std::string& name);

private:
	std::string m_strName;
};
//-----------------------------------------------------------------
template <class T>
class TreeNode : public std::enable_shared_from_this<TreeNode<T>>
{
public:
	typedef T Node;
	typedef std::shared_ptr<Node> node_ptr;
	typedef std::list<node_ptr> children_list;
	typedef typename children_list::iterator child_iter;
	typedef typename children_list::reverse_iterator child_riter;
	typedef typename children_list::const_iterator child_citer;

	virtual bool isCanHaveChildren() const = 0;

	void add(node_ptr node)
	{
		add(node, m_children.end());
	}

	void add(node_ptr node, child_iter iwhere)
	{
		if(!isCanHaveChildren()) return;

		if (node && node != this && node->getParent() != this)
		{
			node->resetParent();

			m_children.insert(iwhere, node);
			onChildAdd(node);
			node->setParent((T*)this);
		}
	}

	void remove(node_ptr node)
	{
		if(!isCanHaveChildren()) return;

		if (node && node != this && node->getParent() == this)
		{
			m_children.remove(node);
			onChildRemove(node);
			node->setParent(0);
		}
	}

	child_iter getOrder(node_ptr node) 
	{
		child_iter f = std::find(m_children.begin(), m_children.end(), node);
		if(f == m_children.begin())
			++f;
		else
			--f;
		return f;
	}

	const T* getParent() const  {return m_parent;}
	T*		 getParent()		{return m_parent;}
	T*		 getParentConst() const		{return m_parent;}

	children_list& getChildren() {return m_children;}
	const children_list& getChildren() const {return m_children;}

	TreeNode() : m_parent(0){}
	virtual ~TreeNode()
	{
		for (children_list::iterator it = m_children.begin(); it != m_children.end(); ++it) 
		{
			(*it)->setParent(0);
		}
	}

	struct _searcher{
		const std::string& m_strName;
		_searcher& operator=(const _searcher&) {}
		_searcher(const std::string& name) : m_strName(name){}
		bool operator()(node_ptr obj) 
		{
			return obj ? (obj->getName() == m_strName) : false;
		}
	};

	node_ptr find(const std::string& node_name)
	{
		return find(tokenize<char>(".", node_name));
	}

	node_ptr find(const std::list<std::string>& nodes_names)
	{
		node_ptr node = child(*nodes_names.begin());

		if (!node) return node; // returns node_ptr();
		
		if (nodes_names.size() == 1)
			return node;

		std::list<std::string> name;
		name.resize(nodes_names.size()-1);
		std::copy((++nodes_names.begin()), nodes_names.end(), name.begin());
		return node->find(name);
	}

	node_ptr child(const std::string& name)
	{
		children_list::iterator it = std::find_if(
			m_children.begin(), 
			m_children.end(), 
			_searcher(name)
			);

		return it == m_children.end() ? node_ptr() : *it;
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
		if(m_parent)
			m_parent->remove(static_cast<T*>(this));
	}

	virtual void onParentChange(T* old_parent)	{old_parent;}
	virtual void onChildAdd(node_ptr& node)	{node;}
	virtual void onChildRemove(node_ptr& node) {node;}

protected:
	children_list m_children;
	T* m_parent;
};
