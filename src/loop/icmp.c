#include "../../inc/loop.h"

/**
 * Calculate the ICMP checksum for a buffer.
 *
 * This function computes the Internet Checksum (RFC 1071) over a given buffer.
 *
 * @param ptr: Pointer to the data buffer.
 * @param nbytes: Size of the buffer in bytes.
 * Return The computed checksum.
 */
static unsigned short checksum(unsigned short *ptr, int nbytes) {
	unsigned long sum;
	unsigned short oddbyte;

	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}
	if (nbytes == 1) {
		oddbyte = 0;
		*((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
		sum += oddbyte;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned short) ~sum;
}

/**
 * Fills the ICMP echo request header and adds a timestamp to the payload.
 *
 * The function prepares an ICMP echo request with current timestamp for RTT computation.
 *
 * @param buf: Buffer to store the ICMP packet.
 * @param packet_len: Total length of the packet (header + body).
 *
 * Return 0 on success, -1 on error.
 */
static int fill_icmp_echo_packet(uint8_t *buf, int packet_len) {
	static int seq = 0;
	struct icmphdr *hdr = (struct icmphdr *)buf;
	struct timeval *timestamp = skip_icmphdr(buf);

	if (gettimeofday(timestamp, NULL) == -1) {
		ft_printf("gettimeofday err: %s\n", strerror(errno));
		return -1;
	}
	hdr->type = ICMP_ECHO;
	hdr->un.echo.id = getpid();
	hdr->un.echo.sequence = seq++;
	hdr->checksum = checksum((unsigned short *)buf, packet_len);
	return 0;
}

/**
 * Send an ICMP echo request.
 *
 * Constructs and sends an ICMP ECHO request to the destination.
 *
 * @param sock_fd: Socket file descriptor.
 * @param si: Pointer to remote socket info.
 * @param pi: Pointer to packet tracking info.

 * Return 0 on success, -1 on failure.
 */
int icmp_send_ping(int sock_fd, const t_sockinfo *si, t_packinfo *pi) {
	ssize_t nb_bytes;
	uint8_t buf[sizeof(struct icmphdr) + ICMP_BODY_SIZE] = {};

	if (fill_icmp_echo_packet(buf, sizeof(buf)) == -1)
		return -1;

	nb_bytes = sendto(sock_fd, buf, sizeof(buf), 0,
			  (const struct sockaddr *)&si->remote_addr,
			  sizeof(si->remote_addr));
	if (nb_bytes == -1)
		goto err;
	pi->nb_send++;
	return 0;

err:
	if (errno == EACCES) {
		ft_printf("ft_ping: socket access error. Are you trying "
		       "to ping broadcast ?\n");
	} else {
		ft_printf("sendto err: %s\n", strerror(errno));
	}
	return -1;
}

/**
 * @brief Checks if an ICMP packet is addressed to this process.
 *
 * Discards echo requests from ourselves when pinging localhost.
 *
 * @param buf Pointer to the received ICMP packet.
 * @return true if the packet is an ECHO reply for this process.
 */
static _Bool is_addressed_to_us(uint8_t *buf) {
	struct icmphdr *hdr_sent;
	struct icmphdr *hdr_rep = (struct icmphdr *)buf;

	if (hdr_rep->type == ICMP_ECHO)
		return 0;

	if (hdr_rep->type != ICMP_ECHOREPLY)
		buf += ICMP_HDR_SIZE + IP_HDR_SIZE;
	hdr_sent = (struct icmphdr *)buf;

	return hdr_sent->un.echo.id == getpid();
}

/**
 * Receive an ICMP echo reply from a non-blocking socket.
 *
 * Reads the incoming packet and prints information if it's valid.
 *
 * @param sock_fd: RAW socket file descriptor.
 * @param pi: Packet info tracker.
 * @param opts: Program options.

 * Return 1 if a packet was received, 0 if no data, -1 on error.
 */
int icmp_recv_ping(int sock_fd, t_packinfo *pi, const t_options *opts) {
	uint8_t buf[RECV_PACK_SIZE] = {};
	ssize_t nb_bytes;
	struct icmphdr *icmph;
	struct iovec iov[1] = {
		[0] = { .iov_base = buf, .iov_len = sizeof(buf)}
	};
	struct msghdr msg = { .msg_iov = iov, .msg_iovlen = 1 };

	nb_bytes = recvmsg(sock_fd, &msg, MSG_DONTWAIT);
	if (errno != EAGAIN && errno != EWOULDBLOCK && nb_bytes == -1) {
		ft_printf("recvmsg err: %s\n", strerror(errno));
		return -1;
	} else if (nb_bytes == -1) {
		return 0;
	}
	icmph = skip_iphdr(buf);
	if (!is_addressed_to_us((uint8_t *)icmph))
		return 0;

	if (icmph->type == ICMP_ECHOREPLY) {
		pi->nb_ok++;
		if (rtts_save_new(pi, icmph) == NULL)
			return -1;
	}
	if (print_recv_info(buf, nb_bytes, opts, pi) == -1)
		return -1;
	return 1;
}