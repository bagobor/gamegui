#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>

//#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

template<class T>
unsigned int get_size(const T& t);

namespace utility
{
	namespace details
	{
		template<class T, unsigned int buff_size, bool p, class KeyType, class Comparator>
		class resource_cache;

		template<class T, unsigned int buff_size, class KeyType, class Comparator>
		class resource_cache<T, buff_size, true, KeyType, Comparator>
		{
		public: 
			typedef boost::shared_ptr<T> resource_ptr;

			resource_ptr get(const KeyType& name) {return resource_ptr();}
			bool add(const KeyType& name, resource_ptr ptr) {return false;}
			void clean() {}
			int get_used_space() const {return 0;}
		};	

		// T - тип ресурса, buff_size - максимальный размер буфера
		// требует наличия метода size_t get_size(const T&)
		template<class T, unsigned int buff_size, class KeyType, class Comparator>
		class resource_cache<T, buff_size, false, KeyType, Comparator>
		{
			typedef boost::mutex::scoped_lock scoped_lock;
			boost::mutex m_mutex;
		public:	
			typedef boost::shared_ptr<T> resource_ptr;
			typedef std::map<KeyType, resource_ptr, Comparator> resource_storage;
			typedef typename resource_storage::iterator resource_storage_it;

			resource_cache() : m_used_space(0) {}	

			bool add(const KeyType& name, const resource_ptr& ptr)
			{
				if(get(name)) 
				{
					return true; // уже в кеше
				}

				scoped_lock lock(m_mutex);

				int res_size = get_size(*ptr);
				bool res = try_clean_space(res_size);

				// как правило если размер ресурса больше чем размер кеша
				// в этом случае ресурс будет удален, т.к. его не удалось 
				// сохранить
				if (!res) return false; 

				m_used_space += res_size;
				m_storage.insert(std::make_pair(name, ptr));

				return true;
			}

			resource_ptr get(const KeyType& name)
			{
				scoped_lock lock(m_mutex);

				resource_storage_it it = m_storage.find(name);

				if (it != m_storage.end())
				{
					resource_ptr res = it->second;
					m_storage.erase(it);
					m_used_space -= get_size(*res);
					return res;				
				}

				return resource_ptr();
			}

			bool empty() const 
			{
				scoped_lock lock(m_mutex);
				return m_storage.empty();
			}

			void clean()
			{
				scoped_lock lock(m_mutex);
				m_storage.clear();
				m_used_space = 0;
			}

			int get_used_space() const {return m_used_space;}

		protected:
			bool try_clean_space(size_t bytes)
			{
				int total_size = buff_size;
				if (m_used_space + bytes <= total_size) return true;
				if (bytes > total_size) return false;

				int bytes_to_free = 0;

				resource_storage_it it = m_storage.begin();
				while (it != m_storage.end())
				{
					bytes_to_free += get_size(*it->second);
					it = m_storage.erase(it);
					if (bytes_to_free >= bytes) break;
				}

				m_used_space -= bytes_to_free;

				return true;
			}

		protected:
			int m_used_space;
			resource_storage m_storage;
		};
	}

	// Перечисление действий при ненайденном ресурсе, возвращаемых callback-функцией
	enum NotFoundResponse
	{
		NFR_RETURN_NULL,
		NFR_RETURN_DEFAULT,
		NFR_RETRY
	};

	template<class T, class KeyType/* = std::wstring*/>
	class IResourceManager
	{
	public:
		typedef KeyType key_type;
		typedef T resource_type;
		typedef typename boost::shared_ptr<T> ResourcePtr;
		// Тип callback-функции для обработки ситуаций ненайденного ресурса
		typedef boost::function<NotFoundResponse (const KeyType& name)> NotFoundCallback;

		typedef boost::function<bool(T&)> predicate;
		typedef boost::function<void(T&)> func;

		virtual void for_each(func f, predicate p = predicate()) = 0;

		virtual ~IResourceManager() {}

		enum QueryType
		{
			QUERY_TYPE_NORMAL,		// ищем ресурс, если не нашли - создаём
			QUERY_TYPE_FIND_ONLY,	// ищем ресурс, если не нашли - не создаём
			QUERY_TYPE_CREATE_ONLY, // не ищем ресурс, просто создаём
		};
		virtual ResourcePtr GetResource(
			const KeyType& name, 
			bool shared = false, 
			QueryType queryType = QUERY_TYPE_NORMAL) = 0;

