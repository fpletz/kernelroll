# Putting rickrolling in kernel space

If you want to hijack open calls for .mp3 files you can just use

sudo apt-get install systemtap
sudo stap -g -e 'probe kernel.function("do_filp_open") { p = kernel_string($pathname); l = strlen(p); if (substr(p, l - 4, l) == ".mp3") { $pathname = %{ (long)"/some/path/rickroll.mp3" %}; } }'
