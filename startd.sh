# list of signals
#https://www.bogotobogo.com/Linux/linux_process_and_signals.php

#Suppose that you have changed your web server's configuration file--you would like to tell the web server to re-read its configuration. You could stop and restart httpd, but this would result in a brief outage period on your web server, which may be undesirable. Most daemons are written to respond to the SIGHUP signal by re-reading their configuration file. So instead of killing and restarting httpd you would send it the SIGHUP signal. Because there is no standard way to respond to these signals, different daemons will have different behavior, so be sure and read the documentation for the daemon in question.
#
#https://stackoverflow.com/questions/14964002/how-can-i-run-rails-server-daemon
#It should be:
#
#rails s -d
#
## to kill the server
#kill `cat tmp/pids/server.pid`
#share  improve this answer  follow 
#answered Feb 19 '13 at 17:46
#
#rubish
#10.7k33 gold badges3939 silver badges5656 bronze badges
#1
#Please use long option/command names, when giving an advice – novices will appreciate it. – tomasbedrich Jan 12 '17 at 10:28
#rails s -d is not working for me is there any other update? – Shiv Aggarwal Mar 30 '18 at 12:01 
#Is there anyway to re-attach if you need to debug the logs? – jahrichie Apr 3 '18 at 21:21
#@jahrichie You can tail the log file tail -f development.log to debug the logs.
