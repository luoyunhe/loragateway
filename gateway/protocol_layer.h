#ifndef __PRO_LAYER__
#define __PRO_LAYER__
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include "sx1278.h"
#include "taskqueue.h"
#include "ctrlqueue.h"
#include <tr1/memory>
void *sx1278(void *arg);
//void deal(SX1278 &sx, std::shared_ptr<task> t);
#endif