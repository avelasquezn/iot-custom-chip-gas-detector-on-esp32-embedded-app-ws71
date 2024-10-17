// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 Angel Velasquez

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

// MQ2 Data Record for Pin Configuration and Attribute Values
typedef struct {
  pin_t pin_a0;
  pin_t pin_d0;
  pin_t pin_vcc;
  pin_t pin_gnd;
  uint32_t gas_attr;
  uint32_t threshold_attr;
} chip_state_t;

// Pre-declare of Timer Event Handler
static void chip_timer_event(void *user_data);

// MQ2 Setup
void chip_init() {
  
  // Chip State Holder
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  
  // Pin Setup
  chip->pin_a0  = pin_init("A0", ANALOG);
  chip->pin_d0  = pin_init("D0", OUTPUT_LOW);
  chip->pin_vcc = pin_init("VCC", INPUT_PULLDOWN);
  chip->pin_gnd = pin_init("GND", INPUT_PULLUP);

  // Attributes Setup
  chip->gas_attr        = attr_init("gas", 10);
  chip->threshold_attr  = attr_init("threshold", 50); 

  // Timer Event Configuration
  const timer_config_t timer_config = {
    .callback = chip_timer_event,
    .user_data = chip
  };

  // Timer Initialization and Start
  timer_t timer_id = timer_init(&timer_config);
  timer_start(timer_id, 1000, true);

}

// Timer Event Handler Implementation
void chip_timer_event(void *user_data) {
  // Get Event Payload
  chip_state_t *chip = (chip_state_t*)user_data;
  // Calculate Voltage
  float voltage = (attr_read_float(chip->gas_attr))*5.0/100;
  // Calculate Threshold
  float threshold_v = (attr_read_float(chip->threshold_attr))*5.0/100;
  // Check if operating
  if (pin_read(chip->pin_vcc) && !pin_read(chip->pin_gnd)) {
    pin_dac_write(chip->pin_a0, voltage);
    // Check if Voltage Value exceeds Threshold Limit
    if(voltage > threshold_v)
      pin_write(chip->pin_d0, HIGH);
    else
      pin_write(chip->pin_d0, LOW);  
  }  
}
