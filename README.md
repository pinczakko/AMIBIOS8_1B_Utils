# README

## Building The AMIBIOS8 1B Utilities
 
### Prerequisites
 
 - Linux/Unix machine (or possibly Mingw in Windows) to compile the code. The current code is only tested in Arch linux. 

 - CMake, you need CMake to generate the Makefile(s). 

 - C Compiler toolchain. CMake will create Makefile(s) which will compile the code with your C compiler.

 - (Optional) If you want to cross compile from Linux to Windows (64-bit), you need to have the mingw-w64 cross compiler toolchain. 

### Producing Executables

The source code produces TWO different AMIBIOS 1B module utilities: ```ami_1b_splitter``` and ```ami_1b_combiner```.

 - Invoke ```build.sh``` script in Linux/Unix to build the executable for Unix/Linux.

 - Invoke ```cross_build.sh``` script in Linux to cross compile with mingw-w64 to produce 64-bit executable for Windows (64-bit).

## AMIBIOS Binary Support

The utilities produced by the source code ONLY work with AMIBIOS8 (legacy BIOS) 1B module. 
You can obtain the 1B module from AMIBIOS8 BIOS binary by using AMI Module Management Tool (MMTool) utility (https://ami.com/en/products/bios-uefi-tools-and-utilities/bios-uefi-utilities/).

The current AMI BIOS splitter/combiner utilities 
only support 1B modules from "AMIBIOS8" variants, i.e. AMIBIOS binaries which are built based on AMIBIOS8 code base. 

If your BIOS are from 2004 upwards, chances are it's based on the AMIBIOS8 code base.


Now, into the condensed user manual..

## Using ami_1b_splitter 

This is the shell/command showing ami_1b_splitter in action:

	C:\Projects\custom_tool\ami_1b_splitter.exe                                        
	Usage:
	C:\Projects\custom_tool\ami_1b_splitter.exe --extract-all 1B_filename 
	C:\Projects\custom_tool\ami_1b_splitter.exe --extract   1B_filename  component_offset
	C:\Projects\custom_tool\ami_1b_splitter.exe --list      1B_filename

In the first variant, this program will extract all components into individual files. 

In the second variant, this program will extract only ONE component which starts at component_offset in the 1B_file

In the third variant, this program only lists the components inside the 1B file along with their information.

_For example, the steps to extract the ACPI table are as follows:_

### List The 1B Module Components

The _first_ step is to list the components of the 1B module, so you can find the offset of the ACPI table. This is how to do that in shell/command prompt:

	C:\Projects\custom_tool\ami_1b_splitter.exe --list 1B.bin
	Name of 1B file: 1B.bin
	Size of 1B header: 0x3C7 bytes
	Number of components (including the non-present): 0x29
	Calculated 1B file size: 0x62FAD
	1B file size (from fstat): 0x62FAD
	Component string exist 
	1B component: Target physical address: 0xF0000, Name: RUN_CSEG, Present in 1B, File offset: 0x3C7, Size: 0x10000
	1B component: Target physical address: 0x40000, Name: POST_CSEG, Present in 1B, File offset: 0x103C7, Size: 0xD092
	1B component: Target physical address: 0x13CB0, Name: DIM_CSEG, Present in 1B, File offset: 0x1D459, Size: 0x950E
	1B component: Target physical address: 0xEBB20, Name: I13R_CSEG, Present in 1B, File offset: 0x26967, Size: 0x44DA
	1B component: Target physical address: 0x24BD0, Name: I13P_CSEG, Present in 1B, File offset: 0x2AE41, Size: 0x56DC
	1B component: Target physical address: 0xEADE0, Name: FDR_CSEG, Present in 1B, File offset: 0x3051D, Size: 0x6
	1B component: Target physical address: 0x4D0A0, Name: SETSVR_CSEG, Present in 1B, File offset: 0x30523, Size: 0x9EB2
	1B component: Target physical address: 0xEA700, Name: I19R_CSEG2, Present in 1B, File offset: 0x3A3D5, Size: 0x6D8
	1B component: Target physical address: 0xEBB10, Name: INT10_CSEG, Present in 1B, File offset: 0x3AAAD, Size: 0xD
	1B component: Target physical address: 0xEADF0, Name: AFDR_CSEG, Present in 1B, File offset: 0x3AABA, Size: 0xD1A
	1B component: Target physical address: 0x13520, Name: INTERFACE_CSEG, File offset: 0x0, Size: 0x78B
	1B component: Target physical address: 0x2A2B0, Name: PMM_CSEG, Present in 1B, File offset: 0x3B7D4, Size: 0x52A
	1B component: Target physical address: 0xADA90, Name: SMI_BSPCSEG, Present in 1B, File offset: 0x3BCFE, Size: 0xB73D
	1B component: Target physical address: 0x2EAD0, Name: SMBIOS_CSEG, Present in 1B, File offset: 0x4743B, Size: 0xE2D
	1B component: Target physical address: 0xA0000, Name: SMI_BSPENTRYSEG, Present in 1B, File offset: 0x48268, Size: 0x140
	1B component: Target physical address: 0x56F60, Name: AGESA_CSEG, Present in 1B, File offset: 0x483A8, Size: 0x3EFE
	1B component: Target physical address: 0xB91D0, Name: SMI_BSPCSEG_32, Present in 1B, File offset: 0x4C2A6, Size: 0x2C1
	1B component: Target physical address: 0x5AE60, Name: ACPITBL_SEG, Present in 1B, File offset: 0x4C567, Size: 0x5A06
	1B component: Target physical address: 0x2E4D0, Name: SBMISC_CSEG, Present in 1B, File offset: 0x51F6D, Size: 0x5F7
	1B component: Target physical address: 0xA0300, Name: SMI_AP1ENTRYSEG, Present in 1B, File offset: 0x52564, Size: 0x8
	1B component: Target physical address: 0xA0600, Name: SMI_AP2ENTRYSEG, Present in 1B, File offset: 0x5256C, Size: 0x8
	1B component: Target physical address: 0xA0900, Name: SMI_AP3ENTRYSEG, Present in 1B, File offset: 0x52574, Size: 0x8
	1B component: Target physical address: 0x400, Name: BDA_DSEG, File offset: 0x0, Size: 0x110
	1B component: Target physical address: 0x510, Name: BBLK_DSEG, File offset: 0x0, Size: 0x13
	1B component: Target physical address: 0x1D1C0, Name: DIM_DSEG, Present in 1B, File offset: 0x5257C, Size: 0x7A0B
	1B component: Target physical address: 0x0, Name: IVT_DSEG, File offset: 0x0, Size: 0x400
	1B component: Target physical address: 0x2A7E0, Name: PMM_DSEG, Present in 1B, File offset: 0x59F87, Size: 0x101F
	1B component: Target physical address: 0x2B800, Name: POST_DSEG, Present in 1B, File offset: 0x5AFA6, Size: 0x20F8
	1B component: Target physical address: 0x30000, Name: TEMP_DSEG, File offset: 0x0, Size: 0x10000
	1B component: Target physical address: 0x4530, Name: USEG, File offset: 0x0, Size: 0xEFF0
	1B component: Target physical address: 0xA8900, Name: SMI_BSPDSEG, Present in 1B, File offset: 0x5D09E, Size: 0x5183
	1B component: Target physical address: 0xB94A0, Name: SMI_STRUCSEG, Present in 1B, File offset: 0x62221, Size: 0x1C8
	1B component: Target physical address: 0xA7E00, Name: SMI_BSPDUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0xE6000, Name: USB_DATA, File offset: 0x0, Size: 0x3000
	1B component: Target physical address: 0x2D900, Name: AGESA_DSEG, Present in 1B, File offset: 0x623E9, Size: 0xBC4
	1B component: Target physical address: 0xA8100, Name: SMI_AP1DUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0xA8400, Name: SMI_AP2DUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0xA8700, Name: SMI_AP3DUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0x530, Name: STACK_SEG, File offset: 0x0, Size: 0x4000
	1B component: Target physical address: 0xA7600, Name: SMI_BSPSTACKSEG, File offset: 0x0, Size: 0x800
	1B component: Target physical address: 0xC0000, Name: SHADOWREGIONS, File offset: 0x0, Size: 0x20000

### Extract The "Target" 1B Module Component 

The _second_ (last) step is to extract the 1B module component that you want. From the list of the componentsin the example above, you find that the offset of the ACPI table is 0x4C567 (offset here means file offset of the component). Now, you can extract the component with the following command:

	C:\Projects\custom_tool\ami_1b_splitter.exe --extract 1B.bin 0x4C567 
	Writing component file: ACPITBL_SEG.bin , size: 0x5A06

Now you can edit the ACPITBL_SEG.bin as needed.

## Using ami_1b_combiner 

This is the shell/command prompt showing ami_1b_combiner in action:

	C:\Projects\custom_tool\wine ami_1b_combiner.exe 
	Usage:
	C:\Projects\custom_tool\ami_1b_combiner.exe --insert  1B_filename  component_filename  component_offset 
	C:\Projects\custom_tool\ami_1b_combiner.exe --list   1B_filename 

In the _first_ variant, this program will combine the component named component_filename
to the 1B file starting at offset component offset. The program checks 
the inserted component size and start offset, if the program found either of them
is incorrect, it will bail out with error message.

In the _second_ variant, this program only lists the components inside the 1B file.

_For example, the steps to insert the modified ACPI Table that you extract previously are as follows:_

### List The 1B Module Components

The _first_ step is to list the 1B module components, so you can find the offset of the ACPI table as follows:

	C:\Projects\custom_tool\ami_1b_combiner.exe --list 1B.bin           
	Name of 1B file: 1B.bin
	Size of 1B header: 0x3C7 bytes
	Number of components (including the non-present): 0x29
	Calculated 1B file size: 0x62FAD
	1B file size (from fstat): 0x62FAD
	Component string exist 
	1B component: Target physical address: 0xF0000, Name: RUN_CSEG, Present in 1B, File offset: 0x3C7, Size: 0x10000
	1B component: Target physical address: 0x40000, Name: POST_CSEG, Present in 1B, File offset: 0x103C7, Size: 0xD092
	1B component: Target physical address: 0x13CB0, Name: DIM_CSEG, Present in 1B, File offset: 0x1D459, Size: 0x950E
	1B component: Target physical address: 0xEBB20, Name: I13R_CSEG, Present in 1B, File offset: 0x26967, Size: 0x44DA
	1B component: Target physical address: 0x24BD0, Name: I13P_CSEG, Present in 1B, File offset: 0x2AE41, Size: 0x56DC
	1B component: Target physical address: 0xEADE0, Name: FDR_CSEG, Present in 1B, File offset: 0x3051D, Size: 0x6
	1B component: Target physical address: 0x4D0A0, Name: SETSVR_CSEG, Present in 1B, File offset: 0x30523, Size: 0x9EB2
	1B component: Target physical address: 0xEA700, Name: I19R_CSEG2, Present in 1B, File offset: 0x3A3D5, Size: 0x6D8
	1B component: Target physical address: 0xEBB10, Name: INT10_CSEG, Present in 1B, File offset: 0x3AAAD, Size: 0xD
	1B component: Target physical address: 0xEADF0, Name: AFDR_CSEG, Present in 1B, File offset: 0x3AABA, Size: 0xD1A
	1B component: Target physical address: 0x13520, Name: INTERFACE_CSEG, File offset: 0x0, Size: 0x78B
	1B component: Target physical address: 0x2A2B0, Name: PMM_CSEG, Present in 1B, File offset: 0x3B7D4, Size: 0x52A
	1B component: Target physical address: 0xADA90, Name: SMI_BSPCSEG, Present in 1B, File offset: 0x3BCFE, Size: 0xB73D
	1B component: Target physical address: 0x2EAD0, Name: SMBIOS_CSEG, Present in 1B, File offset: 0x4743B, Size: 0xE2D
	1B component: Target physical address: 0xA0000, Name: SMI_BSPENTRYSEG, Present in 1B, File offset: 0x48268, Size: 0x140
	1B component: Target physical address: 0x56F60, Name: AGESA_CSEG, Present in 1B, File offset: 0x483A8, Size: 0x3EFE
	1B component: Target physical address: 0xB91D0, Name: SMI_BSPCSEG_32, Present in 1B, File offset: 0x4C2A6, Size: 0x2C1
	1B component: Target physical address: 0x5AE60, Name: ACPITBL_SEG, Present in 1B, File offset: 0x4C567, Size: 0x5A06
	1B component: Target physical address: 0x2E4D0, Name: SBMISC_CSEG, Present in 1B, File offset: 0x51F6D, Size: 0x5F7
	1B component: Target physical address: 0xA0300, Name: SMI_AP1ENTRYSEG, Present in 1B, File offset: 0x52564, Size: 0x8
	1B component: Target physical address: 0xA0600, Name: SMI_AP2ENTRYSEG, Present in 1B, File offset: 0x5256C, Size: 0x8
	1B component: Target physical address: 0xA0900, Name: SMI_AP3ENTRYSEG, Present in 1B, File offset: 0x52574, Size: 0x8
	1B component: Target physical address: 0x400, Name: BDA_DSEG, File offset: 0x0, Size: 0x110
	1B component: Target physical address: 0x510, Name: BBLK_DSEG, File offset: 0x0, Size: 0x13
	1B component: Target physical address: 0x1D1C0, Name: DIM_DSEG, Present in 1B, File offset: 0x5257C, Size: 0x7A0B
	1B component: Target physical address: 0x0, Name: IVT_DSEG, File offset: 0x0, Size: 0x400
	1B component: Target physical address: 0x2A7E0, Name: PMM_DSEG, Present in 1B, File offset: 0x59F87, Size: 0x101F
	1B component: Target physical address: 0x2B800, Name: POST_DSEG, Present in 1B, File offset: 0x5AFA6, Size: 0x20F8
	1B component: Target physical address: 0x30000, Name: TEMP_DSEG, File offset: 0x0, Size: 0x10000
	1B component: Target physical address: 0x4530, Name: USEG, File offset: 0x0, Size: 0xEFF0
	1B component: Target physical address: 0xA8900, Name: SMI_BSPDSEG, Present in 1B, File offset: 0x5D09E, Size: 0x5183
	1B component: Target physical address: 0xB94A0, Name: SMI_STRUCSEG, Present in 1B, File offset: 0x62221, Size: 0x1C8
	1B component: Target physical address: 0xA7E00, Name: SMI_BSPDUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0xE6000, Name: USB_DATA, File offset: 0x0, Size: 0x3000
	1B component: Target physical address: 0x2D900, Name: AGESA_DSEG, Present in 1B, File offset: 0x623E9, Size: 0xBC4
	1B component: Target physical address: 0xA8100, Name: SMI_AP1DUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0xA8400, Name: SMI_AP2DUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0xA8700, Name: SMI_AP3DUMPSEG, File offset: 0x0, Size: 0x200
	1B component: Target physical address: 0x530, Name: STACK_SEG, File offset: 0x0, Size: 0x4000
	1B component: Target physical address: 0xA7600, Name: SMI_BSPSTACKSEG, File offset: 0x0, Size: 0x800
	1B component: Target physical address: 0xC0000, Name: SHADOWREGIONS, File offset: 0x0, Size: 0x20000

### Insert The 1B Module Component

The _second_ (last) step is to insert the component into the 1B module. From the list of 1B module componentsabove, you find that the offset of the ACPI table is 0x4C567 (offset here means file offset of the component). Now, insert the component: 

	C:\Projects\custom_tool\ami_1b_combiner.exe --insert 1B.bin ACPITBL_SEG.bin 0x4C567
	Successfully writing modified 1B file

That's it the modified ACPI Table is now in the 1B module. 

## Advanced usage

 - You can use Windows batch file if you are working with the same component in the 1B file over and over in Windows 

 - You can use shell scripts to automate the process of extraction and insertion without the need 
to list the offset of the componentIf you compiled the utilities yourself in Linux or other variants of Unix.

## BIG FAT NOTE

The Windows port of the utilities were only tested with Wine. Therefore, I don't know whether they will behave the same way in a real Windows machine. Let me know.

## LICENSE

The source code is licensed under GPL v2 (https://www.gnu.org/licenses/gpl-2.0.html). 