		virtual void		ClearSharedResources() = 0;

		virtual void Set_OnNotFound(const NotFoundCallback& callback) = 0;
		virtual void Remove_OnNotFound() = 0;
		virtual int get_used_space() const = 0; 
	};

	// если указано immediate_del то объект будет уничтожен сразу как только на него
	// не станет активных указателей. иначе он будет помещен в список на удаление, и 
	// в случае надобности может быть быстро затребован, без повторного согдания

	// так же надо сделать кеш для объектов общего назначения. которые будут храниться 
	// в менеджере, т.к. они могу и скорей всего используются на протяжении всего жизненного
	// цикла игры - это например текстуры кнопочек, анимация песонажей, и т.д.

	// менеджер гарантирует что если умный указатель пришел со своим делетером, то он будет
	// вызван при уничтожении указателя.

	// в случае immediate_del = false предполагается наличие для типа ф-ии вида get_size(const T&)
	// которая вернет размер объекта в байтах
	template<class T, 
			 bool immediate_del = true, 
			 int buff_size = 0, 
			 class KeyType = std::wstring,
			 class Comparator = std::less<KeyType>,
			 typename mutex = boost::mutex,
			 typename lock = boost::mutex::scoped_lock
			>
	class ResourceManager : public IResourceManager<T, KeyType>
	{
		typedef details::resource_cache<T, buff_size, immediate_del, KeyType, Comparator> delete_cache;
		typedef typename lock scoped_lock;
		mutex m_mutex;
	public:
		typedef typename boost::shared_ptr<T> ResourcePtr;
		typedef typename boost::weak_ptr<T>   ResourceWeakPtr;

		typedef IResourceManager<T, KeyType> Base;

		typedef std::map<KeyType, ResourceWeakPtr, Comparator>	ResourceStorage;
		typedef std::map<KeyType, ResourcePtr, Comparator>	SharedResourceStorage;

		typedef typename SharedResourceStorage::iterator ShaderResIt;
		typedef typename ResourceStorage::iterator		ResIt;

		typedef boost::function<ResourcePtr (const KeyType& name)> CreatorFunc;

		ResourceManager(const CreatorFunc& func, const KeyType& default_res_name) 
			: m_creator_func(func),
			  m_default_resource_name(default_res_name),
			  m_use_default_resource(true),
			  m_used_space(0)
		{}

		ResourceManager(const CreatorFunc& func) 
			: m_creator_func(func),
			  m_use_default_resource(false),
			  m_used_space(0)
		{}

		virtual ~ResourceManager() 
		{
			scoped_lock lock(m_mutex);

			assert(m_storage.empty());
			if (!m_storage.empty())
				throw std::exception("ResourceManager::~ResourceManager::Some resources are not deleted!");
		}


		// параметр shared упраяет приоритетом поиска:
		// true - ищется сначала в shared хранилище, false - сначала в обычном
		// так же этот параметр управляет тем, в какое хранилище будет помещен ресурс
		ResourcePtr GetResource(const KeyType& name, bool shared = false, QueryType queryType = QUERY_TYPE_NORMAL)
		{
			scoped_lock lock(m_mutex);

			if (queryType == QUERY_TYPE_NORMAL || queryType == QUERY_TYPE_FIND_ONLY)
			{
				if (shared)
				{
					SharedResourceStorage::iterator shared_it = m_shared_storage.find(name);
					if (shared_it != m_shared_storage.end())
					{
						return shared_it->second;
					}

					ResourceStorage::iterator it = m_storage.find(name);
					if (it != m_storage.end())
					{
						ResourcePtr res = it->second.lock();
						//assert(res);
						if (res) 
							return res;				
					}
				}
				else
				{
					ResourceStorage::iterator it = m_storage.find(name);
					if (it != m_storage.end())
					{
						ResourcePtr res = it->second.lock();
						//assert(res);
						if(res)
							return res;				
					}

					SharedResourceStorage::iterator shared_it = m_shared_storage.find(name);
					if (shared_it != m_shared_storage.end())
					{
						return shared_it->second;
					}			
				}
			}

			return CreateResource(name, shared, queryType);
		}

		void ClearSharedResources()
		{
			scoped_lock lock(m_mutex);
			//std::cout << "Cleaning Shared Resources..." << std::endl;
			m_shared_storage.clear();
		}

