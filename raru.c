#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

char *self;
unsigned short random;
id_t id;
int fd;

void usageQuit() {
        printf("Usage: %s <program> [arguments]\n", self);
        puts("Runs <program> as a random UID and GID (31337-31592).");
        _exit(1);
}

int main(int argc, char **argv) {
	self=argv[0];
	if (argc == 1)
		usageQuit();
	fd = open("/dev/urandom", O_RDONLY);
	read(fd, &random, sizeof(random));
	close(fd);
	id = 31337 + random;
	// Have to do uid last.
	if (setgid(id) != 0 || setuid(id) != 0) {
		puts("Unable to setuid(), aborting.");
		return(41);
	}
	chdir("/var/empty");
	execvp(argv[1], argv+1);
	// Something broke (not in PATH, etc)
	printf("%s: error\n", self);
	return(42);
}
