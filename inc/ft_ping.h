#ifndef FT_PING_H
# define FT_PING_H

/*-----------------------------------------------------------------------------
								LIBRARIES
-----------------------------------------------------------------------------*/
# include "init.h"
# include "loop.h"
# include "utils.h"

# include "../lib/libft/inc/ft_gc_alloc.h"
# include "../lib/libft/inc/ft_printf.h"
# include "../lib/libft/inc/get_next_line.h"
# include "../lib/libft/inc/libft.h"

# include <errno.h>
# include <math.h>
# include <netdb.h>
# include <signal.h>
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <bits/socket.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>

/*-----------------------------------------------------------------------------
								MACROS
-----------------------------------------------------------------------------*/
# define IP_TTL_VALUE 64
# define IP_HDR_SIZE (sizeof(struct iphdr))
# define ICMP_HDR_SIZE (sizeof(struct icmphdr))
# define ICMP_BODY_SIZE 56

extern _Bool pingloop;
extern _Bool send_packet;
extern t_packinfo *g_pi;


enum    e_exitcode {
    E_EXIT_OK,
    E_EXIT_ERR_HOST,
    E_EXIT_ERR_ARGS = 64
};

/*-----------------------------------------------------------------------------
								STRUCTURES
-----------------------------------------------------------------------------*/
typedef struct    s_options {
    _Bool         help;
    _Bool         quiet;
    _Bool         verb;
    _Bool         timestamp;
    int           count;
}                 t_options;

typedef struct      s_rtt_node {
    struct timeval  val;
    struct s_rtt_node *next;
}                   t_rtt_node;

typedef struct        s_packinfo {
    int               nb_send;
    int               nb_ok;
    int               nb_recv;
    struct timeval    *min;
    struct timeval    *max;
    struct timeval    avg;
    struct timeval    stddev;
    struct timeval    start_time;
    struct timeval    end_time;
    struct timeval    last_send_time;
    t_rtt_node        *rtt_list;
    t_rtt_node        *rtt_last;
}                     t_packinfo;

typedef struct            s_sockinfo {
    char                  *host;
    struct sockaddr_in    remote_addr;
    char                  str_sin_addr[INET_ADDRSTRLEN];
}                         t_sockinfo;



/*-----------------------------------------------------------------------------
								FUNCTIONS
-----------------------------------------------------------------------------*/

static inline void * skip_iphdr(void *buf){
    return (void *)((uint8_t *)buf + IP_HDR_SIZE);
}

static inline void * skip_icmphdr(void *buf){
    return (void *)((uint8_t *)buf + ICMP_HDR_SIZE);
}


int check_rights(void);
int parse_args(int argc, char **argv, char **host, t_options *opts);

#endif