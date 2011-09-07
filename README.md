# Putting rickrolling in kernel space

Kernelroll is a linux kernel module for advanced rickrolling. It patches
the open system call to play a specified music file instead of other
music files.

# Installation

You need the address of sys_call_table in your kernel. Use

    $ grep sys_call_table /boot/System.map-3.0.0-1-amd64
    ffffffff81400300 R sys_call_table

on the respective System.map of your kernel to find out the address

Now fire up kernelroll.c and add yours:

    void **sys_call_table = (void **)0xffffffff81400300;

This will probably be simplified in the future, but as sys_call_table isn't
exported anymore in 2.6 kernels, we have to use some tricks.

Compile with:

    $ make

Load with:

    $ insmod kernelroll.ko rollfile=/path/to/rickroll.mp3

Fire up a music player of your choice and consider yourself rickrolled. ;)
