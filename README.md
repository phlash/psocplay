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
