// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019 DENX Software Engineeering GmbH
 * Author: Parthiban Nallathambi <pn@denx.de>
 */

#include <asm/arch/clock.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <fsl_esdhc_imx.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/ctype.h>
#include <miiphy.h>
#include <netdev.h>
#include <usb.h>
#include <led.h>
#include <usb/ehci-ci.h>
#include <asm/mach-imx/boot_mode.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();

	return 0;
}

#ifdef CONFIG_FEC_MXC
#define ENET_CLK_PAD_CTRL (PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST)

#define ENET_PAD_CTRL     (PAD_CTL_PUS_100K_UP | PAD_CTL_PUE       | \
			   PAD_CTL_SPEED_HIGH  | PAD_CTL_DSE_48ohm | \
			   PAD_CTL_SRE_FAST)

#define MDIO_PAD_CTRL     (PAD_CTL_PUS_100K_UP | PAD_CTL_PUE      | \
			   PAD_CTL_DSE_48ohm   | PAD_CTL_SRE_FAST | \
			   PAD_CTL_ODE)

#define RESET_PAD_CTRL     (PAD_CTL_DSE_48ohm  | PAD_CTL_SRE_SLOW | \
			   PAD_CTL_ODE)

static iomux_v3_cfg_t const fec2_pads[] = {
	MX6_PAD_GPIO1_IO06__ENET2_MDIO | MUX_PAD_CTRL(MDIO_PAD_CTRL),
	MX6_PAD_GPIO1_IO07__ENET2_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_DATA0__ENET2_TDATA00 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_DATA1__ENET2_TDATA01 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_EN__ENET2_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_CLK__ENET2_REF_CLK2 | MUX_PAD_CTRL(ENET_CLK_PAD_CTRL),
	MX6_PAD_ENET2_RX_DATA0__ENET2_RDATA00 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_RX_DATA1__ENET2_RDATA01 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_RX_ER__GPIO2_IO15 | MUX_PAD_CTRL(PAD_CTL_PUS_100K_DOWN |
						       PAD_CTL_DSE_48ohm |
						       PAD_CTL_SRE_SLOW),
	MX6_PAD_ENET2_RX_EN__ENET2_RX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
};

static iomux_v3_cfg_t const phy_reset_pads[] = {
	MX6_PAD_LCD_DATA17__GPIO3_IO22 | MUX_PAD_CTRL(RESET_PAD_CTRL),
};

static void setup_iomux_fec(void)
{
	imx_iomux_v3_setup_multiple_pads(fec2_pads, ARRAY_SIZE(fec2_pads));
}

static int setup_fec(int fec_id)
{
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	clrsetbits_le32(&iomuxc_regs->gpr[1],
			IOMUX_GPR1_FEC2_CLOCK_MUX1_SEL_MASK,
			IOMUX_GPR1_FEC2_CLOCK_MUX2_SEL_MASK);

	setup_iomux_fec();
	imx_iomux_v3_setup_multiple_pads(phy_reset_pads,
					 ARRAY_SIZE(phy_reset_pads));

	/* According to spec - mv88e6020 requires 10ms+ RESETn to be low */
	gpio_request(IMX_GPIO_NR(3, 22), "RESET_SW");
	gpio_direction_output(IMX_GPIO_NR(3, 22), 0);
	mdelay(15);
	gpio_set_value(IMX_GPIO_NR(3, 22), 1);
	mdelay(20);

	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}
#endif

int board_early_init_f(void)
{
#ifdef CONFIG_FEC_MXC
	setup_iomux_fec();
#endif
	return 0;
}
#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 8 bit bus width */
	{"emmc", MAKE_CFGVAL(0x60, 0x48, 0x00, 0x00)},
	//{"emmc", MAKE_CFGVAL(0x40, 0x28, 0x00, 0x00)},
	{NULL,	 0},
};
#endif

int board_init(void)
{
	/* Address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_FEC_MXC
	setup_fec(CONFIG_FEC_ENET_DEV);
#endif
	return 0;
}

int board_late_init(void)
{
	u32 cpurev = get_cpu_rev();
	const char *soc_type = get_imx_type((cpurev & 0xFF000) >> 12);
	char p[sizeof(soc_type)];

#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

	if (IS_ENABLED(CONFIG_LED))
		led_default_state();

	/* Convert to lower case */
	strcpy(p, soc_type);
	for(int i = 0; p[i]; i++)
		p[i] = tolower(p[i]);

	env_set("soc_type", p);
	return 0;
}

int checkboard(void)
{
	u32 cpurev = get_cpu_rev();

	printf("Board: Kostal KIE inverter-i.MX%s\n",
	      get_imx_type((cpurev & 0xFF000) >> 12));

	return 0;
}
