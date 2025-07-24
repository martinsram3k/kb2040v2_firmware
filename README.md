⚙️ KB2040v2 Macropad with QMK Firmware ⌨️
This repository contains the firmware and configuration for my custom KB2040v2 macropad, designed for modularity and user-friendly configuration using QMK Firmware. This project uses the via keymap.

⬇️ How to Install and Use (QMK Firmware)
This project utilizes QMK Firmware. The installation process involves compiling the firmware and then flashing it to the macropad.

Prerequisites
QMK Firmware (installed and configured locally)

Git (for cloning the repository)

USB-C cable for connecting the macropad

Firmware Installation (Flashing)
Clone the Repository:
Open your terminal (or QMK MSYS on Windows) and clone this repository:

Bash

git clone https://github.com/martinsram3k/kb2040v2_firmware
cd kb2040v2_firmware # Navigate into the project directory
(Note: This repository should contain your keymap.c and other necessary QMK files for your kb2040v2 keyboard definition within a standard QMK firmware structure.)

Navigate to the QMK Firmware Root:
Compilation should be performed from the root directory of your QMK Firmware installation (e.g., qmk_firmware/). If you cloned this repository directly into the keyboards or keymaps directory of your QMK installation, you'll need to navigate up to the qmk_firmware root.

Bash

# Example if your project is located at qmk_firmware/keyboards/kb2040v2/keymaps/via
cd qmk_firmware/
Compile the Firmware:

Bash

qmk compile -kb kb2040v2 -km via
This command will compile the .uf2 firmware file. The compiled file will be located in your QMK firmware root directory (e.g., qmk_firmware/kb2040v2_via.uf2).

Enter Bootloader Mode:
Before flashing, your macropad needs to be in bootloader mode. There are several ways to achieve this for the KB2040v2:

Bootmagic (First Key on Plug-in): Press and hold the first key (top-left on your layout, HF_TOGG) while plugging in the USB-C cable.

Dedicated Boot Button: Press the physical BOOT button directly on the KB2040v2 board.

From Modifier Layer: If your macropad is already running this firmware, press the QK_BOOT key defined on the modifier layer ([3]).
Once in bootloader mode, the KB2040v2 will appear as a USB drive named RPI-RP2.

Flash the Firmware:
Drag and drop the compiled .uf2 file (e.g., kb2040v2_via.uf2) onto the RPI-RP2 USB drive. The macropad will automatically unmount and restart with the new firmware.

Macropad Configuration
Once the firmware is flashed, the macropad will function according to the defined layers and keymap in this repository.

Customizing Keybindings with VIA: This macropad is VIA-compatible, meaning you can easily customize keybindings, macros, and lighting (if applicable) using the VIA Configurator software. Simply open the VIA application, connect your macropad, and make your desired changes without needing to re-compile or re-flash the firmware.

Layers: The macropad supports 3 programmable base layers (0, 1, 2) and a special modifier layer (3).

Switching between base layers 0, 1, and 2 is done by tapping (quick press and release) the KC_CYCLE_LAYERS key.

Activating the modifier layer (3) is done by holding the KC_CYCLE_LAYERS key for 2 seconds.

Haptic Feedback: The macropad provides haptic feedback when switching layers.

Hardware Reset (Bootloader): The QK_BOOT key allows direct entry into the bootloader, useful for flashing new firmware.

🚀 Project Overview 🌐
This project focuses on the QMK firmware for the KB2040v2 macropad.

🤖 Firmware (QMK) 💡
File: keymap.c

Initialization (keyboard_post_init_user()):

Sets up the GPIO pin for the solenoid (haptic feedback).

Initializes the QMK haptic subsystem.

Ensures correct initial layer state and previous_base_layer tracking.

KC_CYCLE_LAYERS (Custom Keycode):

Defined as a special user keycode (QK_USER).

Its functionality is split: a quick tap cycles through layers 0, 1, and 2.

A long hold (2 seconds, configurable via HOLD_MODIFIER_LAYER_DELAY) activates the dedicated modifier layer (layer 3).

layer_state_set_user():

This function is called every time the layer state changes.

It automatically triggers a haptic pulse (haptic_play()) when a layer transition occurs, provided haptic feedback is enabled.

matrix_scan_user():

Continuously monitors the KC_CYCLE_LAYERS key for its hold duration.

If the key is held long enough, it activates layer 3, ensuring the modifier layer is engaged only after a deliberate long press.

process_record_user():

The core function for handling key presses and releases.

Manages the complex logic for KC_CYCLE_LAYERS, differentiating between a "tap" (for layer cycling) and a "hold" (for activating the modifier layer).

Handles other custom keycodes like KC_HAPTIC_ON/OFF (though these are not directly on the provided keymap, their processing logic remains).

Keymaps (keymaps[][MATRIX_ROWS][MATRIX_COLS]):

Layer 0 (Base): Your primary layer, e.g., KC_A.

Layer 1: A secondary programmable layer, e.g., KC_B.

Layer 2: A third programmable layer, e.g., KC_C.

Layer 3 (Modifiers): This layer is accessible via a long hold of KC_CYCLE_LAYERS and contains specific utility functions:

