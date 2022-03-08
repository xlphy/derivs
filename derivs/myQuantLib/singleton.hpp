//
//  singleton.hpp
//  derivs
//
//  Created by Xin Li on 3/8/22.
//

#ifndef singleton_hpp
#define singleton_hpp

// compilation flags

#ifdef myQL_ENABLE_SESSIONS
#    include <boost/thread/locks.hpp>
#    include <boost/thread/shared_mutex.hpp>
#else
#    ifdef myQL_ENABLE_SINGLETON_THREAD_SAFE_INIT
#        include <boost/atomic.hpp>
#        include <boost/thread/mutex.hpp>
#    endif
#endif

#include <memory>
#include <map>

namespace myQuantLib {

typedef std::size_t ThreadKey;
#if defined(myQL_ENABLE_SESSIONS)
    // definition must be provided by the user
    ThreadKey session_id();
/*An example implementation
 #include <thread>
 ThreadKey session_id(){
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
 }
 
 */
#endif

//! Basic support for the singleton pattern.
/*! The typical use of this class is:
    \code
    class Foo : public Singleton<Foo> {
        friend class Singleton<Foo>;
      private:
        Foo() {}
      public:
        ...
    };
    \endcode
    which, albeit sub-optimal, frees one from the concerns of
    creating and managing the unique instance and can serve later
    as a single implemementation point should synchronization
    features be added.

    Global can be used to distinguish Singletons that are local to a session
    (Global = false) or that are global across all sessions (B = true).
    This is only relevant if QL_ENABLE_SESSIONS is enabled.
 */

template <class T, class Global = std::integral_constant<bool, false> >
class Singleton {
public:
    // disable copy/move
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    
    // access to the unique instance
    static T& instance();
    
protected:
    Singleton()=default;
    
private:
#ifdef myQL_ENABLE_SESSIONS
    // construct on first use to avoid static initialization order fiasco
    static std::map<ThreadKey, std::shared_ptr<T>>& m_instances() {
        static std::map<ThreadKey, std::shared_ptr<T>> instances;
        return instances;
    }
    static boost::shared_mutex& m_mutex() {
        static boost::shared_mutex mutex;
        return mutex;
    }
#else
#    ifdef myQL_ENABLE_SINGLETON_THREAD_SAFE_INIT
    static boost::atomic<T*>& m_instance() {
        static boost::atomic<T*> instance;
        return instance;
    }
    static boost::mutex& m_mutex() {
        static boost::mutex mutex;
        return mutex;
    }
#    else
    static std::shared_ptr<T>& m_instance() {
        static std::shared_ptr<T> instance;
        return instance;
    }
#    endif
#endif
};


// template definitions
template <class T, class Global>
T& Singleton<T, Global>::instance(){
#ifdef myQL_ENABLE_SESSIONS
    ThreadKey id = session_id();
    {
        boost::shared_lock<boost::shared_mutex> shared_lock(m_mutex());
        auto instance = Global() ? m_instances().begin(): m_instances().find(id);
        if(instance != m_instances().end())
            return *instance->second;
    }
    {
        boost::unique_lock<boost::shared_mutex> unique_lock(m_mutex());
        auto instance = Global() ? m_instance().begin(): m_instances().find(id);
        if(instance != m_instances().end())
            return *instance->second;
        auto tmp = std::shared_ptr<T>(new T);
        m_instances()[id] = tmp;
        return *tmp;
    }
    
#else
#    ifdef myQL_ENABLE_SINGLETON_THREAD_SAFE_INIT
    // thread safe double checked locking pattern with atomic memory calls
    T* instance = m_instance().load(boost::memory_order_consume);
    if(!instance) {
        boost::mutex::scoped_lock guard(m_mutex());
        instance = m_instance().load(boost::memory_order_consume);
        if(!instance){
            instance = new T();
            m_instance().store(instance, boost::memory_order_release);
        }
    }
    return *instance;
#    else
    // simplest base case without multi-threading
    if(m_instance() == nullptr)
        m_instance() = std::shared_ptr<T>(new T);
    return *m_instance();
#    endif
#endif
}



}





#endif /* singleton_hpp */
