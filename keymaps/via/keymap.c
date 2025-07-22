#include QMK_KEYBOARD_H

// Tyto hlavičkové soubory jsou potřeba pro low-level GPIO funkce na RP2040.
#include "hardware/gpio.h"

// Hlavičkový soubor pro haptický ovladač QMK
#include "haptic.h" // Pro funkci haptic_play()

// Definice vlastního keycode. QK_USER zajistí, že se nepřekrývá s existujícími keycody QMK.
enum keycodes {  
  KC_CYCLE_LAYERS = QK_USER,
}; 

// První vrstva v cyklu (vrstva 0)
#define LAYER_CYCLE_START 0 
// Poslední vrstva v cyklu (vrstva 3, což znamená vrstvy 0, 1, 2, 3)
#define LAYER_CYCLE_END   3 

// Statická proměnná pro uložení předchozího stavu vrstev.
// Používá se pro detekci změny vrstvy.
static uint32_t last_layer_state = 0;

// Funkce pro ovládání OLED displeje (pokud je povolen OLED_ENABLE v config.h)
#ifdef OLED_ENABLE
bool oled_task_user(void) {
  if (oled_on()) {
    // Zde můžeš přidat vlastní logiku pro zobrazení textu nebo vrstvy na OLED.
    oled_write_P(PSTR("Hello World"), false);
  }
  return false;
}
#endif // OLED_ENABLE

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
}

// Tato funkce se volá v každém cyklu skenování matice, ale pro haptiku ji nepoužíváme.
void matrix_scan_user(void) {
    // Žádná automatická pulzace ani haptická logika zde.
}

// Tato funkce se volá POKAZDÉ, kdy dojde ke ZMĚNĚ stavu vrstev.
// To je ideální místo pro spuštění haptiky POUZE při změně vrstvy.
layer_state_t layer_state_set_user(layer_state_t state) {
    // Zavolejte původní funkci QMK pro zpracování stavu vrstev, je to důležité.
    state = default_layer_state_set_user(state);

    // Zkontrolujeme, zda se aktuální stav vrstev (po default_layer_state_set_user) liší od předchozího stavu.
    if (state != last_layer_state) {
        // Pokud došlo ke změně vrstvy, spustíme haptiku.
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
            // Naše logika pro cyklování vrstev se spustí pouze při stisku klávesy.
            if (!record->event.pressed) { 
                return false; // Při uvolnění klávesy nic neděláme.
            }
            // Zde JIŽ NEBUDEME VOLAT haptic_play(), protože haptika se spustí v layer_state_set_user()
            // až poté, co se skutečně změní vrstva.
            
            // Získáme nejvyšší aktivní vrstvu.
            uint8_t current_layer = get_highest_layer(layer_state); 
            
            // Zkontrolujeme, zda jsme v definovaném rozsahu cyklování (0-3).
            if (current_layer > LAYER_CYCLE_END || current_layer < LAYER_CYCLE_START) { 
                return false; 
            }
            
            // Vypočítáme další vrstvu v cyklu.
            uint8_t next_layer = current_layer + 1; 
            // Pokud jsme na konci cyklu (vrstva 3), vrátíme se na začátek (vrstvu 0).
            if (next_layer > LAYER_CYCLE_END) { 
                next_layer = LAYER_CYCLE_START; 
            }
            
            // Přesuneme klávesnici na vypočítanou další vrstvu.
            // Tato funkce způsobí volání layer_state_set_user(), kde se haptika spustí.
            layer_move(next_layer); 
            return false; // Klávesa byla zpracována, QMK ji už dál zpracovávat nemusí.

        // Zpracování pro VŠECHNY OSTATNÍ keycody.
        default:
            // Vracíme true, aby QMK zpracoval klávesu normálně (tj. odeslal ji do počítače).
            // Zde NIKDY nevoláme haptic_play(), takže se haptika NEspustí pro žádné jiné klávesy.
            return true; 
    }
}

// Definice layoutů klávesnice pro jednotlivé vrstvy.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Vrstva 0 - Základní vrstva
    [0] = LAYOUT_martin_3x3(
        KC_A, KC_B, KC_C, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_D, KC_E, KC_F, KC_G,
        KC_H, KC_I, KC_NO                  // KC_NO = prázdná/neaktivní klávesa
    ),

    // Vrstva 1 - Číselná/funkční vrstva
    [1] = LAYOUT_martin_3x3(
        KC_B, KC_KP_2, KC_KP_3, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_NO, KC_NO, KC_NO, KC_NO,
        KC_NO, KC_NO, KC_NO                      
    ),

    // Vrstva 2 - Transparentní/prázdná vrstva (propouští klávesy z nižších vrstev, pokud nejsou definovány zde)
    [2] = LAYOUT_martin_3x3(
        KC_C, KC_TRNS, KC_TRNS, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS                  // KC_TRNS = transparentní klávesa
    ),

    // Vrstva 3 - Další transparentní/prázdná vrstva
    [3] = LAYOUT_martin_3x3(
        KC_D, KC_TRNS, KC_TRNS, KC_CYCLE_LAYERS, // Klávesa pro cyklování vrstev
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS
    )
};