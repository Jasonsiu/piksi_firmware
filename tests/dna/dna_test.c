/*
 * Copyright (C) 2011 Fergus Noble <fergusnoble@gmail.com>
 * Copyright (C) 2012 Colin Beighley <colin@swift-nav.com>
 *
 */

#include <stdio.h>
#include <libopencm3/stm32/f2/rcc.h>
#include <libopencm3/stm32/f2/flash.h>
#include <libopencm3/stm32/f2/gpio.h>

#include "main.h"
#include "debug.h"
#include "swift_nap_io.h"
#include "acq.h"
#include "hw/spi.h"
#include "hw/leds.h"
#include "hw/m25_flash.h"

int main(void) {

  led_setup();
  led_on(LED_GREEN);
  led_on(LED_RED);

  /* Force FPGA to configure itself by driving PROGRAM_B low, then high */
  RCC_AHB1ENR |= RCC_AHB1ENR_IOPCEN;
  gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
  gpio_clear(GPIOC, GPIO12);
  for (u32 i = 0; i < 50; i++)
    __asm__("nop");
  gpio_set(GPIOC, GPIO12);

  while (!(swift_nap_conf_done() && swift_nap_hash_rd_done()))
    __asm__("nop");

  debug_setup();
  swift_nap_setup();
  m25_setup();

  printf("\n\nFirmware info - git: " GIT_VERSION ", built: " __DATE__ " " __TIME__ "\n");
  printf("--- DNA TEST ---\n");

  while (1) {
    for (u32 i = 0; i < 10000; i++) 
      __asm__("nop");
    DO_EVERY(50,
      led_toggle(LED_GREEN);
      led_toggle(LED_RED);
    );
    debug_process_messages();
  }

  return 0;
}
