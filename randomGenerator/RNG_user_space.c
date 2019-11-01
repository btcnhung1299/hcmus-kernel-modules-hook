#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>  // Required for read function
#include <fcntl.h>   // Required for O_RDONLY

int main()
{
	int fd, randNum;

	printf("Starting Random Number Generator device\n");

	// Open the device with READ ONLY access

	fd = open("/dev/RNGChar", O_RDONLY); 
	if (fd < 0){
		perror("Failed to open the device");
		return errno;
	}

	// Read from RNG device
	printf("Reading from RNG device\n");
	read(fd, &randNum, sizeof(randNum));
	
	printf("Random number generated = %d\n", randNum);
	printf("End of the program\n");
	return 0;       	
}