HF_TOGG: Toggle haptic feedback on/off. (If these are custom haptic keycodes, ensure they are defined in your rules.mk and config.h).

HF_DWLU, HF_DWLD: Adjust haptic feedback intensity (Down/Up).

KC_CYCLE_LAYERS: Allows you to return to the base layer cycle even from the modifier layer (e.g., by tapping it again).

QK_BOOT: A QMK built-in keycode to instantly enter the bootloader mode.

OLED Display (oled_task_user()):

Purpose: The integrated OLED display serves as a dynamic visual indicator, providing essential feedback about the macropad's current state.

Functionality:

Layer Indication: When operating on base layers (0, 1, 2), the OLED displays custom bitmap images (image1, image2, image3) corresponding to the active layer. This offers immediate and intuitive recognition of the current keymap.

Modifier Layer Status: When the modifier layer (layer 3) is active, the OLED switches to textual information, clearly showing "modifikator" and the current status of haptic feedback ("Haptic: ON/OFF"). This provides quick insight into the special functions enabled on this layer.

Hardware Functions
The firmware interacts with the following hardware components:

Keyboard Matrix: Reads key presses from the 3x3 matrix.

Solenoid/Haptic Motor: Drives a solenoid (or haptic motor) to provide tactile feedback, primarily during layer changes. This enhances the user experience by confirming layer transitions.

OLED Display: Utilizes an I2C OLED display to provide visual information to the user, such as the active layer and special function statuses.

Software Functions
The firmware provides the following software functionalities:

Layer Management: Robust layer switching system with both tap (for cycling base layers) and hold (for dedicated modifier layer) functionality on a single key.

Haptic Feedback Control: Provides real-time haptic feedback based on layer changes and offers control over haptic status.

Bootloader Access: Allows the user to enter the bootloader mode directly from the keyboard, simplifying firmware updates.

VIA Compatibility: The firmware is configured to be compatible with VIA software, enabling easy graphical customization of key bindings, macros, and other settings without re-flashing.

🖼️ Photo Gallery 📸
Here you can insert your photos of the macropad.

Snímek obrazovky 2025-07-22 v 18.16.40.png

Snímek obrazovky 2025-07-22 v 22.15.51.png

Snímek obrazovky 2025-07-22 v 22.16.19.png

Snímek obrazovky 2025-07-22 v 22.40.58.png

Snímek obrazovky 2025-07-22 v 22.43.16.png

Snímek obrazovky 2025-07-23 v 20.24.58.jpg

⚙️ Macropad Description 🛠️
This custom-designed macropad is a versatile tool aimed at enhancing productivity and streamlining workflows. Here's a breakdown of its key features:

Microcontroller: Powered by the robust KB2040 microcontroller (based on the Raspberry Pi RP2040), ensuring reliable performance and ample processing power. 🧠

Case: The macropad's enclosure is 3D-printed using durable PLA filament, providing a sturdy and customizable form factor. 🖨️

PCB: Features a custom-designed Printed Circuit Board (PCB), meticulously crafted for optimal functionality and efficiency. ⚡

Keys: Equipped with 9 programable keys, allowing for a wide range of custom functions and macros. 🔑

Layer Key: Includes a dedicated 10th key for seamless layer switching (KC_CYCLE_LAYERS), enabling access to up to 3 distinct base layers of programable functions, and also a special modifier layer activated by a long press. 🔘

Layers: The macropad supports 3 programable base layers (0, 1, 2) and one dedicated modifier layer (3), effectively quadrupling the number of available custom functions. 📂

OLED Display: An integrated OLED display provides crucial visual feedback, showing the currently active layer and status information. 📊

Haptic Feedback: An integrated solenoid provides tactile feedback during layer transitions and for other specific actions, enhancing the user experience with physical confirmation. 📳

Multimedia Control: The macropad can be configured to control multimedia functions, such as volume adjustment, play/pause, and stop. 🎵

Quick Access: The macropad can be configured to open applications directly, like a calculator. 🔢

This macropad is ideal for professionals and enthusiasts who frequently use complex keyboard shortcuts and seek to enhance their workflow efficiency. Specifically, it caters to:

Photographers: Streamline photo editing processes with quick access to commonly used tools and adjustments. 📸

Video Editors: Accelerate video editing tasks by assigning intricate editing commands to easily accessible keys. 🎬

Graphic Designers: Simplify design workflows with one-touch access to frequently used design software functions. 🎨

Anyone needing complex keyboard shortcuts: In general, anyone who wants to speed up computer work. 🚀

People who want to control multimedia: Easy multimedia control. 🎵

People who want to have quick access to applications: Quick access to applications like calculator. 🔢

Essentially, this macropad is designed for anyone looking to boost productivity and minimize repetitive keystrokes, ultimately leading to a more efficient and enjoyable computing experience. 😃

☕ Support the Project 💖
If you find this project helpful and would like to support its development, you can do so through the following platforms:

Buy Me a Coffee: Buy Me a Coffee Link ☕

PayPal.me: PayPal.me Link 💰
