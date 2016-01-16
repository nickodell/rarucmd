#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>

char *self;
char cwd[PATH_MAX];
unsigned short random;
id_t id;
int fd;

void usageQuit() {
        printf("Usage: %s <program> [arguments]\n", self);
        puts("Runs <program> as a random UID and GID (31337-96872).");
        printf("Recommended: alias %s='env -i PATH=$PATH %s'\n", self, self);
        _exit(1);
}

int main(int argc, char **argv) {
	self=argv[0];
	if (argc == 1)
		usageQuit();
	fd = open("/dev/urandom", O_RDONLY);
	read(fd, &random, sizeof(random));
	close(fd);

	/* Don't mess with NULL pointers. This is setuid, afterall.     */

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		_exit(40);

	/* Have to check uid last.                                      */
	id = 31337 + random;
	if (setgid(id) != 0 || setuid(id) != 0) {
		puts("Unable to setuid(), aborting.");
		return(41);
	}

	/* If we can't chdir() to the cwd, it means we shouldn't be     */
	/* there in the first place. Fall back to / if that's the       */
	/*  case, for security reasons.                                 */

	if (chdir(cwd) != 0)
		chdir("/");

	/* exec the first argument with path hunting.		 	*/
	execvp(argv[1], argv+1);

	/* If we make it this far, something broke (not in PATH, etc)   */
	printf("%s: error\n", self);
	return(42);
}
