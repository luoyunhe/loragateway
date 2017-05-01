#include "protocol_layer.h"
#include <sys/time.h>
#include <wiringPi.h>


void *sx1278(void *a)
{
    arguement *arg = static_cast<arguement*>(a);
	SX1278 sx(arg->ptr_spi, arg->ptr_gpio);
    sx1278_gpio* p_gpio = arg->ptr_gpio;
    task_queue *p_queue = arg->ptr_queue;
	lmap *p_map = arg->ptr_map;
	ctrl_queue *p_ctrl = arg->ptr_ctrl_queue;
    signal_int *p_int = arg->ptr_int;
    int mystate;
    while(1)
    {
        std::shared_ptr<task> t = p_queue->get_block();
	    char rxbuf[32];
	    uint8_t size;
	    switch(t->m_type)
	    {
			case event_search:
			{
				struct timeval time_begin, time_now;
				bool channel[6] = {true, true, true, true, true, true};
				cad_no_int:
				sx.Init();
				sx.setting(&broadcast_mode);
				gettimeofday(&time_begin, NULL);
				cad_again:
				sx.entryCAD();
                mystate = sx.waitForInterrupt(3000);
                if(mystate > 0)//(p_int->wait_for_int(3))
				{
					byte value = sx.readRegister(REG_IRQ_FLAGS);
					sx.clearFlags();
					// look for the CAD detected bit
					if( bitRead(value, 0) == 1 )
					{
						// we detected activity
#if ( debug_mode>1 )
						//printf("cad detected!!!");
#endif
						sx.entryRx();
                        mystate = sx.waitForInterrupt(3000);
                        if(mystate > 0)//(p_int->wait_for_int(3))
						{
							byte value = sx.readRegister(REG_IRQ_FLAGS);
							sx.clearFlags();
							if( bitRead(value, 5) == 1)
							{
								printf("crc err\n");
								gettimeofday(&time_now, NULL);
								if((time_now.tv_sec - time_begin.tv_sec) < 8)
								goto cad_again;
							}
							else
							{
								size = sx.recSinglePackage(rxbuf);
                                uint8_t i = rxbuf[2] - '0';
								if(size == 3 && rxbuf[0] == 'c' && rxbuf[1] == 'h')
									channel[i] = false;
								gettimeofday(&time_now, NULL);
								if((time_now.tv_sec - time_begin.tv_sec) < 8)
									goto cad_again;
						  }
						}

					}
					else
					{
						gettimeofday(&time_now, NULL);
						if((time_now.tv_sec - time_begin.tv_sec) < 8)
							goto cad_again;

					}
				}
				else if(mystate == 0)
				{
					goto cad_no_int;
				}
	                uint8_t i = 0;
	                while(!channel[i] && i < 6)
                    {
                        printf("channel %d is busy\n",i);
	                    i++;
                    }
	                if(i < 6)
                    {
	                    sx.m_channel = i;
                        printf("set gateway to work in channel %d\n", i);
                    }
                    std::shared_ptr<ctrl_task> search_tmp(new ctrl_task(radio_search_done, &sx.m_channel, 1));
                    p_ctrl->add(search_tmp);
                    printf("search_done!\n");
		            break;
	            }
			case event_broadcast_wakeup:
			{
	                arg_event_broadcast_wakeup* a = static_cast<arg_event_broadcast_wakeup*>(t->p_arg);
	                struct timeval t_begin, t_now;

	                sx.Init();
	                while(1)
	                {
                        std::set<uint8_t> idset;
						sx.setting(&broadcast_mode);
	                    gettimeofday(&t_begin, NULL);
		                char bc[8];
		                sprintf(bc,"ch%d",sx.m_channel);
		                sx.sendSinglePayload(bc, 3);
		                //if(p_int->wait_for_int(4));
                        mystate = sx.waitForInterrupt(3000);
		                sx.setting(&wakeup_mode);
		                pthread_mutex_lock(&(a->m_lock));
                        idset = a->m_id;
                        printf("i get id set%d\n");
		                pthread_mutex_unlock(&(a->m_lock));
						std::set<uint8_t>::iterator iter = idset.begin();
		                while(iter != idset.end())
					    {
					        char wu[8];
			                sprintf(wu, "id%c", *iter);
			                printf(wu, "id%c\n", *iter);
			                sx.sendSinglePayload(wu, 3);
		                    //if(p_int->wait_for_int(4));
                            mystate = sx.waitForInterrupt(3000);
					        ++iter;
					    }
						gettimeofday(&t_now, NULL);
	                    char gap = t_begin.tv_sec - t_now.tv_sec + 3;
	                    if(gap > 0)
	                    	sleep(gap);
	                }

					break;
	            }
			case event_normal:
			{
	                arg_event_setting normal(1, 0x0a, 7, sx.m_channel);
	                sx.Init();
	                sx.setting(&normal);
                    bool *run = static_cast<bool*>(t->p_arg);
                    sx.m_channel = t->m_num;
	                while(*run)
	                {
	 					sx.entryCAD();
                        mystate = sx.waitForInterrupt(3000);
		                if(mystate > 0)
		                {
							byte value = sx.readRegister(REG_IRQ_FLAGS);
							sx.clearFlags();
							// look for the CAD detected bit
							if( bitRead(value, 0) == 1 )
							{
								// we detected activity
#if ( debug_mode>1 )
				                //printf("cad detected!!!2\n");
#endif
								sx.entryRx();
                                mystate = sx.waitForInterrupt(3000);
                                //printf("state = %d\n", mystate);
                                if(mystate > 0)
		                        {
                                    printf("return\n");
					                byte value = sx.readRegister(REG_IRQ_FLAGS);
									sx.clearFlags();
									if( bitRead(value, 5) == 1)
					                {
					                	printf("crc err\n");
					                }
		                            else
		                            {
                                        uint8_t id, action;
		              					size = sx.recSinglePackage(rxbuf);
	                                    sscanf(rxbuf,"%c %c",&id, &action);
					                    switch(action)
					                    {
											case '1':
					                        {
					                            char ack[8];
					                            sprintf(ack,"%cOK",id);
					                            sx.sendSinglePayload(ack, strlen(ack));
                                                mystate = sx.waitForInterrupt(3000);
	                                           // p_int->wait_for_int(4);
                                                //printf("hui fu wancheng\n");
					                            std::shared_ptr<ctrl_task> tmp(new ctrl_task(radio_rec, rxbuf, size));
					                            p_ctrl->add(tmp);
                                                printf("action 1 \n");
					                            break;
					                        }
					                        case '2':
					                        {
					                            std::shared_ptr<respon_frame> rf(p_map->get(id));
					                            sx.sendSinglePayload(rf->p_data, rf->m_size);
                                                mystate = sx.waitForInterrupt(3000);
	                                            //p_int->wait_for_int(4);
					                            std::shared_ptr<ctrl_task> tmp(new ctrl_task(radio_wakeup_done, &id, sizeof(id)));
                                                p_ctrl->add(tmp);
                                                printf("action 2 \n");
					                            break;
					                        }
                                            case '3':
                                            {
					                            std::shared_ptr<respon_frame> rf(p_map->get(id));
					                            sx.sendSinglePayload(rf->p_data, rf->m_size);
                                                mystate = sx.waitForInterrupt(3000);
	                                            //p_int->wait_for_int(4);
                                                printf("action 3 \n");
                                                break;
                                            }
                                            default:
                                            break;

				                         }
					                }

									}
								}
							}

						}
						break;
					}
		}
	}
}




