// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright © 2007-2018 ANSSI. All Rights Reserved.

/**
 * downloadrequest.c
 *
 * @brief downloadrequest connects to /var/run/download and sends a download request to a clip-download.
 * @see clip_download
 *
 **/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

static int usage (const char *s);

static int usage(const char *s)
{
	fprintf(stderr, "Usage: %s action jail\n\
where action can have the following values :\n\
\tlaunch\tlaunches clip-download for jail\n\
\tlock\tlock clip-download and clip-install for jail\n\
\tunlock\tunlock clip-download and clip-install for jail\n\
and jail should be either clip, rm_h or rm_b\n", s);
	return -1;
}

int main(int argc, const char *argv[])
{
	int s;
	char c;
	struct sockaddr_un sau;
	char command = 0;

	if (argc == 3)
	{
		if (!strcmp (argv[1], "launch") && !strcmp (argv[2], "clip"))
			command = 'A';
		else if (!strcmp (argv[1], "launch") && !strcmp (argv[2], "rm_h"))
			command = 'B';
		else if (!strcmp (argv[1], "launch") && !strcmp (argv[2], "rm_b"))
			command = 'C';
		else if (!strcmp (argv[1], "lock") && !strcmp (argv[2], "clip"))
			command = 'D';
		else if (!strcmp (argv[1], "lock") && !strcmp (argv[2], "rm_h"))
			command = 'E';
		else if (!strcmp (argv[1], "lock") && !strcmp (argv[2], "rm_b"))
			command = 'F';
		else if (!strcmp (argv[1], "unlock") && !strcmp (argv[2], "clip"))
			command = 'G';
		else if (!strcmp (argv[1], "unlock") && !strcmp (argv[2], "rm_h"))
			command = 'H';
		else if (!strcmp (argv[1], "unlock") && !strcmp (argv[2], "rm_b"))
			command = 'I';
		else
			exit(usage(argv[0]));
	}
	else
		exit(usage(argv[0]));

	sau.sun_family = AF_UNIX;
	snprintf(sau.sun_path, sizeof(sau.sun_path), 
		"%s",DOWNLOADSOCKET);

	s = socket(PF_UNIX, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		return -1;
	}
	
	if (connect(s, (struct sockaddr *)&sau, sizeof(struct sockaddr_un)) < 0) {
		perror("connect");
		close(s);
		return -1;
	}

	if (write(s,&command,1) != 1) {
		perror("write");
		close(s);
		return -1;
	}

	if (read(s, &c, 1) != 1) {
		perror("read");
		close(s);
		return -1;
	}

	close(s);
	if (c == 'Y') {
		printf("Action successfully completed\n");
		return EXIT_SUCCESS;
	} else if (c == 'L') {
		printf("Can not proceed (clip update process locked)\n");
		return EXIT_SUCCESS;
	} else {
		printf("Error during the execution\n");
		return EXIT_FAILURE;
	}
}
