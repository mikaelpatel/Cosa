#ifndef __CONFIG_H__
#define __CONGIG_H__

#include "Cosa/Types.h"

enum bias_t {
  NORMAL_BIAS,
  POSITIVE_BIAS,
  NEGATIVE_BIAS
} __attribute__((packed));

enum rate_t {
  OUTPUT_RATE_0_75_HZ,
  OUTPUT_RATE_1_5_HZ,
  OUTPUT_RATE_3_HZ,
  OUTPUT_RATE_7_5_HZ,
  OUTPUT_RATE_15_HZ,
  OUTPUT_RATE_30_HZ,
  OUTPUT_RATE_75_HZ
} __attribute__((packed));

enum avg_t {
  SAMPLES_AVG_1,
  SAMPLES_AVG_2,
  SAMPLES_AVG_4,
  SAMPLES_AVG_8,
} __attribute__((packed));

union config_t {
  uint8_t reg;
  struct {
    bias_t bias:2;
    rate_t rate:3;
    avg_t avg:3;
  };
};
#endif
