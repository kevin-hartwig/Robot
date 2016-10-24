/*
*
*	Push.c
*
*	Program to take asynchronus input from a file and 
*	push that information through RS-232 serial communications
*	in the form of a buffer.
*
*
*	Created by James Sonnenberg
*	Oct 23, 2016
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>

// SETTINGS // / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
#define BAUDRATE 		B9600						// Baud rate
#define COMMPORT 		"/dev/ttyUSB0"
//
#define RXPSP			"./IPCFiles/COMMTRANSMIT"	//
#define MAXREAD			255							// Buffer length maximums
#define MSGLEN			6							// Number of SIGIO receives
													// permitted before a serial
													// read is activated
static void initSigHandlers(void);
// SIGIO is a special signal....				//
static
void signal_handler_SIGIO (int status, 		    /*  - - Linked to SIGIO 		*/
						   siginfo_t info);   	/* Currently, the second argument
						   						   to the SIGIO handler does not
						   						   work as expected (to pass info
						   						   on the type of IO that generated
						   						   that signal)  This would be
						   						   awesome if it did.  does not
						   						   affect performance currently,
						   						   as SIGIO is ignored if we are
						   						   transmitting (and, this is the
						   						   only time that it would generate
						   						   an unwanted SIGIO signal)*/

// Local Functions
static int openPort(void);
static int initPort(int fd);

//Threads
void *print_message_function( void *ptr);

//Globals
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;
char LastMessage[20];
char ReadBuf[20];
static unsigned int 	PORTfd;
static struct termios oldtio, newtio;	


main()
{
	printf("\nHello World! Please threads work...\n\n");

	pthread_t thread1, thread2;
	const char *message1;
	int iret1, iret2;

	initSigHandlers();		// Initialize signal handlers 
	PORTfd = openPort();	// Open port, and get file descriptor
	initPort(PORTfd);		// Initialize port settings


	/*Creation of threads	 */

	iret1 = pthread_create(&thread1, NULL, print_message_function, (void*) message1);
	if(iret1)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}

/*	iret2 = pthread_create(&thread2, NULL, print_message_function, (void*) message2);
	if(iret2)
	{
		fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
		exit(EXIT_FAILURE);
	}
*/
	printf("pthread_create() for thread 1 returns %d\n",iret1);
	//printf("pthread_create() for thread 2 returns %d\n",iret2);


		pthread_join(thread1,NULL);

	exit(EXIT_SUCCESS);
}



void *print_message_function(void *ptr)
{
	//pthread_mutex_lock( &mutex1 );
	int bytes = 1, len;	
	

	//VARIABLES FOR READING FROM TEXT FILE
	FILE *fp;
	char ch;


	//READING THE INPUT BUFFER FILE
	for(;;)	
	{	
		fp = fopen("inputs.txt", "r");
		if(!fp)
		{
			printf("Error opening file\n");
			return(0);
		}
		fgets(ReadBuf, sizeof(ReadBuf), fp);
	
		fclose(fp);
		//printf("%s 	-Main\n",ReadBuf);
		//iret1 = pthread_create(&thread1, NULL, print_message_function, (void*) message1);	
		//pthread_join(thread1,NULL);
	

		if(strcmp(ReadBuf, LastMessage) != 0) 
		{
			fflush(stdout);
			printf("%s \n", ReadBuf);
			strcpy(LastMessage, ReadBuf);
			
			len = strlen(ReadBuf);
			bytes = write(PORTfd, ReadBuf, len);
			
		if(bytes != len) {	
				printf("Failed to write entire string?\n");
				fflush(stdout);
			}
			else {
				printf("\n<SENT> %s", ReadBuf);
				fflush(stdout);
			}
		}		
	}
	//pthread_mutex_unlock(&mutex1);
}

static int openPort(void) {

	/* open the device to be non-blocking (read will return immediately) */
	int fd = open(COMMPORT, O_RDWR | O_NOCTTY ); //TOOK OUT NONBLOCK
	if (fd == -1) {
		perror(COMMPORT);
		printf("\nExiting...."); fflush(stdout)	;
		sleep(2);
		exit(0);
	}
	fcntl(fd, F_SETFL, 0);
	/* allow the process to receive SIGIO */
	fcntl(fd, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only
	   O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	int oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags | O_ASYNC );		//TOOK OUT NONBLOCK O_ASYNC

	return(fd);
}

/*/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
 *
 * 	int initPort(int fd)
 *
 * 	Initiailize the port.  BAUDRATE is applied here.  fd of serial pot
 * 	is passed as the argument and should be checked for void before
 * 	being passed.
 *
 *  */
static int initPort(int fd) {
	tcgetattr(fd,&oldtio); /* save current port settings */
	/* set new port settings for canonical input processing */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON | ~ECHO;
	newtio.c_cc[VMIN]=10;
	newtio.c_cc[VTIME]=10;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	return(1);
}

/*/ / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /
 *
 * 	void initSigHandlers(void)
 *
 * 	Initializes all signal handlers and signal routing.
 *
 * 	SIGIO is connected to signal_handler_SIGIO with the SA_SIGINFO flag
 * 	but this does not seem to work in passing additional info to the
 * 	signal handler....
 *
 *  */
static void initSigHandlers(void) {
	struct sigaction saio;           /* definition of signal action */

	/* install the signal handler before making the device asynchronous */
	saio.sa_sigaction = (void*)signal_handler_SIGIO;
//	saio.sa_mask = 0;
	saio.sa_flags = SA_SIGINFO;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);
}

/***************************************************************************
* Sets READ flag, and starts a count of SIGIOs receive, up to MSGLEN chars
*
* 		NOTE: This handler will result in SIGIO signals being ignored
* 		whenever a transmit is in effect. This is not good, but currently
* 		there is no way to differentiate between the SIGIOs generated in
* 		O/P ready vs I/P available.  Documenation states that siginfo_t
* 		argument should pass this information, if sigaction() is called
* 		with the flag SA_SIGINFO, but the if statements below currently
* 		do not trigger. TO DO!!!
***************************************************************************/
static void signal_handler_SIGIO (int status, siginfo_t info)
{
	//printf("Signal Caught!\n");
}
























