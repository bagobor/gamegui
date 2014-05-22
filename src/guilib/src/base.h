#pragma once

#include <string>
#include <iostream>
#include <sstream>


template<typename T, typename U> T lexical_cast(U in)
{
	std::stringstream ss;
	ss << in;
	T out;
	ss >> out;
	return out;
}
//template <typename T>
//T lexical_cast(const std::string& str)
//{
//	T var;
//	std::istringstream iss;
//	iss.str(str);
//	iss >> var;
//	// deal with any error bits that may have been set on the stream
//	return var;
//}

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
//class  RefCounted 
//{
//public:
//	RefCounted():m_refCount(0){}
//	virtual ~RefCounted(){}
//private: 
//	int	m_refCount;
//	friend void intrusive_ptr_add_ref(RefCounted*);
//	friend void intrusive_ptr_release(RefCounted*);
//};
//
//inline void intrusive_ptr_add_ref(RefCounted* p)
//{
//	++(p->m_refCount);
//}
//
//inline void intrusive_ptr_release(RefCounted* p)
//{
//	--(p->m_refCount);
//
//	if (p->m_refCount == 0)
//		delete p;
//}
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

struct prop_holder {
	enum type_t {
		range_t,		// range from min to max
		sigle_value_t,	// single value
		validation_t,	// has validator
		list_t,			// list of possible values
		//array_t		// with rand
	} peroperty_type;

	enum value_type_t {
		int_t,
		uint_t,
		float_t,
		double_t,
		string_t,
		object_t		// user type (class,struct) 
	} value_type;;

	prop_holder() : read_only(false) {}
	virtual ~prop_holder() {}
	virtual void from_string(void* owner, const std::string&) const = 0;
	virtual std::string to_string(const void* owner) const = 0;
	
	bool read_only;
	std::string meta_info; // annotations, etc	
	// possible values, min/max value, validator functor, etc
};

template<class OwnerType, typename ValueType>
struct prop_holder_t : prop_holder{
	virtual ValueType get(const OwnerType*) const = 0;
	virtual void set(OwnerType*, const ValueType&) const = 0;
};

template<class T, typename V>
struct prop_holder_m_const : prop_holder_t<T, V>{
	typedef typename V const T::* const mptr_t;
	mptr_t ptr;

	prop_holder_m_const(mptr_t p) : ptr(p) { read_only = true; }

	virtual void set(T* po, const V& v) const { return; }
	virtual void from_string(void* owner, const std::string& str) const { return; }

	virtual V get(const T* po) const { return po ? po->*(ptr) : V(0); }
	virtual std::string to_string(const void* owner) const {
		T* ot = (T*)owner;
		return lexical_cast<std::string>((*ot).*ptr);
	}
};

template<class T, typename V>
struct prop_holder_m : prop_holder_t<T, V>{
	typedef typename V T::*mptr_t;
	mptr_t ptr;

	prop_holder_m(mptr_t p) : ptr(p) { read_only = false; }

	virtual V get(const T* po) const { return po ? po->*(ptr) : V(0); }

	virtual void set(T* po, const V& v) const {
		if (read_only || !po) return;
		po->*(ptr) = v;
	}

	virtual void from_string(void* owner, const std::string& str) const {
		T* ot = (T*)owner;
		if (read_only || !owner) return;
		(*ot).*ptr = lexical_cast<V>(str);
	}

	virtual std::string to_string(const void* owner) const {
		T* ot = (T*)owner;
		return lexical_cast<std::string>((*ot).*ptr);
	}
};

template<class OwnerType, typename ValueType>
struct prop_holder_f : prop_holder_t<OwnerType, ValueType>{
	//typedef typename ValueType OwnerType::*mptr_t;
	typedef void (OwnerType::* setter_t) (ValueType);
	typedef ValueType(OwnerType::* getter_t) () const;

	getter_t getter;
	setter_t setter;

	prop_holder_f(getter_t g, setter_t s) : getter(g), setter(s)
	{
		read_only = s ? false : true;
	}

	virtual ValueType get(const OwnerType* po) const {
		return po ? (po->*getter)() : ValueType(0);
	}

	virtual void set(OwnerType* po, const ValueType& v) const {
		if (read_only || !po) return;
		(po->*setter)(v);
	}

	virtual void from_string(void* owner, const std::string& str) const {
		if (read_only || !owner) return;
		OwnerType* ot = (OwnerType*)owner;
		ValueType v = lexical_cast<ValueType>(str);
		(ot->*setter)(v);
	}

