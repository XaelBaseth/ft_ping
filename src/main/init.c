#include "../../inc/init.h"

/**
 * Resolve the IPv4 address of the target host and initialize the corresponding
 * sockaddr_in structure and printable IP string.
 *
 * @param si Pointer to the sockinfo structure to initialize.
 *
 * @return 0 on success, -1 on error (e.g., resolution failure or inet_ntop).
 */
static int init_sock_addr(t_sockinfo *si)
{
    struct addrinfo hints;
    struct addrinfo *res = NULL;

    ft_memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    int ret = getaddrinfo(si->host, NULL, &hints, &res);
    if (ret != 0) {
        ft_printf("ft_ping: unknown host '%s': %s\n", si->host, gai_strerror(ret));
        return -1;
    }

    memcpy(&si->remote_addr, res->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(res);

    if (inet_ntop(AF_INET, &si->remote_addr.sin_addr, si->str_sin_addr,
        INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        return -1;
        }
    return 0;
}

/**
 * Create a raw socket for sending ICMP echo requests and set the TTL value at
 * the IP level.
 *
 * @param ttl Time To Live value to be set for outgoing packets.
 *
 * @return File descriptor of the created socket on success, -1 on error.
 *
 */
static int create_socket(uint8_t ttl)
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1) {
        perror("setsockopt (IP_TTL)");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

/**
 * Initialize the socket and remote address information for the given host.
 *
 * This includes:
 * - Resolving the host to an IPv4 address (DNS or IP literal),
 * - Creating a raw socket for ICMP echo requests,
 * - Setting the TTL on the socket.
 *
 * @param sock_fd Pointer to the resulting socket file descriptor.
 * @param si Pointer to a sockinfo structure to be populated.
 * @param host The target host to resolve and ping.
 * @param ttl Time To Live value for the IP header.
 *
 * @return 0 on success, -1 on failure. The socket will not be initialized on failure.
 */
int init_sock(int *sock_fd, t_sockinfo *si, char *host, int ttl)
{
    si->host = host;

    if (init_sock_addr(si) == -1)
        return -1;

    int fd = create_socket(ttl);
    if (fd == -1)
        return -1;

    *sock_fd = fd;
    return 0;
}
