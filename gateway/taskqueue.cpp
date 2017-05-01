#include "taskqueue.h"
#include <stdio.h>
//Gpio_Mask* p_gpio_mask = NULL;
task::~task(void)
{
	/*
	switch(m_type)
	{
		case event_setting:
			delete static_cast<arg_event_setting *>(p_arg);
		break;
        case event_tx:
            delete static_cast<arg_event_tx *>(p_arg);
        break;
        default:
        break;
	}
	*/
}
void task_queue::add(const std::shared_ptr<task>& t)
{
    pthread_mutex_lock(&m_qlock);
    m_queue.push_back(t);
    pthread_mutex_unlock(&m_qlock);
    pthread_cond_signal(&m_qready);
}
std::shared_ptr<task> task_queue::get_block(void)
{
    pthread_mutex_lock(&m_qlock);
    while(m_queue.empty())
    {
        pthread_cond_wait(&m_qready, &m_qlock);
    }
    std::shared_ptr<task> ret = m_queue.front();
    m_queue.pop_front();
    pthread_mutex_unlock(&m_qlock);
    return ret;
}
void lmap::add(long id, const void* vbuf, uint8_t len)//std::shared_ptr<respon_frame> ptr)
{
    std::shared_ptr<respon_frame> ptr(new respon_frame(vbuf, len));
    pthread_mutex_lock(&m_mlock);
    if(!m_map.insert(std::map<long, std::shared_ptr<respon_frame> >::value_type(id, ptr)).second)
    {
        m_map.erase(id);
        m_map.insert(std::map<long, std::shared_ptr<respon_frame> >::value_type(id, ptr));
    }
    pthread_mutex_unlock(&m_mlock);
}
std::shared_ptr<respon_frame> lmap::get(long id)
{
    pthread_mutex_lock(&m_mlock);
    if(m_map.count(id) == 1)
    {
        std::shared_ptr<respon_frame> ptr = m_map[id];
        pthread_mutex_unlock(&m_mlock);
        return ptr;
    }
    else
    {
        pthread_mutex_unlock(&m_mlock);
        printf("err:no respon_frame\n");
        return no_rf;
    }
}
void lmap::del(long id)
{
    pthread_mutex_lock(&m_mlock);
    m_map.erase(id);
    pthread_mutex_unlock(&m_mlock);
}
/*
void enable_isr(int gpio, bool e)
{
    switch(gpio)
    {
        case 2:
            p_gpio_mask->gpio_2 = e;
        break;
        case 3:
            p_gpio_mask->gpio_3 = e;
        break;
        case 4:
            p_gpio_mask->gpio_4 = e;
        break;
        case 5:
            p_gpio_mask->gpio_5 = e;
        break;
    }
}
*/
