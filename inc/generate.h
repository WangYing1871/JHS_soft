#ifndef generate_H
#define generate_H 1 
struct lcs_t{
  int layer_id=0;
  int channel_id=0;
  float compres=3;
};
struct lc_adc_t{
  uint16_t layer_id=0;
  uint16_t channel_id=0;
  uint16_t adc_t=0;
};
static uint8_t rise_step = 0x0;
static uint16_t delay_time = 0x258;
static uint8_t wait_cycle = 10;

#endif
