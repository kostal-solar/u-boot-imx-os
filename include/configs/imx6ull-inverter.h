/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Board configuration file for Kostal KIE imx6ULL
 * 
 * Copyright (C) 2024 Kostal Industrie Elektrik GmbH & Co. KG
 * Copyright (C) 2024 Henry Hermanns <h.hermanns@kostal.com>
 *
 * Copyright (C) 2019 Parthiban Nallathambi <pn@denx.de>
 */
#ifndef __KIE_ULL_H
#define __KIE_ULL_H

#include <linux/sizes.h>
#include "mx6_common.h"

/* SPL options */
#include "imx6_spl.h"

#define CONFIG_SYS_FSL_USDHC_NUM	1

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * SZ_1M)

/* Environment in mmcblk0boot2 */
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_SYS_MMC_ENV_PART		2
#define MMC_ROOTFS_DEV		1
#define MMC_ROOTFS_PART		1

/* Console configs */
#define CONFIG_MXC_UART_BASE		UART1_BASE

/* MMC Configs */

#define CONFIG_SYS_FSL_ESDHC_ADDR	USDHC2_BASE_ADDR

/* I2C configs */
#ifdef CONFIG_CMD_I2C
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_SPEED		100000
#endif

/* Miscellaneous configurable options */
#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x10000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ			1000

/* Physical Memory Map */
#define PHYS_SDRAM			MMDC0_ARB_BASE_ADDR
#define PHYS_SDRAM_SIZE			SZ_512M

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* USB Configs */
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET
#define CONFIG_MXC_USB_PORTSC		(PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS		0
#define CONFIG_USB_MAX_CONTROLLER_COUNT	1

#define CONFIG_IMX_THERMAL

#define CONFIG_FEC_ENET_DEV		1

/* environment organization */

