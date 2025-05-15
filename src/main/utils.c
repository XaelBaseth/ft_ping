#include "../../inc/utils.h"

/**
 * Print the usage and options for the ft_ping command.
 *
 * Displays help text with the available command-line options.
 * Called when the user passes -h or on incorrect usage.
 */
void print_help() {
	ft_printf("Usage: ft_ping [OPTION...] HOST ...\n"
	       "Send ICMP ECHO_REQUEST packets to network hosts.\n\n"
	       "Options:\n"
           "\t<HOST>\t\t\t\tDNS name or IP address v4\n"
	       "\t-?\t\t\t\tShow help\n"
           "\t-c <count>\t\t\tstop after <count> replies\n"
           "\t-D\t\t\t\tprint timestamp UNIX style\n"
           "\t-h\t\t\t\tShow help\n"
	       "\t-q\t\t\t\tQuiet output\n"
	       "\t-v\t\t\t\tVerbose output\n\n");
}

/**
 * Print initial information before starting to send ICMP echo requests.
 *
 * Displays the host name, resolved IP address, and number of data bytes.
 * If verbose is enabled, also prints the process ID in hex and decimal.
 *
 * @param si: Pointer to socket information structure.
 * @param opts: Pointer to options structure.
 */
void print_start_info(const t_sockinfo *si, const t_options *opts) {
	int pid;

	ft_printf("PING %s (%s): %d data bytes", si->host, si->str_sin_addr,
	       ICMP_BODY_SIZE);
	if (opts->verb) {
		pid = getpid();
		ft_printf(", id 0x%04x = %d", pid, pid);
	}
	ft_printf("\n");
}

/**
 * Print a human-readable error message for a given ICMP error type and code.
 *
 * @param type: ICMP type (e.g. ICMP_DEST_UNREACH, ICMP_SOURCE_QUENCH).
 * @param code: ICMP code specifying the subtype of error.
 */
static void print_icmp_err(int type, int code) {
	switch (type) {
	case ICMP_DEST_UNREACH:
		switch(code) {
		case ICMP_NET_UNREACH:
			ft_printf("Destination Net Unreachable\n");
			break;
		case ICMP_HOST_UNREACH:
			ft_printf("Destination Host Unreachable\n");
			break;
		case ICMP_PROT_UNREACH:
			ft_printf("Destination Protocol Unreachable\n");
			break;
		case ICMP_PORT_UNREACH:
			ft_printf("Destination Port Unreachable\n");
			break;
		case ICMP_FRAG_NEEDED:
			ft_printf("Frag needed\n");
			break;
		case ICMP_SR_FAILED:
			ft_printf("Source Route Failed\n");
			break;
		case ICMP_NET_UNKNOWN:
			ft_printf("Destination Net Unknown\n");
			break;
		case ICMP_HOST_UNKNOWN:
			ft_printf("Destination Host Unknown\n");
			break;
		case ICMP_HOST_ISOLATED:
			ft_printf("Source Host Isolated\n");
			break;
		case ICMP_NET_ANO:
			ft_printf("Destination Net Prohibited\n");
			break;
		case ICMP_HOST_ANO:
			ft_printf("Destination Host Prohibited\n");
			break;
		case ICMP_NET_UNR_TOS:
			ft_printf("Destination Net Unreachable for Type of Service\n");
			break;
		case ICMP_HOST_UNR_TOS:
			ft_printf("Destination Host Unreachable for Type of Service\n");
			break;
		case ICMP_PKT_FILTERED:
			ft_printf("Packet filtered\n");
			break;
		case ICMP_PREC_VIOLATION:
			ft_printf("Precedence Violation\n");
			break;
		case ICMP_PREC_CUTOFF:
			ft_printf("Precedence Cutoff\n");
			break;
		default:
			ft_printf("Dest Unreachable, Bad Code: %d\n", code);
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		ft_printf("Source Quench\n");
		break;
	case ICMP_REDIRECT:
		switch(code) {
		case ICMP_REDIR_NET:
			ft_printf("Redirect Network");
			break;
		case ICMP_REDIR_HOST:
			ft_printf("Redirect Host");
			break;
		case ICMP_REDIR_NETTOS:
			ft_printf("Redirect Type of Service and Network");
			break;
		case ICMP_REDIR_HOSTTOS:
			ft_printf("Redirect Type of Service and Host");
			break;
		default:
			ft_printf("Redirect, Bad Code: %d", code);
			break;
		}
		break;
	default:
	    ft_printf("Unknown ICMP error type %d, code %d\n", type, code);
	}
}

/**
 * Print a round-trip time value in the format "seconds,milliseconds".
 *
 * @param rtt: Pointer to a timeval structure representing the RTT.
 */
static void print_icmp_rtt(const struct timeval *rtt) {
    long msec = rtt->tv_sec * 1000 + rtt->tv_usec / 1000;
    long frac = (rtt->tv_usec % 1000);
    printf("%ld.%03ld", msec, frac);
}

/**
 * Print the content of an errored ICMP packet (IP header + ICMP header + body).
 *
 * @param buf: Buffer containing the errored ICMP body.
 */
static void print_err_icmp_body(uint8_t *buf) {
	struct iphdr *ipb = skip_icmphdr((struct icmphdr *)buf);
	struct icmphdr *icmpb = skip_iphdr(ipb);
	uint8_t *bytes = (uint8_t *)ipb;
	char str[INET_ADDRSTRLEN];

    if (!buf) return;

	ft_printf("IP Hdr Dump:\n");
	for (size_t i = 0; i < sizeof(struct iphdr); i += 2) {
		ft_printf(" %02x%02x", *bytes, *(bytes + 1));
		bytes += 2;
	}
	ft_printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src	"
	       "Dst	Data\n");
	ft_printf(" %x  %x  %02x %04x %04x   %x %04x  %02x  %02x %04x ",
	       ipb->version, ipb->ihl, ipb->tos, ntohs(ipb->tot_len),
	       ntohs(ipb->id), ntohs(ipb->frag_off) >> 13,
	       ntohs(ipb->frag_off) & 0x1FFF, ipb->ttl, ipb->protocol,
	       ntohs(ipb->check));
	inet_ntop(AF_INET, &ipb->saddr, str, sizeof(str));
	ft_printf("%s  ", str);
	inet_ntop(AF_INET, &ipb->daddr, str, sizeof(str));
	ft_printf("%s\n", str);
	ft_printf("ICMP: type %x, code %x, size %zu, id %#04x, seq 0x%04x\n",
	       icmpb->type, icmpb->code, ICMP_BODY_SIZE + sizeof(*icmpb),
	       icmpb->un.echo.id, icmpb->un.echo.sequence);
}

