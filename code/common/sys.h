/*
-- sys.h --

"System callable functions" relating to APIs or the such.
NOTE: This should probably be put in '/sys'. The idea was that the modules
shouldn't include anything from 'sys' so it was put here. However it seems
unecessary so it should probably be moved.
*/

#ifndef SYS_H
#define SYS_H

// Read stdio input from the system.
// Used to input commands
const char *sys_read_in();

#endif
