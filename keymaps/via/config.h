// config.h

#ifdef VIA_ENABLE
#define VIA_UID_VENDOR_ID 0x4D16
#define VIA_UID_PRODUCT_ID 0x0002

// Povolení OLED displeje (OLED_DRIVER a OLED_TRANSPORT jsou v rules.mk)
#define OLED_DRIVER_ENABLE yes
#define OLED_DISPLAY_ADDRESS 0x3C // Adresa tvého OLED displeje

// Definice pinů pro I2C komunikaci s OLED displejem.
// Pro RP2040 jsou GPIO piny definovány jako GPxx.
// Podle tvých obrázků: GPIO19 je SCL (SCL1), GPIO10 je SDA (SDA1)
#define I2C_SCL_PIN GP19 
#define I2C_SDA_PIN GP10 

// Vynutíme použití I2C0, i když tvé piny jsou primárně pro I2C1.
// Někdy toto pomůže obejít problémy s ovladači, které defaultně očekávají I2C1 na specifických pinech (B6, B7).
// QMK by mělo být schopné přesměrovat I2C0 na libovolné GPIO piny.
#define I2C0_SCL_PIN I2C_SCL_PIN
#define I2C0_SDA_PIN I2C_SDA_PIN
// NEPOUŽÍVAT OLED_DC_PIN ani OLED_CS_PIN pro I2C OLED displeje,
// protože tvůj displej je I2C (4 piny: VCC, GND, SDA, SCL) a tyto piny jsou pro SPI komunikaci.
// #define OLED_DC_PIN  <nějaký pin>
// #define OLED_CS_PIN  <nějaký pin>

#define BOOTMAGIC_ROW 0
#define BOOTMAGIC_COLUMN 0

// Definuj pin, který ovládá bázi tranzistoru pro vibrační motor.
// Dle tvého schématu (image_1dce16.png), pin "gpio vibracni motor" je na pinu 22, což je GPB1 v QMK pro RP2040.
#define SOLENOID_PIN GP18

// #define SOLENOID_PIN_ACTIVE_LOW // Důležité: Tento řádek MUSÍ zůstat ZAKOMENTOVANÝ

// Výchozí doba, po kterou je motor aktivní při spuštění haptiky (v ms).
#define SOLENOID_DEFAULT_DWELL 50

// Minimální a maximální doba dwell, kterou lze nastavit.
#define SOLENOID_MIN_DWELL 4
#define SOLENOID_MAX_DWELL 100

// Velikost kroku pro změnu dwell času.
#define SOLENOID_DWELL_STEP_SIZE 1

// Doporučeno: Vypnutí haptiky v režimu nízké spotřeby pro úsporu energie.
#define HAPTIC_OFF_IN_LOW_POWER 1

// Počet vrstev, které má QMK alokovat pro dynamickou keymapu (pro VIA).
#define DYNAMIC_KEYMAP_LAYER_COUNT 3 



#endif // VIA_ENABLE