/**
 * Print information about a received ICMP packet.
 *
 * Handles both echo replies and ICMP error packets.
 * Output format depends on verbosity and quiet options.
 *
 * @param buf: Pointer to the buffer containing the received packet (IP + ICMP).
 * @param nb_bytes: Total size of the received buffer.
 * @param opts: Options used by ft_ping.
 * @param pi: Packet statistics, used for RTT and counters.
 *
 * Return: 0 on success, -1 on error.
 */
int print_recv_info(void *buf, ssize_t nb_bytes, const t_options *opts, const t_packinfo *pi) {
    char addr[INET_ADDRSTRLEN] = {};
    struct iphdr *iph = buf;
    struct icmphdr *icmph = skip_iphdr(iph);

    if (!inet_ntop(AF_INET, &iph->saddr, addr, INET_ADDRSTRLEN)) {
        perror("inet_ntop");
        return -1;
    }

    struct timeval now = {0};
    if (opts->timestamp)
        gettimeofday(&now, NULL);

    if (!opts->quiet && icmph->type == ICMP_ECHOREPLY) {
        if (opts->timestamp)
            printf("[%ld.%06ld] ", now.tv_sec, (long)now.tv_usec);
        printf("%ld bytes from %s: ", nb_bytes - IP_HDR_SIZE, addr);
        printf("icmp_seq=%d ttl=%d time=", icmph->un.echo.sequence, iph->ttl);
        print_icmp_rtt(&pi->rtt_last->val);
        printf(" ms\n");
    }

    else if (icmph->type != ICMP_ECHOREPLY) {
        if (opts->timestamp)
            printf("[%ld.%06ld] ", now.tv_sec, (long)now.tv_usec);
        printf("%ld bytes from %s: ", nb_bytes - IP_HDR_SIZE, addr);
        print_icmp_err(icmph->type, icmph->code);
        if (opts->verb)
            print_err_icmp_body((uint8_t *)icmph);
    }

    return 0;
}

/**
 * Calculate the percentage of lost packets.
 *
 * @param pi: Packet statistics structure.
 *
 * Return: Percentage of lost packets as a float.
 */
static float calc_packet_loss(const t_packinfo *pi) {
    if (pi->nb_send == 0) return 0.0f;

    return (1.0 - (float)(pi->nb_ok) / (float)pi->nb_send) * 100.0;
}

/**
 * Print final packet statistics after completing all ICMP requests.
 *
 * Includes packets sent/received, loss %, and RTT stats (min/avg/max/stddev).
 *
 * @param si: Socket information structure.
 * @param pi: Packet statistics structure.
 */
void print_end_info(const t_sockinfo *si, t_packinfo *pi) {
    struct timeval delta;
    timersub(&pi->end_time, &pi->start_time, &delta);
    long elapsed_ms = delta.tv_sec * 1000 + delta.tv_usec / 1000;
	ft_printf("\n--- %s ping statistics ---\n", si->host);
	printf("%d packets transmitted, %d packets received, "
	       "%d%% packet loss, time %ldms\n", pi->nb_send, pi->nb_ok,
	       (int)calc_packet_loss(pi), elapsed_ms);
	if (pi->nb_ok) {
		rtts_calc_stats(pi);
	    printf("round-trip min/avg/max/stddev = ");
	    print_icmp_rtt(pi->min);
	    printf("/");
	    print_icmp_rtt(&pi->avg);
	    printf("/");
	    print_icmp_rtt(pi->max);
	    printf("/");
	    print_icmp_rtt(&pi->stddev);
	    printf(" ms\n");
	}
}