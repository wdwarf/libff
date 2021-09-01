#ifndef FF_OBJECTFACTORY_H_
#define FF_OBJECTFACTORY_H_

#include <ff/Exception.h>
#include <ff/Object.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

NS_FF_BEG

EXCEPTION_DEF(FactoryException);

enum class ObjectType { New, Singleton };

// 工厂类接口
class LIBFF_API ObjectFactory : public Object {
 public:
  ObjectFactory() {}

  virtual ~ObjectFactory() {
    std::lock_guard<std::mutex> lk(this->m_mutex);
    this->m_creatorMap.clear();
  }

  /*
   * 注册构造类
   * 注册后便可调用createObject方法构造类T的对象。
   */
  template <typename T, typename... Args>
  void regiesterCreator(std::string className, ObjectType objType, Args... args)
      _throws(FactoryException) {
    std::lock_guard<std::mutex> lk(this->m_mutex);
    CreatorMap::iterator it = this->m_creatorMap.find(className);
    if (it != this->m_creatorMap.end())
      THROW_EXCEPTION(FactoryException,
                      "Class \"" + className + "\" has registered", 0);

    this->m_creatorMap.insert(make_pair(
        className,
        CreatorPtr(new Creator<T>(objType, std::forward<Args>(args)...))));
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
  template <typename T, typename... Args>
  std::shared_ptr<T> createObject(std::string className, Args... args)
      _throws(FactoryException) {
    std::lock_guard<std::mutex> lk(this->m_mutex);

    CreatorMap::iterator it = this->m_creatorMap.find(className);
    if (it == this->m_creatorMap.end())
      THROW_EXCEPTION(FactoryException,
                      "Class \"" + className + "\" not registered", 0);

    Creator<T>* creator = static_cast<Creator<T>*>(it->second.get());
    if (creator->IsSingleton())
      return std::shared_ptr<T>(
          static_cast<T*>(creator->createObject(std::forward<Args>(args)...)),
          [](T*) {});

    return std::shared_ptr<T>(
        static_cast<T*>(creator->createObject(std::forward<Args>(args)...)));
  }

  bool hasCreator(const std::string& className) const {
    std::lock_guard<std::mutex> lk(this->m_mutex);
    return (this->m_creatorMap.find(className) != this->m_creatorMap.end());
  }

  //查询指定类是否是单例
  bool isSingletonObject(std::string className) _throws(FactoryException) {
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
    _ICreator(ObjectType type) { this->m_tpe = type; }
    bool IsSingleton() { return (ObjectType::Singleton == this->m_tpe); }

   protected:
    ObjectType m_tpe;
  };

  typedef std::shared_ptr<_ICreator> CreatorPtr;

  template <class T, typename... Args>
  static void* CreatorFunc(Args... args) {
    return new T(std::forward<Args...>(args)...);
  }

  template <typename T>
  class Creator : public _ICreator {
   public:
    template <typename... Args>
    Creator(ObjectType type, Args... args) : _ICreator(type), obj(nullptr) {
      this->m_creatorFunc =
          std::bind(CreatorFunc<T, Args...>, std::forward<Args>(args)...);
    }

    void* createObject() {
      if (ObjectType::Singleton == this->m_tpe) {
        if (nullptr == obj) obj = (T*)this->m_creatorFunc();
        return obj;
      }
      return this->m_creatorFunc();
    }

    template <typename... Args>
    void* createObject(Args... args) {
      if (ObjectType::Singleton == this->m_tpe) {
        return this->createObject();
      }
      return new T(std::forward<Args>(args)...);
    }

   private:
    T* obj;
    std::function<void*()> m_creatorFunc;
  };

 private:
  typedef std::map<std::string, CreatorPtr> CreatorMap;
  CreatorMap m_creatorMap;
  mutable std::mutex m_mutex;
};

NS_FF_END

#endif /* FF_OBJECTFACTORY_H_ */
