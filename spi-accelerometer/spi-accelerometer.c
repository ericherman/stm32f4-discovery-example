/*
 * This file was part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2012 Kendrick Shaw <kms15@case.edu>
 * Copyright (C) 2012 Eric Herman <eric@freesa.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdlib.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/f4/memorymap.h>
#include <libopencm3/stm32/f4/spi.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include "spi-accelerometer-usb-descriptors.h"
#include "util.h"
#include "opencm3util.h"

static int cdcacm_control_request(usbd_device *usbd_dev,
				  struct usb_setup_data *req, uint8_t **buf,
				  uint16_t *len,
				  void (**complete) (usbd_device *usbd_dev,
						     struct usb_setup_data
						     *req))
{
	/* by casting to void, we avoid an unused argument warning */
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:{
			/*
			 * This Linux cdc_acm driver requires this to be
			 * implemented even though it's optional in the CDC
			 * spec, and we don't advertise it in the ACM
			 * functional descriptor.
			 */
			return 1;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
			return 0;

		return 1;
	}
	return 0;
}

static uint8_t send_command(uint16_t command, uint8_t data)
{
	uint16_t return_value;
	uint16_t ignore;

	gpio_clear(GPIOE, GPIO3);
	spi_send(SPI1, command);
	ignore = spi_read(SPI1);
	(void)ignore;
	spi_send(SPI1, data);
	return_value = spi_read(SPI1);
	gpio_set(GPIOE, GPIO3);
	return (uint8_t)return_value;
}

static uint8_t read_motion_axis(uint8_t axis)
{
	uint16_t command;
	uint8_t data;

	data = 0;

	command =
	    /* READ bit */
	    (0x1 << 7) |
	    /* MS bit:  When 0 do not increment address */
	    (0x0 << 6) |
	    /* bits 2-7 are address */
	    (axis << 0);

	return send_command(command, data);
}

static uint32_t read_motion(void)
{
	uint8_t x, y, z;
	uint32_t combined;

	x = read_motion_axis(0x29);
	y = read_motion_axis(0x2B);
	z = read_motion_axis(0x2D);

	combined = (((uint32_t)x) << 16) | (((uint32_t)y) << 8) | z;
	return combined;
}

static void setup_accelerometer(void)
{
	uint16_t command;
	uint8_t data;

	command =
	    /* READ bit not set */
	    (0x0 << 7) |
	    /* MS bit:  When 0 do not increment address */
	    (0x0 << 6) |
	    /* bits 2-7 are address */
	    (0x20 << 0);

	data =
	    /* data rate selection, 1 = 400Hz */
	    (0x1 << 7) |
	    /* power down control, 1 = active */
	    (0x1 << 6) |
	    /* full scale selection (1 = 8G, 0 = 2G) */
	    (0x0 << 5) |
	    /* Z axis enable */
	    (0x1 << 2) |
	    /* Y axis enable */
	    (0x1 << 1) |
	    /* X axis enable */
	    (0x1 << 0);

	send_command(command, data);
}

/* populates a string buf with text like: "X: 0x23, Y: 0x42, Z: 0x02\r\n" */
static void text_for_motion(uint32_t motion, char *buf, int *len)
{
	int i;
	uint8_t x, y, z;

	x = (uint8_t)(motion >> 16);
	y = (uint8_t)(motion >> 8);
	z = (uint8_t)(motion >> 0);

	i = 0;

	buf[i++] = 'X';
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = '0';
	buf[i++] = 'x';
	buf[i++] = to_hex(x, 1);
	buf[i++] = to_hex(x, 0);

	buf[i++] = ',';
	buf[i++] = ' ';

	buf[i++] = 'Y';
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = '0';
	buf[i++] = 'x';
	buf[i++] = to_hex(y, 1);
	buf[i++] = to_hex(y, 0);

	buf[i++] = ',';
	buf[i++] = ' ';

	buf[i++] = 'Z';
	buf[i++] = ':';
	buf[i++] = ' ';
	buf[i++] = '0';
	buf[i++] = 'x';
	buf[i++] = to_hex(z, 1);
	buf[i++] = to_hex(z, 0);

	buf[i++] = '\r';
	buf[i++] = '\n';

	*len = i;
}

static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	char buf[64];
	uint32_t motion;
	int len;

	(void)ep;

	len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

	if (len) {
		motion = read_motion();
		text_for_motion(motion, buf, &len);
		while (usbd_ep_write_packet(usbd_dev, 0x82, buf, len) == 0) ;
	}
	/* flash the LEDs so we know we're doing something */
	gpio_toggle(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
}

static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64,
		      cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

	usbd_register_control_callback(usbd_dev,
				       USB_REQ_TYPE_CLASS |
				       USB_REQ_TYPE_INTERFACE,
				       USB_REQ_TYPE_TYPE |
				       USB_REQ_TYPE_RECIPIENT,
				       cdcacm_control_request);
}

static void setup_main_clock(void)
{
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_168MHZ]);
}

static void setup_peripheral_clocks(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_OTGFS);
	rcc_periph_clock_enable(RCC_SPI1);
}

static usbd_device *setup_usb(uint8_t *usbd_control_buffer, size_t buf_len)
{
	usbd_device *usbd_dev;

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);

	usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
			     usb_strings, 3, usbd_control_buffer, buf_len);
	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);
	return usbd_dev;
}

static void setup_spi(void)
{
	/* chip select */
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	/* set to high which is not-selected */
	gpio_set(GPIOE, GPIO3);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
			/* serial clock */
			GPIO5 |
			/* master in/slave out */
			GPIO6 |
			/* master out/slave in */
			GPIO7);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

	spi_disable_crc(SPI1);
	spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_32,
			/* high or low for the peripheral device */
			SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
			/* CPHA: Clock phase: read on rising edge of clock */
			SPI_CR1_CPHA_CLK_TRANSITION_2,
			/* DFF: Date frame format (8 or 16 bit) */
			SPI_CR1_DFF_8BIT,
			/* Most or Least Sig Bit First */
			SPI_CR1_MSBFIRST);

	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);

	spi_clear_mode_fault(SPI1);

	spi_enable(SPI1);
}

static void setup_leds(void)
{
	/* enable the four LEDs */
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,
			GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	/* Set two LEDs for wigwag effect when toggling. */
	gpio_set(GPIOD, GPIO12 | GPIO14);
}

int main(void)
{
	usbd_device *usbd_dev;
	uint8_t usbd_control_buffer[128];

	setup_main_clock();
	setup_peripheral_clocks();
	usbd_dev = setup_usb(usbd_control_buffer, 128);
	setup_spi();
	setup_leds();
	setup_accelerometer();

	while (1) {
		usbd_poll(usbd_dev);
	}
}
