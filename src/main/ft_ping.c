#include "../../inc/ft_ping.h"

_Bool pingloop = 1;
_Bool send_packet = 1;
t_packinfo *g_pi = NULL;

/**
 * Signal handler for SIGINT and SIGALRM.
 *
 * Handles program termination and scheduled packet sending.
 *
 * @param signum: The signal number received. Expected: SIGINT or SIGALRM.
 */
void    handler(int signum) {
    if (signum == SIGINT) {
        gettimeofday(&g_pi->end_time, NULL);
        pingloop = 0;
    }
    else if (signum == SIGALRM)
        send_packet = 1;
}

/**
 * Determines whether the ping loop should stop.
 *
 * Used to control the main loop termination when a packet count is specified.
 *
 * @param pi: Pointer to the packet information structure.
 * @param opts: Pointer to the user options structure.
 *
 * @return: true if the sending count is reached and either:
 * - All expected replies have been received, or
 * - One second has passed since the last packet was sent.
 * - False otherwise
 */
_Bool    should_stop(t_packinfo *pi, t_options *opts) {
    if (opts->count != -1) {
        if (pi->nb_send >= opts->count) {
            struct timeval current_time;
            gettimeofday(&current_time, NULL);

            if (pi->nb_recv >= opts->count ||
                (pi->last_send_time.tv_sec + 1 < current_time.tv_sec ||
                (pi->last_send_time.tv_sec + 1 == current_time.tv_sec &&
                 pi->last_send_time.tv_usec <= current_time.tv_usec))) {
                return 1;
                 }
        }
    }
    return 0;
}

/**
 * Arm a one-shot real-time interval timer to schedule the next ping.
 *
 * Sets the ITIMER_REAL timer with the given interval (in seconds).
 * The timer is non-repeating (one-shot), used to trigger SIGALRM.
 *
 * @param interval The delay in seconds before the timer expires (can be fractional).
 */
static void set_ping_timer(float interval) {
    struct itimerval timer;
    timer.it_value.tv_sec = (int)interval;
    timer.it_value.tv_usec = (int)((interval - (int)interval) * 1e6);
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
}

int main(int argc, char **argv) {
    int ret;
    int sock_fd;
    char *host = NULL;
    t_options opts = { .count = -1, .interval = 1.0f, .ttl = 64, };
    t_sockinfo si = {};
    t_packinfo pi = {
        .last_send_time = {0, 0},
    };

    if (check_rights() == -1)
        return E_EXIT_ERR_ARGS;
    if ((ret = parse_args(argc, argv, &host, &opts)) != 0)
        return ret == -1 ? E_EXIT_ERR_ARGS : E_EXIT_OK;
    if (init_sock(&sock_fd, &si, host, opts.ttl) == -1)
        return E_EXIT_ERR_HOST;

    signal(SIGINT, &handler);
    signal(SIGALRM, &handler);
    g_pi = &pi;

    print_start_info(&si, &opts);
    while (pingloop) {
        if (send_packet && (opts.count == -1 || pi.nb_send < opts.count)) {
            send_packet = 0;
            if (icmp_send_ping(sock_fd, &si, &pi) == -1)
                goto fatal_close_sock;
            gettimeofday(&pi.last_send_time, NULL);
            set_ping_timer(opts.interval);
        }
        if (icmp_recv_ping(sock_fd, &pi, &opts, &si) == -1)
            goto fatal_close_sock;
        if (should_stop(&pi, &opts)) {
            gettimeofday(&pi.end_time, NULL);
            pingloop = 0;
        }
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