#include "../../inc/ft_ping.h"

_Bool pingloop = 1;
_Bool send_packet = 1;

void    handler(int signum) {
    if (signum == SIGINT)
        pingloop = 0;
    else if (signum == SIGALRM)
        send_packet = 1;
}

int main(int argc, char **argv) {
    int ret;
    int sock_fd;
    char *host = NULL;
    t_options opts = {};
    t_sockinfo si = {};
    t_packinfo pi = {};

    if (check_rights() == -1)
        return E_EXIT_ERR_ARGS;
    if ((ret = parse_args(argc, argv, &host, &opts)) != 0)
        return ret == -1 ? E_EXIT_ERR_ARGS : E_EXIT_OK;
    if (init_sock(&sock_fd, &si, host, IP_TTL_VALUE) == -1)
        return E_EXIT_ERR_HOST;

    signal(SIGINT, &handler);
    signal(SIGALRM, &handler);

    print_start_info(&si, &opts);
    while (pingloop) {
        if (send_packet) {
            send_packet = 0;
            if (icmp_send_ping(sock_fd, &si, &pi) == -1)
                goto fatal_close_sock;
            alarm(1);
        }
        if (icmp_recv_ping(sock_fd, &pi, &opts) == -1)
            goto fatal_close_sock;
    }
    print_end_info(&si, &pi);

    close(sock_fd);
    rtts_clean(&pi);
    return pi.nb_ok > 0 ? E_EXIT_OK : E_EXIT_ERR_HOST;

    fatal_close_sock:
        close(sock_fd);
    rtts_clean(&pi);
    return E_EXIT_ERR_HOST;
}