#include QMK_KEYBOARD_H

// Tyto hlavičkové soubory jsou potřeba pro low-level GPIO funkce na RP2040.
#include "hardware/gpio.h"

// Hlavičkový soubor pro haptický ovladač QMK
#include "haptic.h" // Pro funkci haptic_play()

// Definice vlastních keycodů. QK_USER zajistí, že se nepřekrývá s existujícími keycody QMK.
enum keycodes {  
  KC_CYCLE_LAYERS = QK_USER,
  // Keycody pro ovládání haptiky
  KC_HAPTIC_ON,    // Zapne haptiku (tato již nebude použita v keymapě, ale je stále definována)
  KC_HAPTIC_OFF,   // Vypne haptiku (tato již nebude použita v keymapě, ale je stále definována)
}; 

// První vrstva v cyklu (vrstva 0)
#define LAYER_CYCLE_START 0 
// Poslední vrstva v cyklu, do které se cykluje (vrstva 2, což znamená vrstvy 0, 1, 2)
// Vrstva 3 (modifikátorů) je vyloučena z cyklování.
#define LAYER_CYCLE_END   3 // Nyní je 3, protože cyklujeme do indexu 2 (0, 1, 2)

// Doba držení v milisekundách pro aktivaci vrstvy 3 (2 sekundy)
#define HOLD_MODIFIER_LAYER_DELAY 2000 

// Statická proměnná pro uložení předchozího stavu vrstev.
// Používá se pro detekci změny vrstvy pro haptiku.
static uint32_t last_layer_state = 0;

// Nová statická proměnná pro uložení vrstvy, na které jsme byli před stiskem KC_CYCLE_LAYERS.
// Používá se pro správné cyklování po klepnutí.
static uint8_t previous_base_layer = 0;

// Čas, kdy byla klávesa KC_CYCLE_LAYERS stisknuta
static uint32_t layer_key_press_timer = 0;
// Flag, zda je vrstva modifikátorů (3) momentálně aktivní (po držení)
static bool is_modifier_layer_active = false;

// Proměnná pro stav haptiky - zda je zapnuta nebo vypnuta
// Protože již nemáme přímé KC_HAPTIC_ON/OFF v keymapě, tato proměnná je nyní bez efektu,
// pokud haptika není ovládána jiným způsobem (např. automaticky na změnu vrstvy).
// Nicméně ji zde ponechávám, pokud by se v budoucnu přidaly jiné funkce pro ovládání haptiky.
static bool haptic_enabled = true; // Haptika je ve výchozím stavu zapnuta


