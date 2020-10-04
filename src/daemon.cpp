#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <string>

//I've also put in the correct include statements for interfacing with the syslog, which is recommended at the very least for sending start/stop/pause/die log statements, in addition to using your own logs with the fopen()/fwrite()/fclose() function calls.

//From here, you can use this skeleton to write your own daemons. Be sure to add in your own logging (or use the syslog facility), and code defensively, code defensively, code defensively!

const char* DAEMON_NAME = "heda-server-daemon";

// Source 1:
// http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html
// Source 2:
// https://gist.github.com/alexdlaird/3100f8c7c96871c5b94e
int main(void) {
  
  /* Our process ID and Session ID */
  pid_t pid, sid;
  
  /* Fork off the parent process */
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then
     we can exit the parent process. */
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  /* Change the file mode mask */
  umask(0);
          
  // Open system logs for the child process
  openlog(DAEMON_NAME, LOG_NOWAIT | LOG_PID, LOG_USER);
  syslog(LOG_NOTICE, "Successfully started daemon-name");         
 
  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0) {
    /* Log the failure */
    syslog(LOG_ERR, "Could not generate session ID for child process");
    exit(EXIT_FAILURE);
  }
  
  /* Change the current working directory */
  if ((chdir("/")) < 0) {
    /* Log the failure */
    syslog(LOG_ERR, "Could not change working directory to /");
    exit(EXIT_FAILURE);
  }
  
  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  
  /* Daemon-specific initialization goes here */
  
  /* The Big Loop */
  while (1) {
     /* Do some task here ... */
     
     sleep(30); /* wait 30 seconds */
  }

  // Close system logs for the child process
  std::string msg = "Stopping "; msg += DAEMON_NAME;
  syslog(LOG_NOTICE, msg.c_str());
  closelog();

  exit(EXIT_SUCCESS);
}
