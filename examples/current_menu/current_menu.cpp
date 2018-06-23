/*
 * current_menu.cpp - Example code using the menu system library.
 * ported to mbed from the original current_item.ino file
 *
 * This example shows all items in the current menu.
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
    pc.printf("%s\n", "");
    for (int i = 0; i < menu.get_num_components(); ++i) {
      MenuComponent const* cp_m_comp = menu.get_menu_component(i);
      cp_m_comp->render(*this);
      
      if (cp_m_comp->is_current())
	 pc.printf("%s\n", "<<< ");
      pc.printf("%s\n", "");
    }
  }
  
  void render_menu_item(MenuItem const& menu_item) const {
    pc.printf("%s", menu_item.get_name());
  }
  
  void render_back_menu_item(BackMenuItem const& menu_item) const {
    pc.printf("%s", menu_item.get_name());
  }
  
  void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
    pc.printf("%s", menu_item.get_name());
  }
  
  void render_menu(Menu const& menu) const {
    pc.printf("%s", menu.get_name());
  }
};
MyRenderer my_renderer;

// forward declarations

void on_item1_selected(MenuComponent* p_menu_component);
void on_item2_selected(MenuComponent* p_menu_component);
void on_item3_selected(MenuComponent* p_menu_component);

// Menu variables

MenuSystem ms(my_renderer);

MenuItem mm_mi1("Level 1 - Item 1 (Item)", &on_item1_selected);
MenuItem mm_mi2("Level 1 - Item 2 (Item)", &on_item2_selected);
Menu mu1("Level 1 - Item 3 (Menu)");
MenuItem mu1_mi1("Level 2 - Item 1 (Item)", &on_item3_selected);

// Menu callback function

bool done = false;

void on_item1_selected(MenuComponent* p_menu_component) {
  pc.printf("%s\n", "Item1 Selected");
}

void on_item2_selected(MenuComponent* p_menu_component) {
  pc.printf("%s\n", "Item2 Selected");
}

void on_item3_selected(MenuComponent* p_menu_component) {
  pc.printf("%s\n", "Item3 Selected");
  done = true;
}

// Standard arduino functions

void setup() {
  ms.get_root_menu().add_item(&mm_mi1);
  ms.get_root_menu().add_item(&mm_mi2);
  ms.get_root_menu().add_menu(&mu1);
  mu1.add_item(&mu1_mi1);
}

int main() {
  setup();
  while(true){
    ms.display();
  
    // Simulate using the menu by walking over the entire structure.
    ms.select();
    ms.next();
  
    if (done) {
      ms.reset();
      done = false;
    }
  
    wait_ms(2000);
  }
}
