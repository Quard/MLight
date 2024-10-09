#ifndef __FEATURES_QC_H__
#define __FEATURES_QC_H__

#include "stm32g030xx.h"
#include "mlight.h"
#include "utils.h"


#define QC_DELAY_START_CHECK 5
#define QC_DELAY_HANDSHAKE   1500
#define QC_DELAY_NEW_MODE    80

#define QC_RET_NOT_SUPPORTED -10
#define QC_RET_10_ONLY       10
#define QC_RET_OK            0


typedef enum {
	QC_PIN_GND,
	QC_PIN_06,
	QC_PIN_33,
} qc_pin_mode_t;

typedef enum {
    QC_MODE_5V,
    QC_MODE_9V,
    QC_MODE_12V,
    QC_MODE_20V,
    QC_MODE_CONTINUOUS,
} qc_mode_t;


typedef struct {
	qc_mode_t mode;
    int8_t voltage;  // V * 10 -- 50 for 5V, 120 for 12V
} qc_t;

int8_t qc_init(qc_t *qc);
void qc_set_mode(qc_t *qc, qc_mode_t mode);

#endif /* __FEATURES_QC_H__ */
