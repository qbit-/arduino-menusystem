/*
 * pcd8544_nav.cpp - Example code using the menu system library
 * this is a mbed port of the original pcd8544_nav.ino file 
 *
 * This example shows using the menu system with a pcd8544 LCD display
 * (controlled over serial).
 *
 * Copyright (c) 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <mbed.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <MenuSystem.h>

// LCD

// Attach pins to DC, CS, RST, MOSI, SCLK
Adafruit_PCD8544 lcd(p8, p12, p11, p5, p7);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define PCD8544_CHAR_HEIGHT 8

//Serial channel
Serial pc(USBTX, USBRX);

//Backlight

PwmOut backlight(p26);

// Renderer

class MyRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
        lcd.clearDisplay();
        menu.render(*this);
        menu.get_current_component()->render(*this);
        lcd.display();
    }

    void render_menu_item(MenuItem const& menu_item) const {
        lcd.setCursor(0, 1 * PCD8544_CHAR_HEIGHT);
        lcd.puts(menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        lcd.setCursor(0, 1 * PCD8544_CHAR_HEIGHT);
        lcd.puts(menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        lcd.setCursor(0, 1 * PCD8544_CHAR_HEIGHT);
        lcd.puts(menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
        lcd.setCursor(0, 0 * PCD8544_CHAR_HEIGHT);
        lcd.puts(menu.get_name());
    }
};
MyRenderer my_renderer;

// Menu callback function

void on_item1_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0, 2 * PCD8544_CHAR_HEIGHT);
    lcd.puts("Item1 Selectd");
    lcd.display();
    wait_ms(150); // so we can look the result on the LCD
}

void on_item2_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0, 2 * PCD8544_CHAR_HEIGHT);
    lcd.puts("Item2 Selectd");
    lcd.display();
    wait_ms(150); // so we can look the result on the LCD
}

void on_item3_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0, 2 * PCD8544_CHAR_HEIGHT);
    lcd.puts("Item3 Selectd");
    lcd.display();
    wait_ms(150); // so we can look the result on the LCD
}


// Menu variables

MenuSystem ms(my_renderer);
MenuItem mm_mi1("Lvl1-Item1(I)", &on_item1_selected);
MenuItem mm_mi2("Lvl1-Item2(I)", &on_item2_selected);
Menu mu1("Lvl1-Item3(M)");
MenuItem mu1_mi1("Lvl2-Item1(I)", &on_item3_selected);

void serial_print_help() {
    pc.printf("%s\n", "***************");
    pc.printf("%s\n", "w: go to previus item (up)");
    pc.printf("%s\n", "s: go to next item (down)");
    pc.printf("%s\n", "a: go back (right)");
    pc.printf("%s\n", "d: select \"selected\" item");
    pc.printf("%s\n", "?: print this help");
    pc.printf("%s\n", "h: print this help");
    pc.printf("%s\n", "***************");
}

void serial_handler() {
    char inChar;
    if((inChar = pc.getc())>0) {
        switch (inChar) {
            case 'w': // Previus item
                ms.prev();
                ms.display();
                break;
            case 's': // Next item
                ms.next();
                ms.display();
                break;
            case 'a': // Back pressed
                ms.back();
                ms.display();
                break;
            case 'd': // Select pressed
                ms.select();
                ms.display();
                break;
            case '?':
            case 'h': // Display help
                serial_print_help();
                break;
            default:
                break;
        }
    }
}

// Standard arduino functions

void setup() {
  lcd.begin();
  lcd.clearDisplay();
  lcd.setContrast(50);

#ifdef HIDE_SPLASH
    lcd.clearDisplay();
#else
    lcd.display();
    wait_ms(2000);
#endif

    backlight = 1;
    serial_print_help();

    ms.get_root_menu().add_item(&mm_mi1);
    ms.get_root_menu().add_item(&mm_mi2);
    ms.get_root_menu().add_menu(&mu1);
    mu1.add_item(&mu1_mi1);
    ms.display();
}

int main() {
  setup();
  while(true){
    serial_handler();
  }
}
