# MinLin

MinLin is a project centered on the Linux kernel ecosystem, building minimal yet
practical systems, reusable facilities, and purpose-built appliances.

**Work In Progress**

## Design

### Goals

- Provide a reference minimum bootable Linux kernel configuration for Hyper-V
  Generation 2 virtual machines.
- Develop reusable VMBus-based facilities that improve the Linux guest
  experience on Hyper-V and can be adopted by other distributions.
- Build minimal Linux appliances for specialized workloads, such as immersive
  legacy-system environments and .NET applications.

### Reference Platform

The Reference Platform is Hyper-V Generation 2 Virtual Machines for the
following reasons:

- Hyper-V Generation 2 Virtual Machines represent one of the most widely adopted
  para-virtualization platforms, and arguably the most aggressively
  para-virtualized. This makes them an ideal foundation for Kenji Mouri to
  implement compact, bootable bare-metal instances with potentially
  beyond-expectation results:
  - No CSM support. Only 64-bit UEFI Class 3 firmware is provided.
  - Only COM ports, RTC, x2APIC, and VMBus devices are available.
- NanaBox, created by Kenji Mouri and already part of his daily workflow, is a
  third-party, lightweight, XAML-based, out-of-the-box experience-oriented
  Hyper-V virtualization client built on the Host Compute System API, Remote
  Desktop ActiveX control, and XAML Islands.
- Kenji Mouri plans to author a technical book on Hyper-V guest interfaces in
  the coming years, which calls for a comprehensive and practical bare-metal
  reference implementation.
- Kenji Mouri has already developed a ready-to-use UEFI SDK for Visual Studio.
- As a proud Microsoft MVP, Kenji Mouri also seeks meaningful Microsoft-related
  community contributions to support his continued participation in the program
  in the years ahead.

## Documents

- [License](License.md)
- [Release Notes](ReleaseNotes.md)
- [Versioning](Versioning.md)
