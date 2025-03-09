/** cutil/posix/getopt.h
 *
 * Header for command-line option parser heavily inspired by POSIX getopt
 * and GNU getopt_long.
 *
 */

#ifndef CUTIL_POSIX_GETOPT_H_INCLUDED
#define CUTIL_POSIX_GETOPT_H_INCLUDED

/**
 * Communication variable with the caller.
 * If the current option takes an argument, it points to that argument.
 */
extern char *cutil_optarg;

/**
 * Communication variable with the caller.
 * Index in 'argv' currently being processed. 0 on first call.
 */
extern int cutil_optind;

/**
 * Communication variable with the caller.
 * Boolean switch to toggle whether to print error messages or not.
 */
extern int cutil_opterr;

/**
 * Communication variable with the caller.
 * Set to an option character which was unrecognized.
 */
extern int cutil_optopt;

/**
 * This structure describes a single long option name for the sake of
 * 'cutil_getopt_long'. The argument longopts must be an array of these
 * structures, one for each long option. Terminate the array with an element
 * containing all zeros.
 */
typedef struct {
    /**
     * This field is the name of the option. It is a string.
     */
    const char *name;
    /**
     * This field says whether the option takes an argument. It is an integer,
     * and there are two legitimate values: CUTIL_OPTION_NO_ARGUMENT and
     * CUTIL_OPTION_REQUIRED_ARGUMENT defined in the enum below.
     */
    int has_arg;
    /**
     * These fields control how to report or act on the option when it occurs.
     *
     * If flag is a null pointer, then the val is a value which identifies this
     * option. Often these values are chosen to uniquely identify particular
     * long options.
     *
     * If flag is not a null pointer, it should be the address of an int
     * variable which is the flag for this option. The value in val is the value
     * to store in the flag to indicate that the option was seen.
     */
    int *flag;
    int val;
} cutil_Option;

/**
 * Names for the values of the `has_arg` field of `cutil_Option`.
 */
enum {
    CUTIL_OPTION_NO_ARGUMENT,
    CUTIL_OPTION_REQUIRED_ARGUMENT,
};

/**
 * The 'cutil_getopt' function is a command-line parser that shall follow
 * Utility Syntax Guidelines 3, 4, 5, 6, 7, 9, and 10 in the Base Definitions
 * volume of IEEE Std 1003.1-2001, Section 12.2, Utility Syntax Guidelines.
 *
 * @param[in] argc argument count as passed to 'main'
 * @param[in] argv argument array as passed to 'main'
 * @param[in] optstring string of recognized option characters; if a character
 * is followed by a colon, the option takes an argument
 *
 * @return next option character (if one is found) from `argv` that matches a
 * character in `optstring`, if there is one that matches. If not, -1
 */
int
cutil_getopt(int argc, char *const *argv, const char *optstring);

/**
 * Decode options from the vector `argv` (whose length is `argc`). The argument
 * `shortopts` describes the short options to accept, just as it does in
 * 'cutil_getopt'. The argument `longopts` describes the long options to accept
 * (see above).
 *
 * When 'cutil_getopt_long' encounters a short option, it does the same thing
 * that 'cutil_getopt' would do: it returns the character code for the option,
 * and stores the option's argument (if it has one) in `cutil_optarg`.
 *
 * When 'cutil_getopt_long' encounters a long option, it takes actions based on
 * the `flag` and `val` fields of the definition of that option.
 *
 * If flag is a null pointer, then 'cutil_getopt_long' returns the contents of
 * val to indicate which option it found. You should arrange distinct values in
 * the `val` field for options with different meanings, so you can decode these
 * values after 'cutil_getopt_long' returns. If the long option is equivalent to
 * a short option, you can use the short option's character code in val.
 *
 * If flag is not a null pointer, that means this option should just set a
 * flag in the program. The `flag` is a variable of type int that you define.
 * Put the address of the flag in the `flag` field. Put in the `val` field the
 * value you would like this option to store in the flag. In this case,
 * 'cutil_getopt_long' returns 0.
 *
 * For any long option, 'cutil_getopt_long' tells you the index in the array
 * `longopts` of the options definition, by storing it into `*indexptr`. You can
 * get the name of the option with `longopts[*indexptr].name`. So you can
 * distinguish among long options either by the values in their `val` fields or
 * by their indices. You can also distinguish in this way among long options
 * that set flags.
 *
 * When a long option has an argument, 'cutil_getopt_long' puts the argument
 * value in the variable `cutil_optarg` before returning. When the option has no
 * argument, the value in `cutil_optarg` is a null pointer.
 *
 * When 'cutil_getopt_long' has no more options to handle, it returns -1, and
 * leaves in the variable cutil_optind the index in `argv` of the next remaining
 * argument.
 *
 * @param[in] argc argument count as passed to 'main'
 * @param[in] argv argument array as passed to 'main'
 * @param[in] shortopts string of recognized option characters; if a character
 * is followed by a colon, the option takes an argument
 * @param[in] longopts array of long options
 * @param[out] indexptr index in the array longopts
 *
 * @return next option character (if one is found) from `argv` that matches a
 * character in `shortopts`, if there is one that matches. If not, -1
 */
int
cutil_getopt_long(
  int argc,
  char *const *argv,
  const char *shortopts,
  const cutil_Option *longopts,
  int *indexptr
);

#endif /* CUTIL_POSIX_GETOPT_H_INCLUDED */
