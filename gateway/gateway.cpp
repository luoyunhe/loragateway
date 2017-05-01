#include "apue.h"
#include "spi.h"
#include "sx1278.h"
#include "gateway.h"
#include "ctrlqueue.h"
#include "taskqueue.h"
#include "protocol_layer.h"
#include "network.h"
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include <iostream>

//#include "DieWithMessage.h"
//#include "TCPClientUtility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFSIZE 512

static pthread_t sx1278_a;
static pthread_t sx1278_b;
static pthread_t network;
task_queue queue_a;
task_queue queue_b;
net_queue* ptr_net_queue;
ctrl_queue* ptr_ctrl_queue;
signal_int int_a;
signal_int int_b;
using namespace std;
int main(int argc, char *argv[])
{
    int err;
    void *tret;
    uint8_t channel = 0;
    arg_event_broadcast_wakeup wakeup_id;
    task_queue * sx1278_a_queue = &queue_a;
    task_queue * sx1278_b_queue = &queue_b;
    ptr_ctrl_queue = new ctrl_queue();
    ptr_net_queue = new net_queue();
    p_gpio_mask = new Gpio_Mask();
    lmap *sx1278_a_map = new lmap();
    lmap *sx1278_b_map = new lmap();
    //create sx1278_a thread
    err = pthread_create(&sx1278_a, NULL, sx1278,   \
            static_cast<void*>(new arguement(getSpi0(),\
            	sx1278_a_queue, sx1278_a_map, ptr_ctrl_queue,\
            		new sx1278_gpio(17, 27), &int_a)));
    if(err != 0)
        err_exit(err, "can't create thread_sx1278_a");
    //create sx1278_b thread
    err = pthread_create(&sx1278_b, NULL, sx1278,   \
            static_cast<void*>(new arguement(getSpi1(),\
            	sx1278_b_queue, sx1278_b_map, ptr_ctrl_queue,\
            		new sx1278_gpio(18, 23), &int_b)));
    if(err != 0)
        err_exit(err, "can't create thread_sx1278_b");
    //create network thread
	 err = pthread_create(&network, NULL, net_thread,   \
            static_cast<void*>(new arg_network_thread(ptr_ctrl_queue, ptr_net_queue)));
    if(err != 0)
        err_exit(err, "can't create thread_network");

    sp_task esearch(new task(event_search, NULL));
    bool* normal_run = new bool(true);
    queue_b.add(esearch);
    {
        sp_ctrl_task ct = ptr_ctrl_queue->get_block();
        if(ct->m_type == radio_search_done)
        {
            channel = (ct->p_arg)[0];
            printf("work in channel %d\n", channel);
        }
    }
    sp_task enormal(new task(event_normal, static_cast<void*>(normal_run), channel));
    sp_task ebcwu(new task(event_broadcast_wakeup, static_cast<void*>(&wakeup_id)));
    queue_b.add(ebcwu);
    queue_a.add(enormal);
    //sx1278_a_map->add('3', "hello", 5);
    while(1)
    {
        sp_ctrl_task t = ptr_ctrl_queue->get_block();
        switch(t->m_type)
        {
            case radio_rec:
                printf("deal radio_rec\n");
                ptr_net_queue->add(net_rec, t->p_arg, t->size);
                printf("id = %d, sevice num = %d, datesize = %d,data: hello\n",t->p_arg[0],t->p_arg[2],t->p_arg[3]);
                break;
            case radio_wakeup_done:
            {
                printf("deal radio_wakeup_done\n");
                wakeup_id.erase((t->p_arg)[0]);
                ptr_net_queue->add(net_wakeup_done, t->p_arg, t->size);
                break;
            }
            case net_wakeup:
            {
                uint8_t id = t->p_arg[0];
                printf("wakeup_id = %c\n", id);
                for(int i = 0; i<t->size; ++i)
                    printf("%c",t->p_arg[i]);
                printf("\n");
                wakeup_id.add(id);
                sx1278_a_map->add(id, t->p_arg, t->size);
                break;
            }
            default:
            break;
        }
    }




    //join
    err = pthread_join(sx1278_a, &tret);
    if(err != 0)
        err_exit(err, "can't join sx1278_a");

    err = pthread_join(sx1278_b, &tret);
    if(err != 0)
        err_exit(err, "can't join sx1278_b");
}




