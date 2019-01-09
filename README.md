

# Linux Shell implemented in C

## Install dependencies
For autocomplete and command history:
``` sudo apt install libreadline-dev ```
## Steps to get the shell running
  * `make`
  * `./cshell`

## Directory Structure
```
├── cd.c
├── cd.o
├── clock.c
├── clock.o
├── echo.c
├── echo.o
├── env.c
├── env.o
├── headers.h
├── job.c
├── job.o
├── ls.c
├── ls.o
├── main.c
├── main.o
├── pinfo.c
├── pinfo.o
├── pwd.c
├── pwd.o
├── README.md
├── utility.c
└── utility.o
```

## Features
  + Prompt display
  + echo ( including echo+, echo ~+, echo $({environment variables}))
  + pwd
  + ls (-a, -l)
  + Background processes and notification upon completion of respective process asynchronously
  + pinfo
  + Piping
  + Redirection
  + setenv/unsetenv
  + jobs
  + kjob
  + overkill
  + Custom SIGINT, SIGTSTP, SIGCHLD handlers

## Bonus functionalities
  + Remindme
  + Clock
  + Autocomplete commands
  + Command history

## Additional Notes

  * remindme message is displayed only upon pressing enter and after time interval is over.
  * remindme message have to mentioned in double quotes.
  * For background process, the '&' has to be written separately
  * echo does not handle multi-line input
  * Autocomplete & command history can be implemented if readline library is installed. (sudo apt install libreadline-dev)
  * Type 'exit' to exit the shell
  * Run make clean once done with the program
