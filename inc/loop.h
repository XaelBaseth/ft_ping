#ifndef LOOP_H
# define LOOP_H

/*-----------------------------------------------------------------------------
                                LIBRARIES
-----------------------------------------------------------------------------*/

# include "ft_ping.h"
# include <netinet/ip_icmp.h>

/*-----------------------------------------------------------------------------
                                MACROS
-----------------------------------------------------------------------------*/

# define RECV_PACK_SIZE ((IP_HDR_SIZE + ICMP_HDR_SIZE) * 2 + ICMP_BODY_SIZE + 1)

/*-----------------------------------------------------------------------------
                                STRUCTURES
-----------------------------------------------------------------------------*/

typedef struct s_packinfo   t_packinfo;
typedef struct s_sockinfo   t_sockinfo;
typedef struct s_options    t_options;
typedef struct s_rtt_node   t_rtt_node;

/*-----------------------------------------------------------------------------
                                FUNCTIONS
-----------------------------------------------------------------------------*/
int         icmp_recv_ping(int sock_fd, t_packinfo *pi, const t_options *opts);
int         icmp_send_ping(int sock_fd, const t_sockinfo *si, t_packinfo *pi);
void        rtts_calc_stats(t_packinfo *pi);
void        calc_stddev(t_packinfo *pi, long nb_elem);
void        rtts_clean(t_packinfo *pi);
t_rtt_node  *rtts_save_new(t_packinfo *pi, struct icmphdr *icmph);

#endif