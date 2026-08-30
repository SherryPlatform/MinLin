# MinLin.Kernel.Configurations

Reference Linux kernel configurations used by MinLin to build minimal,
practical bootable systems for selected platforms.

## Flavors

- [MinLin.Kernel.HvGen2.Standard](MinLin.Kernel.HvGen2.Standard.config): The
  standard minimal flavor for general-purpose systems.
- [MinLin.Kernel.HvGen2.Minimal](MinLin.Kernel.HvGen2.Minimal.config): A further
  reduced flavor for systems requiring a smaller kernel.
- [MinLin.Kernel.HvGen2.Retrovisor](MinLin.Kernel.HvGen2.Retrovisor.config): A
  specialized flavor that boots into a KVM/QEMU environment for running legacy
  operating systems.

## Usage

Install the build dependencies:

```
sudo apt install --no-install-recommends \
    build-essential \
    flex \
    bison \
    dwarves \
    libssl-dev \
    libelf-dev \
    cpio \
    libncurses-dev \
    bc
```

From the Linux kernel source tree, select a flavor and generate the complete
kernel configuration:

```sh
CONFIG_DIR=/path/to/MinLin/Sources/MinLin.Kernel.Configurations
FLAVOR=Standard # Standard, Minimal, or Retrovisor

make \
    ARCH=x86 \
    KCONFIG_ALLCONFIG="${CONFIG_DIR}/MinLin.Kernel.HvGen2.${FLAVOR}.config" \
    allnoconfig
```

The generated configuration is written to `.config` in the Linux kernel source
tree.

## License

Configuration files authored by MinLin are distributed under the MIT License.
Third-party files, if any, retain their original licenses.

The Linux kernel remains subject to its own licensing terms when built using
these configurations.
