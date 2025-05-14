# include "../../inc/loop.h"

/**
 * Calculate the round-trip time (RTT) of a received ICMP packet.
 *
 * @param icmph: Pointer to the received ICMP header.
 * @param new_rtt: Pointer to the RTT node where the result will be stored.
 *
 * The function extracts the timestamp stored in the ICMP body, gets the current
 * time, and computes the delta (current - send) to obtain the RTT.
 *
 * @return: 0 on success, -1 on failure (gettimeofday error).
 */
static int calc_packet_rtt(struct icmphdr *icmph, t_rtt_node *new_rtt)
{
	struct timeval *t_send;
	struct timeval t_recv;

	t_send = ((struct timeval *)skip_icmphdr(icmph));
	if (gettimeofday(&t_recv, NULL) == -1) {
		ft_printf("gettimeofday err: %s\n", strerror(errno));
		return -1;
	}
	timersub(&t_recv, t_send, &new_rtt->val);
	return 0;
}

/**
 * Compute RTT for the received ICMP packet and add it to the end of the RTT list.
 *
 * @param pi: Pointer to the packet info structure containing the RTT list.
 * @param icmph: Pointer to the received ICMP header.
 *
 * @return: Pointer to the newly added RTT node, or NULL on allocation/error.
 */
t_rtt_node * rtts_save_new(t_packinfo *pi, struct icmphdr *icmph) {
	t_rtt_node *elem = pi->rtt_list;
	t_rtt_node *new_rtt = NULL;

	if ((new_rtt = malloc(sizeof(*new_rtt))) == NULL)
		return NULL;
	if (calc_packet_rtt(icmph, new_rtt) == -1)
		return NULL;
	new_rtt->next = NULL;
	if (elem != NULL) {
		while (elem->next)
			elem = elem->next;
		elem->next = new_rtt;
	} else {
		pi->rtt_list = new_rtt;
	}
	pi->rtt_last = new_rtt;
	return new_rtt;
}

/**
 * Free all RTT nodes from the RTT list in the packet info structure.
 *
 * @param pi: Pointer to the packet info structure.
 */
void rtts_clean(t_packinfo *pi) {
	t_rtt_node *elem = pi->rtt_list;
	t_rtt_node *tmp;

	while (elem) {
		tmp = elem;
		elem = elem->next;
		free(tmp);
	}
}

/**
 * Compute the standard deviation of RTT values stored in the packet info structure.
 *
 * @param pi: Pointer to the packet info structure with the RTT list.
 * @param nb_elem: Number of RTT values in the list.
 *
 * Stores result in `pi->stddev`.
 */
void calc_stddev(t_packinfo *pi, long nb_elem) {
	t_rtt_node *elem = pi->rtt_list;
	struct timeval *avg = &pi->avg;
	long sec_dev = 0;
	long usec_dev = 0;
	long total_sec_dev = 0;
	long total_usec_dev = 0;

	while (elem) {
		sec_dev = elem->val.tv_sec - avg->tv_sec;
		sec_dev *= sec_dev;
		total_sec_dev += sec_dev;
		usec_dev = elem->val.tv_usec - avg->tv_usec;
		usec_dev *= usec_dev;
		total_usec_dev += usec_dev;
		elem = elem->next;
	}
	if (nb_elem - 1 > 0) {
		total_sec_dev /= nb_elem - 1;
		total_usec_dev /= nb_elem - 1;
		pi->stddev.tv_sec = (long)sqrt(total_sec_dev);
		pi->stddev.tv_usec = (long)sqrt(total_usec_dev);
	} else {
		pi->stddev.tv_sec = 0;
		pi->stddev.tv_usec = 0;
	}
}

/**
 * Compute RTT statistics from the list: min, max, average, and standard deviation.
 *
 * @param pi: Pointer to the packet info structure.
 *
 * Sets pi->min, pi->max, pi->avg, and pi->stddev fields.
 */
void rtts_calc_stats(t_packinfo *pi) {
	t_rtt_node *elem = pi->rtt_list;
	long nb_elem = 0;
	long total_sec = 0;
	long total_usec = 0;

	pi->min = &elem->val;
	pi->max = &elem->val;
	while (elem) {
		if (timercmp(pi->min, &elem->val, >))
			pi->min = &elem->val;
		else if (timercmp(pi->max, &elem->val, <))
			pi->max = &elem->val;

		total_sec += elem->val.tv_sec;
		total_usec += elem->val.tv_usec;
		if (total_usec > 100000) {
			total_usec -= 100000;
			++total_sec;
		}
		++nb_elem;
		elem = elem->next;
	}
	pi->avg.tv_sec = total_sec / nb_elem;
	pi->avg.tv_usec = total_usec / nb_elem;
	calc_stddev(pi, nb_elem);
}