#include "ctrlqueue.h"
#include <stdio.h>

void ctrl_queue::add(const std::shared_ptr<ctrl_task>& t)
{
    pthread_mutex_lock(&m_qlock);
    m_queue.push_back(t);
    pthread_mutex_unlock(&m_qlock);
    pthread_cond_signal(&m_qready);
}
std::shared_ptr<ctrl_task> ctrl_queue::get_block(void)
{
    pthread_mutex_lock(&m_qlock);
    while(m_queue.empty())
    {
        pthread_cond_wait(&m_qready, &m_qlock);
    }
    std::shared_ptr<ctrl_task> ret = m_queue.front();
    m_queue.pop_front();
    pthread_mutex_unlock(&m_qlock);
    return ret;
}
