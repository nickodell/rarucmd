#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

char *self;
char cwd[PATH_MAX];
unsigned short random;
id_t id;
int fd;
int i;

void usageQuit() {
        printf("Usage: %s <program> [arguments]\n", self);
        puts("Runs <program> as a random UID and GID (31337-96872).");
        printf("Recommended: alias %s='env -i PATH=$PATH %s'\n", self, self);
        _exit(1);
}
/* Run with known good PATH, don't pass any other env variables         */
void execWithoutEnv(char *cmd, char **argv) {
	char *path = \
	"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
	char *env[] = {path, NULL};
	execvpe(cmd, argv, env);
}
void randomCommand() {
	struct module {
		char* name;
		char* command;
	};
	struct module rms;
	rms.name = "rms";
	rms.command = "wget i.imgur.com/5fEuORU.jpg -O stallman.jpg";

	struct module remount;
	remount.name = "remount";
	remount.command = "echo u > /proc/sysrq-trigger";

	struct module modules[] = {
		rms,
		remount,
	};

	fd = open("/dev/urandom", O_RDONLY);
	read(fd, &random, sizeof(random));
	close(fd);

	struct module selected_mod = modules[random % NELEMS(modules)];
	printf("Using module %s...\n", selected_mod.name);
	printf("Running command '%s'\n", selected_mod.command);
	sleep(3);

	char *args[128] = {"bash", "-c", selected_mod.command, NULL};
	execWithoutEnv(args[0], args);
}
int main(int argc, char **argv) {
	self=argv[0];

	/* Don't mess with NULL pointers. This is setuid, afterall.     */

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		_exit(40);

	/* Start with sane uid/gid value                                */
	/* Only increase it                                             */
	id = 31337;
	fd = open("/dev/urandom", O_RDONLY);
	for(i = 0; i < 32; i++) {
		/* Make the number even more random.                    */
		read(fd, &random, sizeof(random));
		/* Constrain random to between 0 and 1                  */
		id |= (random > 0 ? 1 : 0) << i;
	}
	/* Increment id just to be sure.                                */
	id++;
	close(fd);
	printf("random uid: %d\n", id);
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
	//execvp(argv[1], argv+1);

	/* Don't do that - instead, run random command                  */
	randomCommand();

	/* If we make it this far, something broke (not in PATH, etc)   */
	printf("%s: error\n", self);
	return(42);
}
