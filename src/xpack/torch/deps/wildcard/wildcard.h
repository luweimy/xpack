/*
 * Wildcard matching engine for use with SFTP-based file transfer
 * programs (PSFTP, new-look PSCP): since SFTP has no notion of
 * getting the remote side to do globbing (and rightly so) we have
 * to do it locally, by retrieving all the filenames in a directory
 * and checking each against the wildcard pattern.
 */

#ifndef wildcard_h
#define wildcard_h

/*
 * Definition of wildcard syntax:
 *
 *  - * matches any sequence of characters, including zero.
 *  - ? matches exactly one character which can be anything.
 *  - [abc] matches exactly one character which is a, b or c.
 *  - [a-f] matches anything from a through f.
 *  - [^a-f] matches anything _except_ a through f.
 *  - [-_] matches - or _; [^-_] matches anything else. (The - is
 *    non-special if it occurs immediately after the opening
 *    bracket or ^.)
 *  - [a^] matches an a or a ^. (The ^ is non-special if it does
 *    _not_ occur immediately after the opening bracket.)
 *  - \*, \?, \[, \], \\ match the single characters *, ?, [, ], \.
 *  - All other characters are non-special and match themselves.
 */

/*
 * Some notes on differences from POSIX globs (IEEE Std 1003.1, 2003 ed.):
 *  - backslashes act as escapes even within [] bracket expressions
 *  - does not support [!...] for non-matching list (POSIX are weird);
 *    NB POSIX allows [^...] as well via "A bracket expression starting
 *    with an unquoted circumflex character produces unspecified
 *    results". If we wanted to allow [!...] we might want to define
 *    [^!] as having its literal meaning (match '^' or '!').
 *  - none of the scary [[:class:]] stuff, etc
 */

/*
 * The wildcard matching technique we use is very simple and
 * potentially O(N^2) in running time, but I don't anticipate it
 * being that bad in reality (particularly since N will be the size
 * of a filename, which isn't all that much). Perhaps one day, once
 * PuTTY has grown a regexp matcher for some other reason, I might
 * come back and reimplement wildcards by translating them into
 * regexps or directly into NFAs; but for the moment, in the
 * absence of any other need for the NFA->DFA translation engine,
 * anything more than the simplest possible wildcard matcher is
 * vast code-size overkill.
 *
 * Essentially, these wildcards are much simpler than regexps in
 * that they consist of a sequence of rigid fragments (? and [...]
 * can never match more or less than one character) separated by
 * asterisks. It is therefore extremely simple to look at a rigid
 * fragment and determine whether or not it begins at a particular
 * point in the test string; so we can search along the string
 * until we find each fragment, then search for the next. As long
 * as we find each fragment in the _first_ place it occurs, there
 * will never be a danger of having to backpedal and try to find it
 * again somewhere else.
 */

/*
 * This is the real wildcard matching routine. It returns 1 for a
 * successful match, 0 for an unsuccessful match, and <0 for a
 * syntax error in the wildcard.
 */
extern int wc_match(const char *wildcard, const char *target);

/*
 * Error reporting is done by returning various negative values
 * from the wildcard routines. Passing any such value to wc_error
 * will give a human-readable message.
 */
extern const char *wc_error(int value);


#endif /* wildcard_h */
