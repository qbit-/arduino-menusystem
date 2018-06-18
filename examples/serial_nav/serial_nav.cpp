/*
 * serial_nav.cpp - Example code using the menu system library
 * this is a mbed port from the original serial_nav.ino file 
 *
 * This example shows the menu system being controlled over the serial port.
 *
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <mbed.h>
#include <stdio.h>
#include <string.h>
using namespace std;

// Serial terminal
Serial pc(USBTX, USBRX);

#include <MenuSystem.h>
#include "CustomNumericMenuItem.h"
#include "MyRenderer.h"

// forward declarations
const string format_float(const float value);
const string format_int(const float value);
const string format_color(const float value);
void on_component_selected(MenuComponent* p_menu_component);

// Menu variables

MyRenderer my_renderer;
MenuSystem ms(my_renderer);

MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_component_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_component_selected);
Menu mu1("Level 1 - Item 3 (Menu)");
BackMenuItem mu1_mi0("Level 2 - Back (Item)", &on_component_selected, &ms);
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", &on_component_selected);
NumericMenuItem mu1_mi2("Level 2 - Txt Item 2 (Item)", nullptr, 0, 0, 2, 1, format_color);
CustomNumericMenuItem mu1_mi3(12, "Level 2 - Cust Item 3 (Item)", 80, 65, 121, 3, format_int);
NumericMenuItem mm_mi4("Level 1 - Float Item 4 (Item)", nullptr, 0.5, 0.0, 1.0, 0.1, format_float);
NumericMenuItem mm_mi5("Level 1 - Int Item 5 (Item)", nullptr, 50, -100, 100, 1, format_int);

// Menu callback function

// writes the (int) value of a float into a char buffer.
const string format_int(const float value) {
    return to_string((int) value);
}

// writes the value of a float into a char buffer.
const string format_float(const float value) {
    return to_string(value);
}

// writes the value of a float into a char buffer as predefined colors.
const string format_color(const float value) {
    string buffer;

    switch((int) value)
    {
        case 0:
            buffer += "Red";
            break;
        case 1:
            buffer += "Green";
            break;
        case 2:
            buffer += "Blue";
            break;
        default:
            buffer += "undef";
    }

    return buffer;
}

// In this example all menu items use the same callback.

void on_component_selected(MenuComponent* p_menu_component) {
    pc.printf("%s\n", p_menu_component->get_name());
}

void display_help() {
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
    if ((inChar = pc.getc()) > 0) {
        pc.printf("%s\n", "\033c");
        switch (inChar) {
            case 'w': // Previus item
                ms.prev();
                ms.display();
                pc.printf("%s\n", "");
                break;
            case 's': // Next item
                ms.next();
                ms.display();
                pc.printf("%s\n", "");
                break;
            case 'a': // Back presed
                ms.back();
                ms.display();
                pc.printf("%s\n", "");
                break;
            case 'd': // Select presed
                ms.select();
                ms.display();
                pc.printf("%s\n", "");
                break;
            case '?':
            case 'h': // Display help
                ms.display();
                pc.printf("%s\n", "");
                break;
            default:
                break;
        }
    }
}

// Standard arduino functions

void setup() {

    ms.get_root_menu().add_item(&mm_mi1);
    ms.get_root_menu().add_item(&mm_mi2);
    ms.get_root_menu().add_menu(&mu1);
    mu1.add_item(&mu1_mi0);
    mu1.add_item(&mu1_mi1);
    mu1.add_item(&mu1_mi2);
    mu1.add_item(&mu1_mi3);
    ms.get_root_menu().add_item(&mm_mi4);
    ms.get_root_menu().add_item(&mm_mi5);

    display_help();
    ms.display();
}

int main() {
  setup();
  serial_handler();
}
