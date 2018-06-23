/*
 * current_item.cpp - Example code using the menu system library.
 * ported to mbed from the original current_item.ino file
 *
 * This example shows only one (the current) menu item at a time.
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <mbed.h>
#include <stdio.h>
#include <MenuSystem.h>

// Serial terminal
Serial pc(USBTX, USBRX);

// renderer

class MyRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
      menu.get_current_component()->render(*this);
    }

    void render_menu_item(MenuItem const& menu_item) const {
      pc.printf("%s\n", menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
      pc.printf("%s\n", menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
      pc.printf("%s\n", menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
      pc.printf("%s\n", menu.get_name());
    }
};
MyRenderer my_renderer;

// forward declarations

void on_item1_selected(MenuComponent* p_menu_component);
void on_item2_selected(MenuComponent* p_menu_component);
void on_item3_selected(MenuComponent* p_menu_component);
void on_item4_selected(MenuComponent* p_menu_component);

// Menu variables

MenuSystem ms(my_renderer);

MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_item1_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_item2_selected);
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", &on_item3_selected);
MenuItem mu1_mi2("Level 2 - Item 2 (Item)", &on_item4_selected);

// Menu callback function

bool bRanCallback = false;

void on_item1_selected(MenuComponent* p_menu_component) {
  pc.printf("%s\n", "Item1 Selected");
  bRanCallback = true;
}

void on_item2_selected(MenuComponent* p_menu_component) {
  pc.printf("%s\n", "Item2 Selected");
  bRanCallback = true;
}

void on_item3_selected(MenuComponent* p_menu_component) {
  pc.printf("%s\n", "Item3 Selected");
  bRanCallback = true;
}

void on_item4_selected(MenuComponent* p_menu_component) {
    pc.printf("%s\n","Item4 Selected");
    bRanCallback = false;
    ms.reset();
}

// Standard arduino functions

void setup() {
  ms.get_root_menu().add_item(&mm_mi1);
  ms.get_root_menu().add_item(&mm_mi2);
  ms.get_root_menu().add_menu(&mu1);
  mu1.add_item(&mu1_mi1);
  mu1.add_item(&mu1_mi2);
}

int main() {
  setup();
  while(true){
    ms.display();
    ms.select();
    if (bRanCallback) {
      ms.next();
      bRanCallback = false;
    }
    wait_ms(2000);
  }
}
