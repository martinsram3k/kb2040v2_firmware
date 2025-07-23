// config.h

#ifdef VIA_ENABLE
#define VIA_UID_VENDOR_ID 0x4D16
#define VIA_UID_PRODUCT_ID 0x0002

// Povolení OLED displeje (OLED_DRIVER a OLED_TRANSPORT jsou v rules.mk)
#define OLED_DRIVER_ENABLE yes
#define OLED_DISPLAY_ADDRESS 0x3C // Adresa tvého OLED displeje

// Definice pinů pro I2C komunikaci s OLED displejem.
#define I2C_SCL_PIN GP19 
#define I2C_SDA_PIN GP10 

// Vynutíme použití I2C0, i když tvé piny jsou primárně pro I2C1.
#define I2C0_SCL_PIN I2C_SCL_PIN
#define I2C0_SDA_PIN I2C_SDA_PIN

#define BOOTMAGIC_ROW 0
#define BOOTMAGIC_COLUMN 0

// Definuj pin, který ovládá bázi tranzistoru pro vibrační motor.
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

// NOVÉ DŮLEŽITÉ PŘÍDAVKY PRO KONTROLU HAPTIKY:
// TYTO ZAJISTÍ, ŽE HAPTIKA SE NEBUDE SPUŠTĚT AUTOMATICKY.
// Vypnout haptiku při stisku každé klávesy.
#define HAPTIC_ENABLE_FEEDBACK_FOR_EVERY_KEY false 
// Vypnout haptiku pro změny vrstev, pokud by to nějak kolidovalo,
// ale my ji spouštíme manuálně v layer_state_set_user().
// Zde bychom spíše očekávali, že defaultní haptika pro změnu vrstev nebude aktivní,
// jelikož ji přebíráme v layer_state_set_user.
// Pro jistotu bych toto ponechal jako false, pokud se objeví duplicitní vibrace.
// #define HAPTIC_ENABLE_FEEDBACK_LAYER_CHANGE false 

// Počet vrstev, které má QMK alokovat pro dynamickou keymapu (pro VIA).
#define DYNAMIC_KEYMAP_LAYER_COUNT 3 

#endif // VIA_ENABLE