		const KeyType& get_default_resorce_name() const {return m_default_resource_name;}

		void for_each(Base::func f, Base::predicate p = Base::predicate())
		{
			scoped_lock lock(m_mutex);

			if (!immediate_del)
				m_delete_cache.clean();

			for (ResIt it = m_storage.begin(); it != m_storage.end(); ++it)
			{				
				ResourcePtr res_ptr = (it->second).lock();
				assert(res_ptr);
				T& res = *res_ptr;
				if (!p || (p && p(res)))
					f(res);
			}

			for (ShaderResIt it = m_shared_storage.begin(); it != m_shared_storage.end(); ++it)
			{
				assert(it->second);
				T& res = *(it->second);
				if (!p || (p && p(res)))
					f(res);
			}
		}

		void Set_OnNotFound(const NotFoundCallback& callback)
		{
			m_notFoundCallback = callback;
		}

		void Remove_OnNotFound()
		{
			m_notFoundCallback = NULL;
		}

		virtual int get_used_space() const 
		{
			return m_delete_cache.get_used_space() + m_used_space;
		}

	private:
		ResourcePtr get_resource(const KeyType& name, QueryType queryType = QUERY_TYPE_NORMAL)
		{
			bool do_cache = !immediate_del;
			ResourcePtr res;

			if (queryType == QUERY_TYPE_NORMAL || queryType == QUERY_TYPE_FIND_ONLY)
			{
				res = do_cache ? m_delete_cache.get(name) : ResourcePtr();
			}

			if (!res && (queryType == QUERY_TYPE_NORMAL || queryType == QUERY_TYPE_CREATE_ONLY))
			{
				res = m_creator_func(name);
			}

			return res;
		}


		ResourcePtr CreateResource(const KeyType& name, bool shared, QueryType queryType)
		{
			//////////////////////////////////////////////////////////////////////////
			//if (!m_default_resource && m_use_default_resource)
			//{
			//	m_default_resource = get_resource(m_default_resource_name);
			//	if (!m_default_resource) m_use_default_resource = false;
			//}
			//return m_default_resource;
			//////////////////////////////////////////////////////////////////////////

			ResourcePtr res;
			bool repeat;
			
			do
			{
				repeat = false;
				res = get_resource(name, queryType);

				if (!res)
				{
					NotFoundResponse response = m_use_default_resource ? NFR_RETURN_DEFAULT : NFR_RETURN_NULL;
					if (!m_notFoundCallback.empty())
						response = m_notFoundCallback(name);

					if (response == NFR_RETURN_DEFAULT)
					{
						if (!m_default_resource)
						{
							m_default_resource = get_resource(m_default_resource_name);
							if (!m_default_resource) m_use_default_resource = false;
						}

						res = m_default_resource;
					}
					else if (response == NFR_RETRY)
					{
						repeat = true;
					}
				}
			}
			while (repeat);


			if (!res) return res;

			if (shared)
			{			
				m_shared_storage[name] = res;
			}
			else
			{
				typedef std::pair<KeyType, ResourceWeakPtr> value;
				ResourceStorage::_Pairib ib = m_storage.insert(value(name, ResourceWeakPtr()));
				assert(ib.second);

				ResourcePtr packed_res(res.get(), boost::bind(&ResourceManager::on_resource_release, this, _1, ib.first, res));
				res = packed_res;

				if (res != m_default_resource)
				{
					m_used_space += get_size(*res);
				}

				m_storage[name] = res;
			}

			return res;
		}


		void on_resource_release(T* p, typename ResourceStorage::iterator it, ResourcePtr orig_ptr)
		{	
			scoped_lock lock(m_mutex);
			
			const KeyType& name = it->first;

			if (orig_ptr != m_default_resource)
			{
				int res_size = get_size(*orig_ptr);
				//assert(m_used_space >= res_size);
				m_used_space -= res_size;

				if (!immediate_del)
				{				
					m_delete_cache.add(name, orig_ptr);				
				}
			}

			m_storage.erase(it);
		}

	private:
		CreatorFunc				m_creator_func;

		SharedResourceStorage	m_shared_storage;
		ResourceStorage			m_storage;

		delete_cache			m_delete_cache;

		// Callback OnFileNotFound
		NotFoundCallback		m_notFoundCallback;

		bool					m_use_default_resource;
		KeyType					m_default_resource_name;
		ResourcePtr				m_default_resource;

		int						m_used_space;
	};
}