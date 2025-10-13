#ifdef VIA_ENABLE  // Povolené via
#define VIA_UID_VENDOR_ID 0x4D16 
#define VIA_UID_PRODUCT_ID 0x0002

#define OLED_DRIVER_ENABLE yes
#define OLED_DISPLAY_ADDRESS 0x3C  // Adresa I2C displeje OLED

#define I2C_SCL_PIN GP19 // Pin SCL I2C
#define I2C_SDA_PIN GP10 // Pin SDA I2C

#define I2C0_SCL_PIN I2C_SCL_PIN
#define I2C0_SDA_PIN I2C_SDA_PIN

#define OLED_FADE_OUT 2000 // Trvání vyblednutí OLED v ms
#define OLED_TIMEOUT 20000 // Vypnutí OLED po nečinnosti v ms
#define OLED_BRIGHTNESS 255 // Jas OLED (0-255)

#define BOOTMAGIC_ROW 0 // Řádek pro Bootmagic (tlačítko v levém horním rohu)
#define BOOTMAGIC_COLUMN 0 // Sloupec pro Bootmagic (tlačítko v levém horním rohu)

#define SOLENOID_PIN GP18 // Pin pro haptiku

#define SOLENOID_DEFAULT_DWELL 20 // Výchozí doba aktivace haptiky v ms

#define SOLENOID_MIN_DWELL 1 // Minimální doba aktivace haptiky v ms
#define SOLENOID_MAX_DWELL 400 // Maximální doba aktivace haptiky v ms

#define SOLENOID_DWELL_STEP_SIZE 5 // Krok pro změnu doby aktivace haptiky v ms

#define HAPTIC_OFF_IN_LOW_POWER 1 // Vypnutí haptiky v režimu nízké spotřeby

#define NO_HAPTIC_MOD         
#define NO_HAPTIC_ALPHA       
#define NO_HAPTIC_PUNCTUATION 
#define NO_HAPTIC_LOCKKEYS    
#define NO_HAPTIC_NAV         
#define NO_HAPTIC_NUMERIC     

#define DYNAMIC_KEYMAP_LAYER_COUNT 4 // Počet vrstev pro dynamickou klávesnici

#endif 