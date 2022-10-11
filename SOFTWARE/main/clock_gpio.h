#define PIN_NUM_DS      3
#define PIN_NUM_ST      4
#define PIN_NUM_SH      5

#define PIN_NUM_TX      6
#define PIN_NUM_RX      7
#define PIN_NUM_GPSOFF  10

#define PIN_NUM_KEY0    2
#define PIN_NUM_KEY1    8
#define PIN_NUM_KEY2    9

#define SMG_A           (1<<0)
#define SMG_B           (1<<1)
#define SMG_C           (1<<2)
#define SMG_D           (1<<3)
#define SMG_E           (1<<4)
#define SMG_F           (1<<5)
#define SMG_G           (1<<6)
#define SMG_P           (1<<7)

#define SMG_BIT0        (1<<0)
#define SMG_BIT1        (1<<1)
#define SMG_BIT2        (1<<2)
#define SMG_BIT3        (1<<3)
#define SMG_BIT4        (1<<4)
#define SMG_BIT5        (1<<5)

#define SMG_0           (SMG_A | SMG_B | SMG_F | SMG_D | SMG_C | SMG_E)
#define SMG_1           (SMG_B | SMG_C)
#define SMG_2           (SMG_A | SMG_B | SMG_G | SMG_E | SMG_D)
#define SMG_3           (SMG_A | SMG_B | SMG_G | SMG_C | SMG_D)
#define SMG_4           (SMG_F | SMG_B | SMG_G | SMG_C)
#define SMG_5           (SMG_A | SMG_F | SMG_D | SMG_G | SMG_C)
#define SMG_6           (SMG_A | SMG_F | SMG_D | SMG_G | SMG_C | SMG_E)
#define SMG_7           (SMG_A | SMG_B | SMG_C)
#define SMG_8           (SMG_A | SMG_B | SMG_F | SMG_D | SMG_G | SMG_C | SMG_E)
#define SMG_9           (SMG_A | SMG_B | SMG_F | SMG_D | SMG_G | SMG_C)