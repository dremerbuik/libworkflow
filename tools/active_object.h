#ifndef __ACTIVE_OBJECT_H_
#define __ACTIVE_OBJECT_H_


#include <tools/shared_ptr.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

SHARED_PTR(ActiveObject);
typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;
namespace boost {
    class thread;
    class recursive_mutex;
};

/**
 ActiveObject is thread given a life time. 
 By default, it starts on construction, but can be delayed. in this case, the  active object expect to be started using start() function.
 
 When calling, from any thread, stop(), all active request planned will finish and then the object itself will stop.
 
 Thus, expect started() to be called from the thread once it's well started.
 stopped() will be called on this thread as well just before it's closure.
 */
class ActiveObject : public boost::enable_shared_from_this<ActiveObject> {
    const std::string name;
    IOServicePtr service;
    boost::shared_ptr<boost::thread> thread;
    boost::shared_ptr<boost::asio::io_service::work> worker;
    boost::shared_ptr<boost::recursive_mutex> mutex;
public:
    ActiveObject(const std::string &name, bool delay_start = false);
    ActiveObject(const ActiveObject &) = delete; // disallow copy.
    virtual ~ActiveObject();
    
    //! if not already running, will start the thread and io_service.
    void start();
    
    //! async stops the thread.
    void stop();
    
    //! sync stop the thread.
    void terminate();
    
    IOServicePtr getIOService();
    
    const std::string & getName() const;
    
protected:
    virtual void started();
    virtual void stopped();

private:
    void run();
};

OSTREAM_HELPER_DECL(ActiveObject);

#endif // __ACTIVE_OBJECT_H_