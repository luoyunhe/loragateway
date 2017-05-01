#ifndef __NETWORK__H_
#define __NETWORK__H_
#include "ctrlqueue.h"
#include <deque>
#include <pthread.h>
#include <map>
#include <string>
#include <list>
#include <tr1/memory>
#include <cstring>
#include <ctime>
#include <stdint.h>

enum net_event{
    net_rec = 0,
    net_wakeup_done,
    net_data_available
};

class net_task{
public:
    net_task(const net_event e, const void* d, const uint8_t s) :m_type(e),size(s)
    {
        if(s > 0)
        {
            p_arg = new uint8_t[s];
            memcpy(p_arg, d, s);
        }
    }
    ~net_task()
    {
        if(size > 0)
        {
            delete[] p_arg;
        }

    }
    net_event m_type;
    uint8_t *p_arg = NULL;
    uint8_t size = 0;
};
class net_queue{
public:
    void add(const std::shared_ptr<net_task>& t);
    void add(const net_event e, const void* p, const uint8_t s);
    std::shared_ptr<net_task> get_block(void);
private:
    std::deque<std::shared_ptr<net_task> > m_queue;
    pthread_cond_t m_qready = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m_qlock = PTHREAD_MUTEX_INITIALIZER;
};
struct arg_network_thread{
    arg_network_thread(ctrl_queue* c, net_queue* n) : ptr_ctrl_queue(c), ptr_net_queue(n){}
    net_queue* ptr_net_queue;
    ctrl_queue* ptr_ctrl_queue;
};
void *net_thread(void *a);

typedef std::shared_ptr<net_task> sp_net_task;



#endif
