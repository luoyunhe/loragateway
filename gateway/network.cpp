#include "network.h"
#include "DieWithMessage.h"
#include "TCPClientUtility.h"
#include "apue.h"
#include <pthread.h>
#include <poll.h>
#define BUFSIZE 512
static pthread_t notify_thread_id;
struct arg_notify_thread{
	arg_notify_thread(net_queue* p, int s) : p_queue(p), m_socket(s){}
	net_queue* p_queue;
    int m_socket;
};
void *notify_thread(void* a)
{
	arg_notify_thread* arg = static_cast<arg_notify_thread*>(a);
	int sock = arg->m_socket;
    net_queue* ptr_net_queue = arg->p_queue;
    int timeout = 100000;
    struct pollfd pollfds;
    pollfds.fd = sock;
    pollfds.events = POLLIN | POLLPRI;
    while(1)
    {
	    switch(poll(&pollfds,1,timeout))
        {       //开始监控
		    case -1:                    //函数调用出错
		        printf("poll error \r\n");
		    break;
		    case 0:
                printf("net time out \r\n");
		    break;
		    default:                    //得到数据返回
		        ptr_net_queue->add(net_data_available, NULL, 0);
                sleep(1);
		    break;
	    }
    }
}
void *net_thread(void *a)
{
	arg_network_thread* arg = static_cast<arg_network_thread*>(a);
    net_queue* p_net = arg->ptr_net_queue;
    ctrl_queue* p_ctrl = arg->ptr_ctrl_queue;
    char buffer[BUFSIZE];
    char packet[BUFSIZE];
    int  packet_index = 0;
    char id = 0, action = 0;
    char data[] = "END"; //连接时发一些数据来识别是网关
    char hello[] = "HelloEND";
    char  *server = "114.215.111.84"; //IP
    char *service = "1234"; //Port
    int sock;
    while(1)
    {
        sock = SetupTCPClientSocket(server, service);
        printf("socket is %d\n", sock);
        if(sock < 0)
        {
            DieWithUserMessage("SetupTCPClientSocket() failed", "unable to connect");
            sleep(10);
        }
        else
            break;
    }
    send(sock, hello, 8, 0);

         //create a notify  thread
    int err = pthread_create(&notify_thread_id, NULL, notify_thread, static_cast<void*>(new arg_notify_thread(p_net, sock)));
    if(err != 0)
        err_exit(err, "can't create thread_sx1278_a");
     while(1)
    {
        sp_net_task t = p_net->get_block();
        switch(t->m_type)
        {
            case net_rec:
                printf("to %d\n", t->size);
                memcpy(buffer, t->p_arg , t->size);
                memcpy(buffer + t->size, data , 4);
                send(sock, buffer, t->size + 3, 0);
                printf("to server %s\n", buffer);
                break;
            case net_wakeup_done:
            {

                break;
            }
            case net_data_available:
            {
                int len = recv(sock, buffer, BUFSIZE, 0);
                for(int i = 0; i < len; ++i)
                {
                    if(buffer[i] == '\0')
                    {
                        if(packet[1] == '2')
                            p_ctrl->add(net_wakeup, packet, packet_index + 1);
                        else if(packet[1] == '1')
                            p_ctrl->add(net_interaction, packet, packet_index + 1);
                        packet[packet_index] = '\0';
                        printf("recv = %s\n", packet);
                        packet_index = 0;
                    }
                    else
                    {
                        packet[packet_index++] = buffer[i];
                    }
                }
                break;
            }
            default:
            break;
        }

    }
}

void net_queue::add(const std::shared_ptr<net_task>& t)
{
    pthread_mutex_lock(&m_qlock);
    if(m_queue.size() > 49)
        return;
    m_queue.push_back(t);
    pthread_mutex_unlock(&m_qlock);
    pthread_cond_signal(&m_qready);
}
void net_queue::add(const net_event e, const void* p, const uint8_t s)
{
	sp_net_task tmp(new net_task(e, p, s));
    add(tmp);
}
std::shared_ptr<net_task> net_queue::get_block(void)
{
    pthread_mutex_lock(&m_qlock);
    while(m_queue.empty())
    {
        pthread_cond_wait(&m_qready, &m_qlock);
    }
	std::shared_ptr<net_task> ret = m_queue.front();
	m_queue.pop_front();
    pthread_mutex_unlock(&m_qlock);
    return ret;
}