// Funkce volaná po inicializaci klávesnice
void keyboard_post_init_user(void) {
    // Nastavení pinu SOLENOID_PIN jako GPIO výstupu.
    // SOLENOID_PIN je definován v config.h
    gpio_set_function(SOLENOID_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(SOLENOID_PIN, GPIO_OUT);
    // Zajistíme, že pin je na nízké úrovni na začátku, aby motor nebzučel ihned.
    gpio_put(SOLENOID_PIN, 0); 
    
    // Inicializuje haptický subsystém QMK.
    haptic_init(); 
    // Inicializujeme last_layer_state s aktuálním stavem vrstev při startu.
    last_layer_state = layer_state; 
    // Inicializujeme previous_base_layer s aktuální vrstvou.
    // Důležité: Vrstva 3 není "základní" vrstva, takže pokud jsme náhodou na ní
    // při startu (např. po hot-reloadu), vrátíme se na 0.
    uint8_t current_highest = get_highest_layer(layer_state);
    if (current_highest == 3) { // Pokud je vrstva 3 aktivní při startu
        previous_base_layer = LAYER_CYCLE_START; // Nastavíme na start cyklu
        layer_move(LAYER_CYCLE_START); // A přesuneme se na ni
    } else {
        previous_base_layer = current_highest;
    }
}

// Tato funkce se volá v každém cyklu skenování matice.
// Zde budeme kontrolovat časovač pro aktivaci vrstvy 3.
void matrix_scan_user(void) {
    // Pokud je klávesa KC_CYCLE_LAYERS stisknuta a vrstva 3 ještě není aktivní,
    // a uplynula doba HOLD_MODIFIER_LAYER_DELAY
    if (layer_key_press_timer > 0 && 
        !is_modifier_layer_active && 
        timer_elapsed(layer_key_press_timer) >= HOLD_MODIFIER_LAYER_DELAY) {
        
        // Aktivujeme vrstvu modifikátorů (vrstva 3)
        layer_on(3);
        // Nastavíme flag, že je vrstva 3 aktivní
        is_modifier_layer_active = true;
    }
}

// Tato funkce se volá POKAZDÉ, kdy dojde ke ZMĚNĚ stavu vrstev.
// Zde budeme spouštět haptiku POUZE při změně vrstvy.
layer_state_t layer_state_set_user(layer_state_t state) {
    // Zavolejte původní funkci QMK pro zpracování stavu vrstev, je to důležité.
    state = default_layer_state_set_user(state);

    // Zkontrolujeme, zda se aktuální stav vrstev (po default_layer_state_set_user) liší od předchozího stavu.
    // A také zkontrolujeme, zda je haptika povolena.
    if (state != last_layer_state && haptic_enabled) {
        // Pokud došlo ke změně vrstvy a haptika je zapnutá, spustíme haptiku.
        haptic_play();
    }
    
    // Uložíme aktuální stav vrstev pro další porovnání.
    last_layer_state = state;

    return state;
}

// Toto je hlavní funkce pro zpracování stisku/uvolnění kláves.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Zpracování logiky podle stisknutého keycode.
    switch (keycode) { 
        case KC_CYCLE_LAYERS: 
            if (record->event.pressed) {
                // Klávesa stisknuta
                // Uložíme čas stisku.
                layer_key_press_timer = timer_read();
                // Resetujeme flag aktivní modifikátorové vrstvy.
                is_modifier_layer_active = false;

                // Uložíme aktuální základní vrstvu před případnou aktivací vrstvy modifikátorů.
                uint8_t current_highest = get_highest_layer(layer_state);
                if (current_highest != 3) { // Pouze pokud nejsme na vrstvě 3
                    previous_base_layer = current_highest;
                }
                
                return false; // Klávesa byla zpracována, neposílat dál do QMK.
            } else {
                // Klávesa uvolněna
                // Resetujeme časovač pro tuto klávesu.
                layer_key_press_timer = 0; 
                
                // Pokud byla vrstva modifikátorů aktivní (byla držena dostatečně dlouho), vypneme ji.
                if (is_modifier_layer_active) {
                    layer_off(3);
                    is_modifier_layer_active = false; // Resetujeme flag
                } else {
                    // Pokud vrstva modifikátorů NEBYLA aktivní (klávesa byla uvolněna před prodlevou),
                    // znamená to, že to byl "tap" nebo "rychlý hold", který by měl cyklovat.
                    
                    // Provedeme cyklování vrstev.
                    // Cyklujeme z vrstvy, na které jsme byli před tapem/holdem.
                    uint8_t current_layer_for_cycle = previous_base_layer; 

                    // Zkontrolujeme, zda jsme v definovaném rozsahu cyklování (0, 1, 2).
                    // Pokud je current_layer_for_cycle mimo rozsah (např. 3),
                    // vrátíme se na začátek cyklu, protože na 3 se necykluje.
                    if (current_layer_for_cycle >= LAYER_CYCLE_END || current_layer_for_cycle < LAYER_CYCLE_START) { 
                        current_layer_for_cycle = LAYER_CYCLE_START; 
                    }
                    
                    // Vypočítáme další vrstvu v cyklu (pouze 0, 1, 2).
                    uint8_t next_layer = current_layer_for_cycle + 1; 
                    // Pokud jsme na konci cyklu (vrstva 2), vrátíme se na začátek (vrstvu 0).
                    if (next_layer >= LAYER_CYCLE_END) { // Nyní LAYER_CYCLE_END je 3, takže když next_layer dosáhne 3, cyklujeme na 0.
                        next_layer = LAYER_CYCLE_START; 
                    }
                    
                    // Přesuneme klávesnici na vypočítanou další vrstvu.
                    // Tato funkce způsobí volání layer_state_set_user(), kde se haptika spustí.
                    layer_move(next_layer); 
                }
                return false; // Klávesa byla zpracována, QMK ji už dál zpracovávat nemusí.
            }
        
        // Zpracování pro tlačítka haptiky (tyto keycody již nejsou v keymapě, ale funkce zůstává)
        case KC_HAPTIC_ON:
            if (record->event.pressed) {
                haptic_enabled = true; // Zapneme haptiku
                // Můžeme zde přidat i krátkou haptickou odezvu pro potvrzení, že se zapnula
                haptic_play(); 
            }
            return false; // Zpracováno
        
        case KC_HAPTIC_OFF:
            if (record->event.pressed) {
                haptic_enabled = false; // Vypneme haptiku
            }
            return false; // Zpracováno

        // Zpracování pro VŠECHNY OSTATNÍ keycody.
        default:
            // Vracíme true, aby QMK zpracoval klávesu normálně (tj. odeslal ji do počítače).
            return true; 
    }
}

// Definice layoutů klávesnice pro jednotlivé vrstvy.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Vrstva 0 
    [0] = LAYOUT_martin_3x3(
        KC_A, KC_B, KC_C, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_D, KC_E, KC_F, KC_G,
        KC_H, KC_I, KC_TRNS                 // KC_NO = prázdná/neaktivní klávesa
    ),

    // Vrstva 1 
    [1] = LAYOUT_martin_3x3(
        KC_B, KC_NO, KC_NO, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_TRNS                     
    ),

    // Vrstva 2 
    [2] = LAYOUT_martin_3x3(
        KC_C, KC_NO, KC_NO, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_TRNS                // KC_TRNS = transparentní klávesa (ale zde je KC_NO podle zadání)
    ),

    // Vrstva modifikátorů (upravena podle požadavků)
     [3] = LAYOUT_martin_3x3(
        QK_HAPTIC_RESET, QK_HAPTIC_DWELL_UP, QK_HAPTIC_DWELL_DOWN, KC_CYCLE_LAYERS, // První řada
        QK_BOOT, KC_NO, KC_NO, KC_NO, // Druhá řada s jasem
        KC_NO, KC_NO, KC_TRNS                 // Třetí řada s jedním QK_BOOT navíc
     ),
};


