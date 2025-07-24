// config.h

#ifdef VIA_ENABLE
#define VIA_UID_VENDOR_ID 0x4D16
#define VIA_UID_PRODUCT_ID 0x0002

// Povolení OLED displeje (OLED_DRIVER a OLED_TRANSPORT jsou v rules.mk)
#define OLED_DRIVER_ENABLE yes
#define OLED_DISPLAY_ADDRESS 0x3C // Adresa tvého OLED displeje

// Definice pinů pro I2C komunikaci s OLED displejem.
#define I2C_SCL_PIN GP19 // GPIO19 je SCL
#define I2C_SDA_PIN GP10 // GPIO10 je SDA

// Vynutíme použití I2C0, i když tvé piny jsou primárně pro I2C1.
#define I2C0_SCL_PIN I2C_SCL_PIN
#define I2C0_SDA_PIN I2C_SDA_PIN

#define OLED_FADE_OUT 1000
#define OLED_TIMEOUT 10000
#define OLED_BRIGHTNESS 125

#define BOOTMAGIC_ROW 0
#define BOOTMAGIC_COLUMN 0

// Definuj pin, který ovládá bázi tranzistoru pro vibrační motor.
#define SOLENOID_PIN GP18

// #define SOLENOID_PIN_ACTIVE_LOW // Důležité: Tento řádek MUSÍ zůstat ZAKOMENTOVANÝ

// Výchozí doba, po kterou je motor aktivní při spuštění haptiky (v ms).
#define SOLENOID_DEFAULT_DWELL 1

// Minimální a maximální doba dwell, kterou lze nastavit.
#define SOLENOID_MIN_DWELL 1
#define SOLENOID_MAX_DWELL 400

// Velikost kroku pro změnu dwell času.
#define SOLENOID_DWELL_STEP_SIZE 5

// Doporučeno: Vypnutí haptiky v režimu nízké spotřeby pro úsporu energie.
#define HAPTIC_OFF_IN_LOW_POWER 1

// DŮLEŽITÉ NOVÉ DEFINICE pro potlačení automatické haptiky
// Tyto definice zajistí, že haptika se nebude spouštět automaticky
// u různých kategorií kláves, ale pouze tam, kde ji voláte ručně.
#define NO_HAPTIC_MOD         // Zakáže haptiku pro modifikátory a momentární vrstvy/modifikátory
#define NO_HAPTIC_ALPHA       // Zakáže haptiku pro abecední klávesy (A-Z)
#define NO_HAPTIC_PUNCTUATION // Zakáže haptiku pro interpunkční klávesy
#define NO_HAPTIC_LOCKKEYS    // Zakáže haptiku pro Caps Lock, Scroll Lock, Num Lock
#define NO_HAPTIC_NAV         // Zakáže haptiku pro navigační klávesy
#define NO_HAPTIC_NUMERIC     // Zakáže haptiku pro numerické klávesy (0-9)

// Počet vrstev, které má QMK alokovat pro dynamickou keymapu (pro VIA).
#define DYNAMIC_KEYMAP_LAYER_COUNT 4

#endif // VIA_ENABLE