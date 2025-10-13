# rules.mk

VIA_ENABLE = yes // VIA support
OLED_ENABLE = yes // OLED display support
OLED_DRIVER = ssd1306 // OLED driver
OLED_TRANSPORT = i2c // OLED transport protocol

BOOTMAGIC_ENABLE = yes // Bootmagic

HAPTIC_ENABLE = yes // Haptic feedback support
HAPTIC_DRIVER = solenoid // Haptic driver

QMK_C_DEFS += -DKEYBOARD_USER_ENABLE

LTO_ENABLE = yes 