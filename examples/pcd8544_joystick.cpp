#include <mbed.h>
#include "Joystick.h"
#include <Adafruit_PCD8544.h>
#include <Adafruit_GFX.h>
#include <MenuSystem.h>
#include "stdio.h"

// Vertical (y), Horizontal (x), Button
Joystick ctrl(p15, p16, p29); 

// Attach pins to RST, SCE, DC, DN(MOSI), SCLK(SCK)
Adafruit_PCD8544 lcd(p11, p12, p8, p5, p7);
PwmOut backlight(p26);
#define PCD8544_CHAR_HEIGHT 8

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
    wait_ms(400); // so we can look the result on the LCD
}

void on_item2_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0, 2 * PCD8544_CHAR_HEIGHT);
    lcd.puts("Item2 Selectd");
    lcd.display();
    wait_ms(400); // so we can look the result on the LCD
}

void on_item3_selected(MenuComponent* p_menu_component) {
    lcd.setCursor(0, 2 * PCD8544_CHAR_HEIGHT);
    lcd.puts("Item3 Selectd");
    lcd.display();
    wait_ms(400); // so we can look the result on the LCD
}

// Menu variables

MenuSystem ms(my_renderer);
MenuItem mm_mi1("Lvl1-Item1(I)", &on_item1_selected);
MenuItem mm_mi2("Lvl1-Item2(I)", &on_item2_selected);
Menu mu1("Lvl1-Item3(M)");
MenuItem mu1_mi1("Lvl2-Item1(I)", &on_item3_selected);

void joystick_handler(Joystick & ctrl) {
  uint8_t dir;
  dir = ctrl.get_direction();
  switch (dir) {
  case 5: // Previus item
    ms.prev();
    ms.display();
    break;
  case 1: // Next item
    ms.next();
    ms.display();
    break;
  case 7: // Back pressed
    ms.back();
    ms.display();
    break;
  case 3: // Select pressed
    ms.select();
    ms.display();
    break;
  default:
    break;
  }
}

void setup() {
  ctrl.init();
  backlight = 1;
  
  lcd.begin();
  lcd.setContrast(60);
  lcd.clearDisplay();

  // Build the menu
  ms.get_root_menu().add_item(&mm_mi1);
  ms.get_root_menu().add_item(&mm_mi2);
  ms.get_root_menu().add_menu(&mu1);
  mu1.add_item(&mu1_mi1);
  ms.display();
};

int main() {
  
  setup();
  while(true) {
    joystick_handler(ctrl);
    wait_ms(300);
  }
}
