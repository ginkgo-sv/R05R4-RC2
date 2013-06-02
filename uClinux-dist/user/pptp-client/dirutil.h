/* dirutil.h ... directory utilities.
 *               C. Scott Ananian <cananian@alumni.princeton.edu>
 *
 * $Id: dirutil.h,v 1.2 2005/07/25 05:00:24 magicyang Exp $
 */

/* Returned malloc'ed string representing basename */
char *basenamex(char *pathname);
/* Return malloc'ed string representing directory name (no trailing slash) */
char *dirname(char *pathname);
/* In-place modify a string to remove trailing slashes.  Returns arg. */
char *stripslash(char *pathname);
/* ensure dirname exists, creating it if necessary. */
int make_valid_path(char *dirname, mode_t mode);
