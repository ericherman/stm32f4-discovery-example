/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2012, 2020 Eric Herman <eric@freesa.org>
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

#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/scb.h>

#include "rotate-chars-usb-descriptors.h"
#include "rot13.h"

/* CDC (ACM): Communication Device Class (Abstract Control Model) */

static enum usbd_request_return_codes	//
cdcacm_control_request(usbd_device *usbd_dev,
		       struct usb_setup_data *req,
		       uint8_t **buf,
		       uint16_t *len,
		       void (**complete)(usbd_device *usbd_dev,
					 struct usb_setup_data *req))
{
	/* by casting to void, we avoid an unused argument warning */
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest) {
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:{
			/*
			 * This Linux cdc_acm driver requires this to be implemented
			 * even though it's optional in the CDC spec, and we don't
			 * advertise it in the ACM functional descriptor.
			 */
			return USBD_REQ_HANDLED;
		}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding)) {
			return USBD_REQ_NOTSUPP;
		}

		return USBD_REQ_HANDLED;
	}
	return USBD_REQ_NOTSUPP;
}

static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;

	char buf[64];
	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

	if (len) {
		rotate_letters(buf, (unsigned int)len);
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
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
}

static void setup_peripheral_clocks(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_OTGFS);
	rcc_periph_clock_enable(RCC_GPIOD);
}

static usbd_device *setup_usb(uint8_t *usbd_control_buffer, size_t buf_len)
{
	usbd_device *usbd_dev;

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO11 | GPIO12);

	usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
			     usb_strings, 3, usbd_control_buffer, buf_len);
	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);
	return usbd_dev;
}

static void setup_leds(void)
{
	/* enable the four LEDs */
	/* Set GPIO12-15 (in GPIO port D) to 'output push-pull'. */
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
	setup_leds();

	while (1) {
		usbd_poll(usbd_dev);
	}
}
