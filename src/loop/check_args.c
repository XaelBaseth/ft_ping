#include "../../inc/ft_ping.h"

static const char supported_opts[] = "h?qvcD";

/**
* Make sure ping is running with admin rights.
*
* Return: 0 on success, -1 on failure.
 */
int check_rights(void) {
    if (getuid() != 0) {
        ft_printf("ft_ping: usage error: must be run as root\n");
        return -1;
    }
    return 0;
}

/**
* Set the appropriate flag in options struct based on given option character.
*
* @opt: The option character.
* @opts: Pointer to the options structure to update.
*
* Return: 0 on success, -1 if the option is invalid.
 */
static int handle_option(char opt, t_options *opts) {
    switch (opt) {
    case 'h': opts->help = 1;
        break;
    case '?' : opts->help = 1;
        break;
    case 'q': opts->quiet = 1;
        break;
    case 'v': opts->verb = 1;
        break;
    case 'c': opts->count = 1;
        break;
    case 'D': opts->timestamp = 1;
        break;
    default:
        ft_printf("ft_ping: invalid option -- '%c'\n", opt);
        return -1;
    }
    return 0;
}

/**
* Parse a single argument containing option flags.
*
* @arg: The argument string to parse.
* @opts: Pointer to the options structure to update.
*
* Return: 0 on success, -1 if an invalid option is encountered.
 */
static int parse_option_arg(const char *arg, t_options *opts) {
    for (size_t i = 1; arg[i]; i++) {
        if (!ft_strchr(supported_opts, arg[i]))
            return handle_option(arg[i], opts);
        if (handle_option(arg[i], opts) == -1)
            return -1;
    }
    return 0;
}

static int handle_count_option(int argc, char **argv, int *i, t_options *opts) {
    const char *arg = argv[*i];

    if (arg[2]) {
        opts->count = ft_atoi(&arg[2]);
    } else {
        if (*i + 1 >= argc || !ft_isdigit(argv[*i + 1][0])) {
            ft_printf("ft_ping: option requires an argument -- 'c'\n");
            return -1;
        }
        *i += 1;
        opts->count = ft_atoi(argv[*i]);
    }
    if (opts->count <= 0) {
        ft_printf("ft_ping: invalid count value\n");
        return -1;
    }
    return 0;
}

/**
* Parse command-line arguments to extract options and the target host.
*
* @argc: Argument count.
* @argv: Argument vector.
* @host: Output pointer to store the target hostname or IP address.
* @opts: Pointer to the options structure to populate with flags.
*
* Return: 0 on success, 1 if help was requested, -1 on error (e.g. missing/too many hosts or bad option).
 */
int parse_args(int argc, char **argv, char **host, t_options *opts) {
    int host_count = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1]) {
            if (argv[i][1] == 'c') {
                if (handle_count_option(argc, argv, &i, opts) == -1)
                    return -1;
            } else {
                if (parse_option_arg(argv[i], opts) == -1)
                    return -1;
            }
        } else {
            *host = argv[i];
            host_count++;
        }
    }

    if (opts->help) {
        print_help();
        return 1;
    }
    if (host_count == 0) {
        ft_printf("ft_ping: missing host operand\n");
        return -1;
    } else if (host_count > 1) {
        ft_printf("ft_ping: only one host is allowed\n");
        return -1;
    }
    return 0;
}
