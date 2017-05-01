#ifndef __GPIO_CTRL_H__
#define __GPIO_CTRL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */

#ifdef __cplusplus
extern "C" {
#endif
int gpio_wait_for_int(int fd, int ms);
int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);
int gpio_set_edge(unsigned int gpio, const char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd);
#ifdef __cplusplus
}
#endif
#endif
