#pragma once

// BOARD BASE PIN
#define BOARD_RESET_PIN     (-1)
#define BOARD_I2C_SDA_PIN   (14)
#define BOARD_I2C_SCL_PIN   (15)

// LCD PIN
#define LCD_RST_PIN         (-1)
#define LCD_BL_PIN          (16)
#define LCD_DISP_EN_GPIO    (-1)

// LCD RGB SPI CONFIG PIN
#define LCD_SPI_MOSI        (-1)
#define LCD_SPI_CLK         (-1)
#define LCD_SPI_CS          (-1)

// LCD RGB565 DRIVER PIN
#define LCD_PCLK_GPIO       (49)
#define LCD_VSYNC_GPIO      (46)
#define LCD_HSYNC_GPIO      (47)
#define LCD_DE_GPIO         (45)

#define LCD_D0_GPIO         (54)    // R0
#define LCD_D1_GPIO         (53)    // R1
#define LCD_D2_GPIO         (52)    // R2
#define LCD_D3_GPIO         (51)    // R3
#define LCD_D4_GPIO         (50)    // R4
#define LCD_D5_GPIO         (48)    // R5
#define LCD_D6_GPIO         (4)     // G0
#define LCD_D7_GPIO         (11)    // G1
#define LCD_D8_GPIO         (12)    // G2
#define LCD_D9_GPIO         (13)    // G3
#define LCD_D10_GPIO        (3)     // G4
#define LCD_D11_GPIO        (2)     // G5
#define LCD_D12_GPIO        (10)    // B0
#define LCD_D13_GPIO        (9)     // B1
#define LCD_D14_GPIO        (8)     // B2
#define LCD_D15_GPIO        (7)     // B3
#define LCD_D16_GPIO        (6)     // B4
#define LCD_D17_GPIO        (5)     // B5

// TOUCH PAD PIN
#define TP_I2C_SDA_PIN      (14)
#define TP_I2C_SCL_PIN      (15)
#define TP_I2C_INT_PIN      (0)
#define TP_I2C_RST_PIN      (1)

// SD CARD
#define SD_CLK_PIN          (43) // CLK
#define SD_CMD_PIN          (44) // MOSI
#define SD_D0_PIN           (39) // MISO
#define SD_D1_PIN           (40)
#define SD_D2_PIN           (41)
#define SD_D3_PIN           (42) // CS
#define SD_POWER_PIN        (21) // CS

// EXTERNAL PIN
#define EXTERNAL_PIN_0      (17)
#define EXTERNAL_PIN_1      (18)
#define EXTERNAL_PIN_2      (19)
#define EXTERNAL_PIN_3      (20)
#define EXTERNAL_PIN_4      (24)
#define EXTERNAL_PIN_5      (25)
#define EXTERNAL_PIN_6      (26)
#define EXTERNAL_PIN_7      (27)
