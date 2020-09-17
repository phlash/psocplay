## Setting up Debian stable for CY8CKIT-059 development

### You're gonna need a bigger OpenOCD :)

 * Install default openocd package
 * Pull the following from SID (via packages.debian.org/openocd)
     * openocd (0.10.0+g20200819-1)
     * libjim0.79 (0.79-dfsg0-2)
     * libjaylink0 (0.2.0-1)
 * Install the bundle with dpkg -i

Re-plug the devkit to ensure new udev rules are applied

Use the config herin (openocd.cfg)

### Toolchains: arm-none-eabi / gdb-multiarch

Just install 'em :)

Typical usage with gdb...

```
gdb-multiarch
> target extended-remote |openocd -f openocd.cfg -c "gdb_port pipe; log_output /dev/null"
```

### Makefile targets

The usual suspects: `all`, `clean` plus a couple of typing savers: `prog` to program the target
and `debug` to fire up gdb as above with source level debugging ready to roll :)

### Going all GUI

I'm using Visual Studio Code, from the official repo:
```
/etc/apt/sources.list.d/vscode.list:
deb [arch=amd64] https://packages.microsoft.com/repos/vscode buster main
```
Add the Cortex-Debug extension from the Marketplace, as the default gdb support makes poor assumptions..

Load this folder, hit Ctrl-Shift-B and watch `make` do it's magic, or hit F5 and be impressed ;)