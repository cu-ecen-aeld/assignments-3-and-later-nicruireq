#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>

#define ARGS_NUMBER 3

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		syslog(LOG_ERR, "There is not enough arguments.");
		return 1;
	}

	// copy arguments
	size_t writefileSize = strlen(argv[1]);
	char writefile[writefileSize+1]; 
	strcpy(writefile, argv[1]);
	size_t writestrSize = strlen(argv[2]);
	char writestr[writestrSize+1];
	strcpy(writestr, argv[2]);

	// Setting syslog
	openlog("writer", LOG_CONS, LOG_USER);

	// File processing
	int writefileDescriptor = open(writefile, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP);

	if (writefileDescriptor == -1)
	{
		syslog(LOG_ERR, "Error opening file: %s", strerror(errno));
		return 1;
	}
	else
	{
		int writtenSize = write(writefileDescriptor, writestr, writestrSize);
		if (writtenSize == -1)
		{
			syslog(LOG_ERR, "Error writting the file: %s", strerror(errno));
			return 1;
		}
		else if (writtenSize != writestrSize)
		{
			syslog(LOG_ERR, "The full string can not be written in the file.");
			return 1;
		}
		else
		{
			syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
		}
	}

	return 0;
}

