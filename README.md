## MyTimeServer
MTS is a very small and simple time server, that doesn't follow the RFC specification.

###Server
The server was written in C and made to be fast, it's lightweight and can be runned in any computer. 
By default listen on port 1234 and can run in verbose mode and write a log file if necessary.

Your usage:
```
Usage: bin/server [options]
Brief: Performs server to listening on port 1234
Options:
  -l Enables log file
  -v Enables verbose mode
  -h Show this help
```

###Client
In the same way, is very simple, it's just get the current time from the server and [can] change the machine time. 
It also has an interactive mode, if there is a need to getting time "manually".

Your usage:
```
Usage: bin/client <ip-address> [options]
Brief: Performs client to get the current time on 1234 port.
Options:
  -i Iterative mode
  -s Set mode, changes the system time (needs to be root)
  -h Show this help
```

###Build
To build both client and server, just type: ```make```, and voilá.
