#include "../../inc/ft_ping.h"

static const char supported_opts[] = "h?qvcDitn";

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
    case 'i': opts->interval = 1;
        break;
    case 't': opts->ttl = 1;
        break;
    case 'n': opts->no_dns = 1;
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


/**
 * Handle the '-c' option to specify the number of ping requests.
 *
 * Parses the count value either attached to the option (e.g., `-c5`) or provided as the next argument (e.g., `-c 5`). Stores the value in `opts->count`.
 *
 * @param argc The argument count from main().
 * @param argv The argument vector from main().
 * @param i Pointer to the current index in argv, will be incremented if value is in the next arg.
 * @param opts Pointer to the options structure where the count will be stored.
 *
 * @return 0 on success, -1 on failure (e.g., missing or invalid value).
 */
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
 * Parse the -i option argument and set interval in options.
 *
 * @argc: Argument count.
 * @argv: Argument vector.
 * @index: Pointer to current index in argv, will be incremented if argument consumed.
 * @opts: Pointer to options struct to update.
 *
 * @return: 0 on success, -1 on error (invalid or missing argument).
 */
static int handle_interval_option(int argc, char **argv, int *index, t_options *opts) {
    if (*index + 1 >= argc) {
        ft_printf("ft_ping: option -i requires an argument\n");
        return -1;
    }
    char *arg = argv[++(*index)];
    float val = atoi(arg);
    if (val <= 0.0f) {
        ft_printf("ft_ping: invalid interval '%s'\n", arg);
        return -1;
    }
    opts->interval = val;
    return 0;
}

/**
 * Handle the '-t' option to set the TTL (Time To Live).
 *
 * Extracts the TTL value from the next argument and validates it's within [1, 255].
 * The value is then assigned to `opts->ttl`.
 *
 * @param argc The argument count from main().
 * @param argv The argument vector from main().
 * @param index Pointer to the current index in argv, will be incremented to access TTL value.
 * @param opts Pointer to the options structure where the TTL will be stored.
 *
 * @return 0 on success, -1 on failure (e.g., missing or out-of-range value).
 */
static int handle_ttl_option(int argc, char **argv, int *index, t_options *opts) {
    if (*index + 1 >= argc) {
        ft_printf("ft_ping: option -t requires an argument\n");
        return -1;
    }
    char *arg = argv[++(*index)];
    int val = atoi(arg);
    if (val <= 0 || val > 255) {
        ft_printf("ft_ping: invalid ttl '%s' (must be 1-255)\n", arg);
        return -1;
    }
    opts->ttl = (uint8_t)val;
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
            switch (argv[i][1]) {
            case 'c':
                if (handle_count_option(argc, argv, &i, opts) == -1)
                    return -1;
                break;
            case 'i':
                if (handle_interval_option(argc, argv, &i, opts) == -1)
                    return -1;
                break;
            case 't':
                if (handle_ttl_option(argc, argv, &i, opts) == -1)
                    return -1;
                break;
            default:
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
