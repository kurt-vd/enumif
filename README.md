## libenumif

libenumif is a C-library which only can be used under Linux.

The main goal is to fetch a list of available network interfaces
present on the system, from a C-program.
The core library function is **enumif()**

*enumif* is also a program that wraps *enumif()* library call.

## enumif vs. if_nameindex
libenumif supersedes if_nameindex().
*if_nameindex()* fetches only IP network interfaces.
*enumif()* fetches all interfaces.
To demonstrate the difference, a sample program if_nameindex has been added.

## cross-compile

Cross-compiling is done by creating **config.mk** in the libenumif root.
config.mk may add, overrule, extend Makefile variables.

