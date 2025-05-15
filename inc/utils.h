#ifndef UTILS_H
# define UTILS_H

/*-----------------------------------------------------------------------------
                                LIBRARIES
-----------------------------------------------------------------------------*/

# include "ft_ping.h"

# include <sys/types.h>
/*-----------------------------------------------------------------------------
                                MACROS
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                                STRUCTURES
-----------------------------------------------------------------------------*/

typedef struct s_packinfo   t_packinfo;
typedef struct s_sockinfo   t_sockinfo;
typedef struct s_options    t_options;

/*-----------------------------------------------------------------------------
                                FUNCTIONS
-----------------------------------------------------------------------------*/
void    handler(int signum);
void    print_help();
void    print_start_info(const t_sockinfo *si, const t_options *opts);
void    print_end_info(const t_sockinfo *si, t_packinfo *pi);
int     print_recv_info(void *buf, ssize_t nb_bytes, const t_options *opts, const t_packinfo *pi, const t_sockinfo *si);

#endif