	virtual std::string to_string(const void* owner) const {
		if (!owner) return std::string();
		OwnerType* ot = (OwnerType*)owner;
		ValueType v = (ot->*getter)();
		return lexical_cast<std::string>(v);
	}
};


class property {

public:
	property() : value_type(typeid(void)), owner_type(typeid(void)), holder(0){}
	property(const std::type_info& owner_type, const std::type_info& value_type, prop_holder* holder)
		: value_type(value_type), owner_type(owner_type), holder(holder)
	{}

	~property() {
		delete holder; holder = 0;
	}

	//data member property
	template<class T, typename V>
	static property* create(V T::* p, bool read_only) {
		const std::type_info& ot = typeid(T);
		const std::type_info& vt = typeid(V);
		prop_holder* holder = 0;
		if (read_only)
			holder = new prop_holder_m_const <T, V>(p);
		else
			holder = new prop_holder_m <T, V>(p);

		property* out = new property(ot, vt, holder);
		return out;
	}

	//handle case of const data member
	template<class T, typename V>
	static property* create(V const T::* const p) {
		const std::type_info& ot = typeid(T);
		const std::type_info& vt = typeid(V);
		prop_holder* holder = new prop_holder_m_const <T, V>(p);
		property* out = new property(ot, vt, holder);
		return out;
	}

	template<class T, typename V>
	static property* create(V(T::* getter) () const, void (T::* setter) (V)) {
		const std::type_info& ot = typeid(T);
		const std::type_info& vt = typeid(V);
		prop_holder* holder = new prop_holder_f<T, V>(getter, setter);
		property* out = new property(ot, vt, holder);
		return out;
		//return 0;
	}

	template<class T>
	void set(T* owner, const char* s) const{
		if (typeid(const char*) != value_type)
			set(owner, std::string(s));
		else
			set<T, const char*>(owner, s);
	}

	template<class T, typename V>
	void set(T* owner, V v) const{
		if (typeid(T) != owner_type) return;
		if (typeid(V) == value_type){
			prop_holder_t<T, V>* h = static_cast<prop_holder_t<T, V>*>(holder);
			h->set(owner, v);
		}
		else{
			holder->from_string(owner, boost::lexical_cast<std::string>(v));
		}
	}

	template<class T, typename V>
	V get(const T* owner) const{
		if (typeid(T) != owner_type) return V(0);
		if (typeid(V) != value_type)
			return boost::lexical_cast<V>(holder->to_string(owner));

		prop_holder_t<T, V>* h = (prop_holder_t<T, V>*)holder;
		return h->get(owner);
	}

	const std::type_info& owner_type;
	const std::type_info& value_type;

	operator bool() const { return owner_type != typeid(void); }

private:
	prop_holder* holder;
};

struct registry{
	typedef std::map<std::string, property*> properties_t;
	typedef std::map<std::string, properties_t> types_t;
	typedef std::map<std::string, std::string> type_aliases_t;

	template<class T, typename V>
	static const property& reg(const char* name, V T::*ptr, bool read_only = false){
		const type_info& ti = typeid(T);
		property* p = property::create<T, V>(ptr, read_only);
		types[ti.name()][name] = p;
		return *p;
	}

	template<class T, typename V>
	static const property& reg(const char* name, V const T::* const ptr){
		const type_info& ti = typeid(T);
		property* p = property::create<T, V>(ptr);
		types[ti.name()][name] = p;
		return *p;
	}

	template<class T, typename V>
	static const property& reg(const char* name, V(T::* getter) () const, void (T::* setter) (V) = 0){
		const type_info& ti = typeid(T);
		property* p = property::create<T, V>(getter, setter);
		types[ti.name()][name] = p;
		return *p;
	}

	template<class T>
	static const property& get(const char* name){
		const type_info& ti = typeid(T);
		return *(types[ti.name()][name]);
	}
private:
	static types_t types;
	static type_aliases_t aliases;
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
		if(!isCanHaveChildren()) return;

		if (node && node.get() != this && node->getParent() == this)
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
		if (m_parent) {
			node_ptr n(static_cast<T*>(this));
			m_parent->remove(n);
		}
	}

	virtual void onParentChange(T* old_parent)	{old_parent;}
	virtual void onChildAdd(node_ptr& node)	{node;}
	virtual void onChildRemove(node_ptr& node) {node;}

protected:
	children_list m_children;
	T* m_parent;
};