#define ENV_MMC \
	"addcon=setenv bootargs ${bootargs} console=${console},${baudrate} ${bootinfo}\0" \
	"addip=setenv bootargs ${bootargs} ip=dhcp\0" \
	"addstaticip=setenv bootargs ${bootargs} ip=192.168.0.20::192.168.0.1:255.255.255.0:scb-factory:lan1\0" \
	"altbootcmd=if test -e ${mmcpart};then if itest ${rootpart} -eq 3; " \
		"then setenv rootpart 4; setenv mmcfitpart 2; " \
		"else setenv rootpart 3; setenv mmcfitpart 1; fi; " \
		"else setenv rootpart 3; setenv mmcfitpart 1; fi; " \
		"setenv bootcount 0; saveenv; boot\0" \
	"altboot=if itest ${bootcount} -gt ${bootlimitrescue}; " \
		"then run swupdate; else run bootcmdaltmmc; fi;\0"\
	"bootcmdaltmmc=if test ${mmxpart} -eq 1; then setenv mmxpart 2; " \
			"else setenv mmcpart 1; fi;run mmc_mmc_fit\0" \
	"hab_check_img=" \
		"if hab_auth_img ${loadaddr} ${filesize}; then " \
			"echo 'HAB auth OK, booting!'; " \
		"else " \
			"echo 'HAB authentication FAILED'; reset; " \
		"fi\0" \
	"bootlimitrescue=6\0" \
	"mmcdev=" __stringify(MMC_ROOTFS_DEV) "\0" \
	"mmcpart=" __stringify(MMC_ROOTFS_PART) "\0" \
	"bootdelay=0\0" \
	"silent=1\0" \
	"optargs=rw rootwait quiet splash plymouth.ignore-serial-consoles\0" \
	"mmcautodetect=yes\0" \
	"mmcrootfstype=ext3\0" \
	"rootpart=3\0" \
	"mmcfit_name=fitImage\0" \
	"mmcfitpart=1\0" \
	"mmcloadfit=echo Reading fit image from 0:${mmcfitpart};" \
		"load mmc 0:${mmcfitpart} ${fit_addr} ${mmcfit_name}\0" \
	"mmcargs=setenv bootargs " \
		"root=/dev/mmcblk${mmcdev}p${rootpart} ${optargs} " \
		"rootfstype=${mmcrootfstype}\0" \
	"mmc_mmc_fit=run mmcloadfit; run hab_check_img;" \
		"run mmcargs addcon; bootm ${loadaddr}#conf-imx${soc_type}-kie-inverter.dtb\0" \
	"swupdate_args=setenv bootargs root=/dev/ram splash plymouth.ignore-serial-consoles\0"	\
	"swupdate=mmc dev 0 1;mmc read ${fit_addr} 0 8000;" \
		"run swupdate_args addcon;bootm ${fit_addr}#conf-imx${soc_type}-kie-inverter.dtb\0" \
	"swupdate_usb=if usb start && usb storage;then " \
		"load usb 0:1 ${loadaddr} ${swupdate_usb_name};" \
		"run hab_check_img;" \
		"run swupdate_args addcon; bootm ${loadaddr}#conf-imx${soc_type}-kie-inverter.dtb;" \
	"fi\0" \
	"swupdate_tftp=if dhcp ${tftp_rootpath}/${swupdate_usb_name};then " \
		"run hab_check_img; run swupdate_args addcon addstaticip; bootm ${loadaddr}#conf-imx${soc_type}-kie-inverter.dtb;" \
	"fi\0" \
	"tftp_rootpath=kostal\0" \
	"swupdate_usb_name=rescue-initramfs.itb.signed\0" \
	"kernel_file=kostal/fitImage\0" \
	"download_kernel=" \
		"dhcp ${loadaddr} ${kernel_file};\0" \
	"nfsargs=setenv bootargs " \
	"root=/dev/nfs rw "	  \
	"nfsroot=${serverip}:${rootpath},nolock,nfsvers=3" \
	"\0" \
	"rootpath=/srv/tftp/kostal/rootfs\0" \
	"boot_tftp=" \
	"if run download_kernel; then "	  \
	     "setenv bootargs ${bootargs} console=${console} "	  \
	     "root=/dev/sda5 rootwait rootfstype=ext4;" \
	     "bootm ${loadaddr}#conf-imx${soc_type}-kie-inverter.dtb;reset;" \
	"fi\0" \
	"boot_nfs=" \
	"if run download_kernel; then "	  \
	     "run nfsargs;"	  \
	     "run addip;"	  \
	     "setenv bootargs ${bootargs} console=${console};"	  \
	     "bootm ${loadaddr}#conf-imx${soc_type}-kie-inverter.dtb;reset;" \
	"fi\0" \
	"ledon=led red on; led green on; sleep 0.1\0" \
	"ledoff=led red off; led green off\0" \
	"kbd_read=" \
		"setenv kbdres 0;" \
		"setenv k;" \
		"setenv v 1;" \
		"for k in 0 1 2 3 4 5; do " \
			"if gpio input kbd${k}; then " \
				"setexpr kbdres ${kbdres} + ${v};" \
			"fi;" \
			"setexpr v ${v} * 2;" \
		"done;" \
		"echo Recognized keys: ${kbdres}\0" \
	"start_swupdate_tftp=" \
		"run dynamicip;" \
		"if run check_secfuse; then " \
			"echo secfuse unfused.; " \
			"run factory1; " \
		"else echo secfuse enabled; " \
			"setenv bootinfo boot_type=factory;" \
			"setenv swupdate_usb_name factory-image-fit.itb.signed;" \
			"run swupdate_tftp; " \
		"fi\0" \
	"start_swupdate_usb=" \
		"setenv bootinfo boot_type=swu boot_keys=${kbdres};" \
		"run swupdate_usb\0" \
	"boot_switch=" \
		"if itest ${kbdres} == 24; then " \
			"echo booting from tftp...;" \
			"run start_swupdate_tftp;" \
		"fi;" \
		"if itest ${kbdres} == 21; then "	  \
			"echo booting from usb...;" \
			"run start_swupdate_usb;" \
		"fi;" \
		"if itest ${kbdres} == 18; then "	  \
			"echo booting from usb...;" \
			"run start_swupdate_usb;" \
		"fi;" \
		"if itest ${kbdres} == 6; then "	  \
			"echo booting from usb...;" \
			"run start_swupdate_usb;" \
		"fi\0" \
	"net_update_uboot=" \
	   "if dhcp kostal/SPL-emmc.signed; then " \
	       "setexpr fw_sz ${filesize} / 0x200; " \
	       "setexpr fw_sz ${fw_sz} + 1; "  \
	       "mmc dev 0 1; " \
	       "mmc write ${loadaddr} 2 ${fw_sz};" \
	       "mmc dev 0 0; " \
	   "fi;" \
	   "if tftpboot kostal/u-boot-ivt.img-emmc.signed; then " \
	       "setexpr fw_sz ${filesize} / 0x200; " \
	       "setexpr fw_sz ${fw_sz} + 1; "  \
	       "mmc dev 0 1; " \
	       "mmc write ${loadaddr} 8a ${fw_sz}; " \
	       "mmc dev 0 0; " \
	   "fi\0" \
	"update_uboot=usb start; " \
	    "if usb storage; then " \
		"load usb 0:1 ${loadaddr} SPL-emmc.signed; " \
		"setexpr fw_sz ${filesize} / 0x200; " \
		"setexpr fw_sz ${fw_sz} + 1; " \
		"mmc dev 0 1; " \
		"mmc write ${loadaddr} 2 ${fw_sz}; " \
		"load usb 0:1 ${loadaddr} u-boot-ivt.img-emmc.signed; " \
		"setexpr fw_sz ${filesize} / 0x200; " \
		"setexpr fw_sz ${fw_sz} + 1; "  \
		"mmc write ${loadaddr} 8a ${fw_sz}; " \
		"mmc dev 0 0; " \
	    "fi\0" \
	"check_secfuse=if fuse cmp 0 6 0x0; then true; else if fuse cmp 0 6 0x80040; then true; else false; fi; fi;\0" \
	"watchdogr=gpio set GPIO1_05; gpio clear GPIO1_05;\0" \
	"put_logfile=run watchdogr; tftpput ${logaddr} ${logsz} 192.168.0.1:factorylog1.txt;\0" \
	"staticip= setenv serverip 192.168.0.1; " \
		"setenv ipaddr 192.168.0.20; " \
		"setenv netmask 255.255.255.0; \0" \
	"dynamicip= dhcp;" \
		"setenv serverip 192.168.0.1; \0" \
	"logaddr=0x83400000\0" \
	"logtxt01=mw.l ${logaddrptr} 0x0a206f67; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"setexpr logsz ${logsz} + 0x04; " \
		"run put_logfile;\0" \
	"logtxt02=mw.l ${logaddrptr} 0x20746f67; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x656c6966; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x0a202020; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"setexpr logsz ${logsz} + 0x0c;" \
		"run put_logfile;\0" \
	"logtxt03=mw.l ${logaddrptr} 0x20676973; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x656e6f64; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x0a202020; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"setexpr logsz ${logsz} + 0x0c; " \
		"run put_logfile;\0" \
	"logtxt04= " \
		"mw.l ${logaddrptr} 0x20636573; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x656e6f64; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x0a202020; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"setexpr logsz ${logsz} + 0x0c; " \
		"run put_logfile;\0" \
	"logtxt05= " \
		"mw.l ${logaddrptr} 0x7364656c; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x69616620; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x0a64656c; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"setexpr logsz ${logsz} + 0x0c; " \
		"run put_logfile;\0" \
	"logtxt06= " \
		"mw.l ${logaddrptr} 0x7364656c; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"mw.l ${logaddrptr} 0x0a6b6f20; " \
		"setexpr logaddrptr ${logaddrptr} + 4; " \
		"setexpr logsz ${logsz} + 0x08; " \
		"run put_logfile;\0" \
	"putreadyfile= " \
		"mw.l 0x83400100 0x72203166; " \
		"mw.l 0x83400104 0x79646165; " \
		"tftpput 0x83400100 8 192.168.0.1:ready1;\0" \
	"fuse_sec= " \
		"setexpr fuseerror 1; " \
		"if tftpboot ${fusefile}; then " \
			"echo got fuse file; " \
			"run logtxt02; " \
			"if fuse progm -y 3 0 ${loadaddr} 8; then " \
				"fuse sense 3 0 8;" \
				"echo sig done; " \
				"run logtxt03; " \
				"if fuse prog -y 0 6 0x2; then " \
				"fuse sense 0 6;" \
				"echo sec fused; " \
				"run logtxt04; " \
				"setexpr fuseerror 0; " \
				"fi; " \
			"fi; " \
		"else " \
			"echo no fuse file; " \
		"fi; " \
		"run finalfeedback;\0" \
	"finalfeedback=" \
		"if itest ${fuseerror} != 0; then " \
			"led red on; " \
			"while true; do echo .; sleep 5; done; " \
		"else " \
			"if i2c probe 0x20; then  " \
				"echo Found IO-Expander.; " \
				"run logtxt06; " \
				"led green on; " \
				"led red on; " \
			"else  " \
				"echo No IO-Expander, led feedback will not work.; " \
				"run logtxt05; " \
			"fi; " \
			"run putreadyfile; " \
			"sleep 5; " \
			"reset; " \
		"fi;\0" \
	"factory1=setexpr logaddrptr ${logaddr}; setexpr logsz 0; run logtxt01; run fuse_sec;\0" \
	"fusefile=SRK_1_2_3_4_fuse.bin\0" \
	"version=1.5\0"

/* Default environment */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0xffffffff\0" \
	"console=ttymxc0,115200n8\0" \
	"addcon=setenv bootargs ${bootargs} console=${console},${baudrate}\0" \
	"fit_addr=0x82000000\0" \
	ENV_MMC

#define CONFIG_BOOTCOMMAND "run ledon; run kbd_read; run ledoff; echo U-Boot-env version ${version}; run boot_switch; run mmc_mmc_fit; run swupdate;"

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(MMC, mmc, 1) \
	func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

#endif /* __KIE_ULL_H */
