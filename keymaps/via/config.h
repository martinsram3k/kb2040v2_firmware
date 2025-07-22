// config.h

#ifdef VIA_ENABLE
#define VIA_UID_VENDOR_ID 0x4D16
#define VIA_UID_PRODUCT_ID 0x0002
#define OLED_DRIVER_ENABLE yes
#define OLED_DISPLAY_ADDRESS 0x3D

#define BOOTMAGIC_ROW 0
#define BOOTMAGIC_COLUMN 0

// Toto je klíčové. Podle tvých schémat je motor připojen k GPIO22,
// což je v QMK pro RP2040 desky GPB1 (nebo ekvivalentní označení pro pin 22).
// Změnil jsi to na GP18, což je GPIO18 (SCK pin na některých schématech).
// Pokud je motor stále fyzicky na pinu 22, MUSÍ to být GPB1.
// Pokud jsi motor fyzicky přepojil na pin 18, pak GP18 je OK, ale musí to odpovídat realitě!
// Předpokládám, že se vracíme k původnímu pinu 22 (GPB1), který je dle schématu.
#define SOLENOID_PIN GP18

// #define SOLENOID_PIN_ACTIVE_LOW // Důležité: Tento řádek MUSÍ zůstat ZAKOMENTOVANÝ

// Výchozí doba, po kterou je motor aktivní při spuštění haptiky (v ms).
#define SOLENOID_DEFAULT_DWELL 100

// Minimální a maximální doba dwell, kterou lze nastavit (pokud bys chtěl měnit za běhu)
#define SOLENOID_MIN_DWELL 4
#define SOLENOID_MAX_DWELL 100

// Velikost kroku pro změnu dwell času
#define SOLENOID_DWELL_STEP_SIZE 1

// Doporučeno: Vypnutí haptiky v režimu nízké spotřeby pro úsporu energie.
#define HAPTIC_OFF_IN_LOW_POWER 1

// Počet vrstev pro dynamickou keymapu (VIA). Máš 0, 1, 2, 3 = 4 vrstvy.
#define DYNAMIC_KEYMAP_LAYER_COUNT 4 

#endif // VIA_ENABLE