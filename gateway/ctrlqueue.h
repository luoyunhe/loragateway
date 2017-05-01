#ifndef _CTRL_QUEUE_H
#define _CTRL_QUEUE_H
#include <deque>
#include <pthread.h>
#include <map>
#include <string>
#include <list>
#include <tr1/memory>
#include <cstring>
#include <ctime>
#include <stdint.h>

enum ctrl_event{
    radio_rec = 0,
    radio_search_done,
    radio_wakeup_done,
    net_interaction,
    net_wakeup
};


class ctrl_task{
public:
    ctrl_task(const ctrl_event e, const void* d, const uint8_t s) : m_type(e), size(s)
    {
        p_arg = new uint8_t[s];
        memcpy(p_arg, d, s);
    }
    ~ctrl_task()
    {
        delete[] p_arg;
    }
    ctrl_event m_type;
    uint8_t *p_arg = NULL;
    uint8_t size = 0;
};
class ctrl_queue{
public:
    void add(const ctrl_event e, const void* d, const uint8_t s)
    {
        std::shared_ptr<ctrl_task> tmp(new ctrl_task(e, d, s));
        add(tmp);
    }
    void add(const std::shared_ptr<ctrl_task>& t);
    std::shared_ptr<ctrl_task> get_block(void);
private:
    std::deque<std::shared_ptr<ctrl_task> > m_queue;
    pthread_cond_t m_qready = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m_qlock = PTHREAD_MUTEX_INITIALIZER;
};

typedef std::shared_ptr<ctrl_task> sp_ctrl_task;
#endif
