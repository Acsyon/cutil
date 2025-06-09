#include <cutil/posix/getopt.h>

#include <stdlib.h>
#include <string.h>

#include <cutil/io/log.h>
#include <cutil/std/stdio.h>

/**
 * Make _() a no-op to allow for easy potential inclusion of gettext later
 */
#define _(ARG) (ARG)

/**
 * Communication variable with the caller.
 * If the current option takes an argument, it points to that argument.
 */
char *cutil_optarg = NULL;

/**
 * Communication variable with the caller.
 * Index in ARGV currently being processed. 0 on first call.
 */
int cutil_optind = 1;

/**
 * Communication variable with the caller.
 * Boolean switch to toggle whether to print error messages or not.
 */
int cutil_opterr = 1;

/**
 * Communication variable with the caller.
 * Set to an option character which was unrecognized.
 */
int cutil_optopt = '?';

/**
 * The next char to be scanned in the current `argv` element. This is necessary
 * to resume for multiple short options in single element (which is allowed).
 *
 * If it points to '\0' or is NULL, we are done for the current `argv` element
 * and can advance to the next one.
 */
static char *_nextchar;

int
cutil_getopt(int argc, char *const *argv, const char *optstring)
{
    return cutil_getopt_long(argc, argv, optstring, NULL, NULL);
}

int
cutil_getopt_long(
  int argc,
  char *const *argv,
  const char *shortopts,
  const cutil_Option *longopts,
  int *indexptr
)
{
    cutil_optarg = NULL;

    /* argv[0] is program name not an argument */
    if (cutil_optind == 0) {
        cutil_optind = 1;
    }

    /* Analyse new `argv` element */
    if (_nextchar == NULL || *_nextchar == '\0') {
        /* Stop if we encounter `argv` element "--" */
        if (cutil_optind != argc && strcmp(argv[cutil_optind], "--") == 0) {
            return -1;
        }

        /* Stop if we have processed all entries of argv */
        if (cutil_optind == argc) {
            return -1;
        }

        /* Stop for a non-option */
        if (argv[cutil_optind][0] != '-' || argv[cutil_optind][1] == '\0') {
            return -1;
        }

        /* For valid element, skip initial dash(es) */
        _nextchar = &argv[cutil_optind][1];
        if (longopts != NULL && argv[cutil_optind][1] == '-') {
            ++_nextchar;
        }
    }

    /* Check if current element is a long option */
    if (longopts != NULL && argv[cutil_optind][1] == '-') {
        /* Search end of option */
        char *optend = _nextchar;
        while (*optend != '\0' && *optend != '=') {
            ++optend;
        }

        /* Go through all long options */
        int found = 0;
        const cutil_Option *opt;
        int idx;
        for (opt = longopts, idx = 0; opt->name; ++opt, ++idx) {
            if (strncmp(opt->name, _nextchar, optend - _nextchar) == 0) {
                found = 1;
                break;
            }
        }

        /* Process found option */
        if (found != 0) {
            cutil_optind++;
            if (*optend != '\0') {
                if (opt->has_arg != CUTIL_OPTION_NO_ARGUMENT) {
                    cutil_optarg = optend + 1;
                } else {
                    if (cutil_opterr != 0) {
                        cutil_log_warn(
                          _("Option '--%s' doesn't allow an argument\n"),
                          opt->name
                        );

                        cutil_optopt = opt->val;
                        return '?';
                    }
                }
            } else if (opt->has_arg == CUTIL_OPTION_REQUIRED_ARGUMENT) {
                if (cutil_optind < argc) {
                    cutil_optarg = argv[cutil_optind];
                    ++cutil_optind;
                } else {
                    if (cutil_opterr != 0) {
                        cutil_log_warn(
                          _("Option '%s' requires an argument\n"),
                          argv[cutil_optind - 1]
                        );
                    }
                    cutil_optopt = opt->val;
                    return (shortopts[0] == ':') ? ':' : '?';
                }
            }
            _nextchar += strlen(_nextchar);
            if (indexptr != NULL) {
                *indexptr = idx;
            }
            if (opt->flag != NULL) {
                *(opt->flag) = opt->val;
                return 0;
            }
            return opt->val;
        }

        /* Not a valid long option */
        if (cutil_opterr != 0) {
            const char tmp = *optend;
            *optend = '\0';
            cutil_log_warn(_("Unrecognized option '--%s'\n"), _nextchar);
            *optend = tmp;
        }
        _nextchar = NULL;
        ++cutil_optind;
        cutil_optopt = 0;
        return '?';
    }

    /* Handle short option */
    {
        /* Search character in `shortopts` */
        char c = *_nextchar;
        char *tmp = strchr(shortopts, c);
        ++_nextchar;

        /* Increment `cutil_optind` if we arrived at last character of current
         * `argv` element. */
        if (*_nextchar == '\0') {
            ++cutil_optind;
        }

        /* Invalid option */
        if (tmp == NULL || c == ':') {
            if (cutil_opterr != 0) {
                cutil_log_warn(_("Unrecognized option -- '-%c'\n"), c);
            }
            cutil_optopt = c;
            return '?';
        }

        if (tmp[1] == ':') {
            /* This option requires an argument. Therefore, the next
             * character cannot be in the same `argv` element. */
            if (*_nextchar != '\0') {
                cutil_optarg = _nextchar;
                ++cutil_optind;
            } else if (cutil_optind == argc) {
                if (cutil_opterr != 0) {
                    cutil_log_warn(
                      _("Option requires an argument -- '%c'\n"), c
                    );
                }
                cutil_optopt = c;
                c = (shortopts[0] == ':') ? ':' : '?';
            } else {
                /* Increment `cutil_optind` once more, if argument is next
                 * `argv` element */
                cutil_optarg = argv[cutil_optind];
                ++cutil_optind;
            }
            _nextchar = NULL;
        }
        return c;
    }
}
