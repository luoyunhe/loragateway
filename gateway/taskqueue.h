#ifndef _TASK_QUEUE_H
#define _TASK_QUEUE_H
#include "sx1278.h"
#include "ctrlqueue.h"
#include <deque>
#include <pthread.h>
#include <map>
#include <string>
#include <list>
#include <tr1/memory>
#include <cstring>
#include <ctime>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>
#include <set>
typedef std::set<uint8_t> id_set;
/*
struct Gpio_Mask{
    volatile bool gpio_2 = false;
    volatile bool gpio_3 = false;
    volatile bool gpio_4 = false;
    volatile bool gpio_5 = false;
};
extern Gpio_Mask* p_gpio_mask;
void enable_isr(int gpio, bool e);

*/

struct arg_event_broadcast_wakeup{
    void add(uint8_t id)
    {
        pthread_mutex_lock(&m_lock);
        m_id.insert(id);
        printf("insert id\n");
        pthread_mutex_unlock(&m_lock);
    }
    void erase(uint8_t id)
    {
        pthread_mutex_lock(&m_lock);
        m_id.erase(id);
        pthread_mutex_unlock(&m_lock);
    }
    pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;
    id_set m_id;
};

/*
struct arg_event_tx{
	arg_event_tx(const void *buf, uint8_t size) : m_size(size)
	{
	    if(m_size > 63)
	        m_size = 63;
	    memcpy(m_buf, buf, m_size);
	}
	uint8_t m_size;
	uint8_t m_buf[64];
};
*/
enum EVENT{
     event_broadcast_wakeup = 0,
     event_search,
     event_normal,
//    event_wakeup,


};
class task{
public:
    task(EVENT e, void *arg) : m_type(e), p_arg(arg){}
    task(EVENT e, void *arg, uint8_t n) : m_type(e), p_arg(arg), m_num(n){}
    ~task();
    EVENT m_type;
    void *p_arg;
    uint8_t m_num;
};
//std::shared_ptr<task> task_factory(EVENT e, )
class task_queue{
public:
    void add(const std::shared_ptr<task>& task);
    std::shared_ptr<task> get_block(void);
private:
    std::deque<std::shared_ptr<task> > m_queue;
    pthread_cond_t m_qready = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m_qlock = PTHREAD_MUTEX_INITIALIZER;
};
class respon_frame{
public:
    respon_frame(const void *d, unsigned char len = 24) : m_size(len)//dangerous!!
    {
        p_data = new char[len];
        memcpy(p_data, d, len);
    }
    ~respon_frame(void)
    {
        delete[] p_data;
    }
    char *p_data;
    unsigned char m_size;
};
/*class id_time{
public:
    id_time(long i, time_t t) : m_id(i), m_time(t){}
    long m_id;
    time_t m_time;
};*/
class lmap{

public:
    lmap(void)
    {
        std::shared_ptr<respon_frame> tmp(new respon_frame("error", 5));
        no_rf = tmp;
    }
    void add(long id, const void* vbuf, uint8_t len);//std::shared_ptr<respon_frame>);
    std::shared_ptr<respon_frame> get(long id);
    void del(long id);
    std::shared_ptr<respon_frame> no_rf;
private:
    std::map<long, std::shared_ptr<respon_frame> > m_map;
   // std::list<id_time> m_list;
    pthread_mutex_t m_mlock = PTHREAD_MUTEX_INITIALIZER;
};
class signal_int{
public:
    bool wait_for_int(const unsigned int& sec)
    {
        //pthread_mutex_lock(&m_lock);
        m_busy = true;
        while(m_busy)
        {
        /*
            clock_gettime(CLOCK_REALTIME, &m_time);
            m_time.tv_sec += sec;
            if(ETIMEDOUT == pthread_cond_timedwait(&m_ready, &m_lock, &m_time))
            {
                pthread_mutex_unlock(&m_lock);
                return false;
            }
            */
        }
        //pthread_mutex_unlock(&m_lock);
        return true;
    }
    void notify(void)
    {
        //pthread_mutex_lock(&m_lock);
        m_busy = false;
        //pthread_mutex_unlock(&m_lock);

        //pthread_cond_signal(&m_ready);
    }

private:
    bool m_busy;
    struct timespec m_time = {0, 0};
    pthread_cond_t m_ready = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m_lock = PTHREAD_MUTEX_INITIALIZER;

};
struct arguement{
    Spi *ptr_spi;
    task_queue *ptr_queue;
    lmap *ptr_map;
    sx1278_gpio *ptr_gpio;
    ctrl_queue *ptr_ctrl_queue;
    signal_int *ptr_int;
    arguement(Spi *spi, task_queue *queue, lmap *map, ctrl_queue * cqueue, sx1278_gpio *gpio, signal_int *i) \
        : ptr_spi(spi), ptr_queue(queue), ptr_map(map), ptr_gpio(gpio), ptr_ctrl_queue(cqueue), ptr_int(i){}
};

typedef std::shared_ptr<task> sp_task;
const arg_event_setting broadcast_mode(1, 0x0a, 6, 7);
const arg_event_setting wakeup_mode(1, 0x0a, 7, 8);
#endif