#ifdef OLED_ENABLE

bool oled_task_user(void) {


  
    static const char image1 [] PROGMEM = {
// 'L1', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x10, 
0x30, 0x38, 0x38, 0x38, 0x38, 0x38, 0x30, 0x00, 0x00, 0x00, 0x00, 0x10, 0x30, 0x38, 0x38, 0x38, 
0x38, 0x38, 0x38, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 
0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf0, 0xf8, 0xf8, 
0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 
0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x07, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x80, 0xc0, 0x80, 0x80, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xfc, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xe0, 0xe0, 
0xc0, 0x80, 0x00, 0xe0, 0xe0, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x0f, 0x0f, 0x0f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xff, 0x7e, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x03, 0x07, 0x07, 0x0f, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xe0, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x01, 
0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x0c, 0x0c, 
0x0c, 0x0c, 0x0c, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x04, 
0x00, 0x00, 0x00, 0x06, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 
0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x0f, 0x0f, 
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 
 };

    static const char image2 [] PROGMEM = {
  // 'L2', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0x00, 
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 
0x30, 0x30, 0x30, 0x70, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xc0, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xe0, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0x00, 
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 
0x30, 0x30, 0x30, 0x70, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x0f, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x80, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x1f, 0x3f, 0x3f, 
0x7f, 0xff, 0xff, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x0f, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xe0, 0xe0, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7c, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x1e, 0x1f, 0x1f, 0x0f, 0x07, 0x0f, 0x0f, 0x1f, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xc1, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xfc, 
0xf8, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1c, 0x1c, 
0x1c, 0x1c, 0x1c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x08, 
0x00, 0x00, 0x00, 0x0c, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x0f, 0x07, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x1c, 0x1c, 
0x1c, 0x1c, 0x1c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x08, 
0x00, 0x00, 0x00, 0x00, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };

    static const char image3 [] PROGMEM = {
       // 'L3', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x70, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe0, 0x00, 0x00, 0x00, 
0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 
0x30, 0x30, 0x30, 0x30, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0x1f, 0x1f, 0x1f, 0xff, 0x7f, 0x3f, 0x7f, 0x7f, 0x1f, 0x1f, 0x1f, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xe0, 0xe0, 
0xc0, 0x80, 0x00, 0xe0, 0xe0, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xf0, 0xf0, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xfc, 0xf8, 0xf0, 0xf0, 0xf0, 0xf0, 0xf8, 0xfc, 0xf8, 0xf8, 0xf0, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xff, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x01, 
0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe1, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 
0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x07, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 
0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 
0x00, 0x00, 0x00, 0x00, 0x0f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    };

    static const char image4 [] PROGMEM = {
 // 'S', 128x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xe0, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x3f, 0x0f, 0x0f, 0x0f, 0x0f, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0x07, 
0x03, 0x83, 0x83, 0x83, 0x03, 0x07, 0x0f, 0xff, 0xff, 0xff, 0x87, 0x87, 0x87, 0x07, 0x07, 0x07, 
0x07, 0x07, 0x07, 0xff, 0xff, 0xff, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0xff, 
0xff, 0xff, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0x3f, 0x3f, 
0x3f, 0x3f, 0x3f, 0x3f, 0x1f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 
0x0f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x3f, 0x0f, 0x0f, 0x0f, 0x0f, 0x1f, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xf0, 0xe0, 0xe0, 0xe0, 0xe0, 0xf0, 0xe0, 0xe0, 0xe0, 0xff, 0xff, 0xff, 0xf0, 0xe0, 
0xc0, 0x80, 0x80, 0xc3, 0xc0, 0xc0, 0xf0, 0xff, 0xff, 0xff, 0xc1, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 
0xc2, 0xc3, 0xc3, 0xff, 0xff, 0xff, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xff, 
0xff, 0xff, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xf8, 0xf8, 
0xf8, 0xf8, 0xf8, 0xf8, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xe0, 0xe0, 0xe0, 0xe0, 0xf0, 0xe0, 0xe0, 0xe0, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 
0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };   


    switch (get_highest_layer(layer_state)) {
        case 0:
            // Vykreslení obrázku na OLED pro vrstvu 0
            oled_write_raw_P(image1, sizeof(image1));
            break;
        case 1:
            // Vykreslení obrázku na OLED pro vrstvu 1
            oled_write_raw_P(image2, sizeof(image2));
            break;
        case 2:
            // Vykreslení obrázku na OLED pro vrstvu 2
            oled_write_raw_P(image3, sizeof(image3));
            break;

         case 3:
         oled_write_raw_P(image4, sizeof(image4));
            
            break;

        default:
            // Zobrazení textu "Undefined" pro neznámé vrstvy
         oled_write_raw_P(image3, sizeof(image3));
           
    }
    
    return false;
}
#endif