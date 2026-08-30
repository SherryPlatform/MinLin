# MinLin.Kernel.Configurations

Reference Linux kernel configurations used by MinLin to build minimal,
practical bootable systems for selected platforms.

## Usage

- Install the build dependencies:

```
sudo apt install --no-install-recommends \
    build-essential \
    flex \
    bison \
    dwarves \
    libssl-dev \
    libelf-dev \
    cpio \
    libncurses-dev
```

## License

Configuration files authored by MinLin are distributed under the MIT License.
Third-party files, if any, retain their original licenses.

The Linux kernel remains subject to its own licensing terms when built using
these configurations.
