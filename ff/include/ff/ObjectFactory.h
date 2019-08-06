#ifndef FF_OBJECTFACTORY_H_
#define FF_OBJECTFACTORY_H_

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <bits/allocator.h>
#include <ff/Object.h>
#include <ff/Exception.h>

namespace NS_FF {

EXCEPTION_DEF(FactoryException);

enum class ObjectType {
	New, Singleton
};

// 工厂类接口
class ObjectFactory: public Object {
public:
	ObjectFactory() {
	}

	virtual ~ObjectFactory() {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		this->m_creatorMap.clear();
	}

	/*
	 * 注册构造类
	 * 注册后便可调用createObject方法构造类T的对象。
	 */
	template<typename T>
	void regiesterCreator(std::string className, ObjectType objType) {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		CreatorMap::iterator it = this->m_creatorMap.find(className);
		if (it != this->m_creatorMap.end())
			THROW_EXCEPTION(FactoryException,
					"Class \"" + className + "\" has registered", 0);

		this->m_creatorMap.insert(
				make_pair(className,
						CreatorPtr(
								(objType == ObjectType::Singleton) ?
										(_ICreator*) new _SingletonCreator<T>() :
										(_ICreator*) new _NewCreator<T>())));
	}

	/*
	 * 反注册指定类
	 */
	void unregiesterCreator(std::string className) {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		CreatorMap::iterator it = this->m_creatorMap.find(className);
		if (it != this->m_creatorMap.end()) {
			this->m_creatorMap.erase(it);
		}
	}

	//创建对象，并强转为T类指针
	template<typename T>
	std::shared_ptr<T> createObject(std::string className) {
		std::lock_guard<std::mutex> lk(this->m_mutex);

		CreatorMap::iterator it = this->m_creatorMap.find(className);
		if (it == this->m_creatorMap.end())
			THROW_EXCEPTION(FactoryException,
					"Class \"" + className + "\" not registered", 0);

		auto& creator = it->second;
		if (creator->IsSingleton())
			return std::shared_ptr<T>(static_cast<T*>(creator->createObject()),
					[](T*) {});

		return std::shared_ptr<T>(static_cast<T*>(creator->createObject()));
	}

	bool hasCreator(const std::string& className) const{
		std::lock_guard<std::mutex> lk(this->m_mutex);
		return (this->m_creatorMap.find(className) != this->m_creatorMap.end());
	}

	//查询指定类是否是单例
	bool isSingletonObject(std::string className) {
		std::lock_guard<std::mutex> lk(this->m_mutex);
		CreatorMap::iterator it = this->m_creatorMap.find(className);
		if (it != this->m_creatorMap.end()) {
			return it->second->IsSingleton();
		}

		THROW_EXCEPTION(FactoryException,
				"Class \"" + className + "\" not registered", 0);
	}

	//=================================================
	//=================================================
	class _ICreator {
	public:
		_ICreator() {
		}
		virtual ~_ICreator() {
		}
		virtual void* createObject() = 0;
		virtual bool IsSingleton() = 0;
	};

	typedef std::shared_ptr<_ICreator> CreatorPtr;

	// 构造类，T为具体类
	template<typename T>
	class _NewCreator: public _ICreator {
	public:
		virtual void* createObject() {
			return new T();
		}

		virtual bool IsSingleton() {
			return false;
		}
	};

	template<typename T>
	class _SingletonCreator: public _ICreator {
	public:
		_SingletonCreator() :
				obj(NULL) {
		}

		virtual void* createObject() {
			if (!obj) {
				std::allocator<T> allocatorT;
				obj = (T*) allocatorT.allocate(sizeof(T));
				allocatorT.construct(obj);
			}
			return obj;
		}

		virtual bool IsSingleton() {
			return true;
		}

		~_SingletonCreator() {
			if (this->obj)
				delete this->obj;
		}

	private:
		T* obj;
	};

private:
	typedef std::map<std::string, CreatorPtr> CreatorMap;
	CreatorMap m_creatorMap;
	mutable std::mutex m_mutex;
};

}
#endif /* FF_OBJECTFACTORY_H_ */

