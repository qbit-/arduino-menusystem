/*
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include "MenuSystem.h"
#include <stdlib.h>
#include <string> 
using namespace std;


// *********************************************************
// MenuComponent
// *********************************************************

MenuComponent::MenuComponent(const char* name, ComponentCbPtr on_activate, ComponentCbPtr on_current)
: _name(name),
  _is_active(false),
  _is_current(false),
  _on_activate(on_activate),
  _on_current(on_current),
  _p_parent(nullptr){
}

const char* MenuComponent::get_name() const {
    return _name;
}

void MenuComponent::set_name(const char* name) {
    _name = name;
}

bool MenuComponent::is_active() const {
    return _is_active;
}

void MenuComponent::set_active(bool is_active) {
  _is_active = is_active;
}

bool MenuComponent::is_current() const {
    return _is_current;
}

void MenuComponent::set_current(bool is_current) {
    _is_current = is_current;
    if (_is_current && _on_current != nullptr)
      _on_current(this);
}

Menu* MenuComponent::activate() {
    if (_on_activate != nullptr)
        _on_activate(this);

    return nullptr;
}

void MenuComponent::set_on_activate_cb(ComponentCbPtr on_activate) {
    _on_activate = on_activate;
}

void MenuComponent::set_on_current_cb(ComponentCbPtr on_current) {
    _on_current = on_current;
}

Menu const* MenuComponent::get_parent(){
    return _p_parent;
}

void MenuComponent::set_parent(Menu* p_parent) {
    _p_parent = p_parent;
}

// *********************************************************
// Menu
// *********************************************************

Menu::Menu(const char* name, ComponentCbPtr on_activate, ComponentCbPtr on_current)
  : MenuComponent(name, on_activate, on_current),
  _p_current_component(nullptr),
  _menu_components(nullptr),
  _num_components(0),
  _current_component_num(0),
  _previous_component_num(0) {
}

bool Menu::next(bool loop) {
    _previous_component_num = _current_component_num;

    if (!_num_components) {
        return false;
    } else if (_current_component_num != _num_components - 1) {
        _current_component_num++;
        _p_current_component = _menu_components[_current_component_num];

        _p_current_component->set_current();
        _menu_components[_previous_component_num]->set_current(false);
        return true;
    } else if (loop) {
        _current_component_num = 0;
        _p_current_component = _menu_components[_current_component_num];

        _p_current_component->set_current();
        _menu_components[_previous_component_num]->set_current(false);

        return true;
    }
    return false;
}

bool Menu::prev(bool loop) {
    _previous_component_num = _current_component_num;

    if (!_num_components) {
        return false;
    } else if (_current_component_num != 0) {
        _current_component_num--;
        _p_current_component = _menu_components[_current_component_num];

        _p_current_component->set_current();
        _menu_components[_previous_component_num]->set_current(false);

        return true;
    } else if (loop) {
        _current_component_num = _num_components - 1;
        _p_current_component = _menu_components[_current_component_num];

        _p_current_component->set_current();
        _menu_components[_previous_component_num]->set_current(false);

        return true;
    }
    return false;
}

Menu* Menu::activate_menucomponent() {
    if (!_num_components)
        return nullptr;

    MenuComponent* pComponent = _menu_components[_current_component_num];

    if (pComponent == nullptr)
        return nullptr;

    return pComponent->activate();
}

Menu* Menu::activate() {
    MenuComponent::activate();
    this->set_active(true);
    _p_current_component = _num_components ? _menu_components[0] : nullptr;
    if (_p_current_component)
      _p_current_component->set_current();
    return this;
}

void Menu::reset() {
  // Makes first menuitem current
  if(_p_current_component){
    _p_current_component->set_current(false);
    _p_current_component->set_active(false);
  }
  _previous_component_num = 0;
  _current_component_num = 0;
  _p_current_component = _num_components ? _menu_components[0] : nullptr;
  if (this->is_active() && _p_current_component){
    _p_current_component->set_current();
  }
}

// }

void Menu::add(MenuComponent* p_component) {
    // Resize menu component list, keeping existing items.
    // If it fails, then the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                (_num_components + 1)
                                                * sizeof(MenuComponent*));
    if (_menu_components == nullptr)
      return;

    _menu_components[_num_components] = p_component;

    // if (_num_components == 0) {
    //   _p_current_component = p_component;
    //   _p_current_component->_is_current = true;
    // }

    _num_components++;
    p_component->set_parent(this);
}

MenuComponent const* Menu::get_menu_component(uint8_t index) const {
    return _menu_components[index];
}

MenuComponent const* Menu::get_current_component() const {
    return _p_current_component;
}

uint8_t Menu::get_num_components() const {
    return _num_components;
}

uint8_t Menu::get_current_component_num() const {
    return _current_component_num;
}

uint8_t Menu::get_previous_component_num() const {
    return _previous_component_num;
}

void Menu::render(MenuComponentRenderer const& renderer) const {
    renderer.render(*this);
}

// *********************************************************
// BackMenuItem
// *********************************************************

BackMenuItem::BackMenuItem(const char* name, MenuSystem* ms, ComponentCbPtr on_activate, ComponentCbPtr on_current)
  : MenuItem(name, on_activate, on_current),
  _menu_system(ms) {
}

Menu* BackMenuItem::activate() {
    if (_on_activate!=nullptr)
        _on_activate(this);

    if (_menu_system!=nullptr)
        _menu_system->back();

    return nullptr;
}

void BackMenuItem::render(MenuComponentRenderer const& renderer) const {
    renderer.render(*this);
}

// *********************************************************
// MenuItem
// *********************************************************

MenuItem::MenuItem(const char* name, ComponentCbPtr on_activate, ComponentCbPtr on_current)
  : MenuComponent(name, on_activate, on_current) {
}

Menu* MenuItem::activate() {
  // call attached function
  MenuComponent::activate();
  return nullptr;
}

void MenuItem::reset() {
    // Do nothing.
}

void MenuItem::render(MenuComponentRenderer const& renderer) const {
    renderer.render(*this);
}

bool MenuItem::next(bool loop) {
    return false;
}

bool MenuItem::prev(bool loop) {
    return false;
}

// *********************************************************
// NumericMenuItem
// *********************************************************

NumericMenuItem::NumericMenuItem(
   const char* basename,
   float value, float min_value, float max_value,
   float increment,
   ComponentCbPtr on_activate,
   ComponentCbPtr on_current,
   ValueCbPtr format_value_fn): MenuItem(basename, on_activate, on_current),
				      _value(value),
				      _min_value(min_value),
				      _max_value(max_value),
				      _increment(increment),
				      _format_value_fn(format_value_fn){
    if (_increment < 0.0) _increment = -_increment;
    if (_min_value > _max_value) {
        float tmp = _max_value;
        _max_value = _min_value;
        _min_value = tmp;
    }
};

void NumericMenuItem::set_number_formatter(
  ValueCbPtr format_value_fn){
  _format_value_fn = format_value_fn;
}

Menu* NumericMenuItem::activate() {
    _is_active = !_is_active;

    // Only run _on_activate when the user is done editing the value
    if (!_is_active && _on_activate != nullptr)
        _on_activate(this);
    return nullptr;
}

void NumericMenuItem::render(
    MenuComponentRenderer const& renderer) const {
    renderer.render(*this);
}

float NumericMenuItem::get_value() const {
    return _value;
}

float NumericMenuItem::get_min_value() const {
    return _min_value;
}

float NumericMenuItem::get_max_value() const {
    return _max_value;
}

string NumericMenuItem::get_formatted_value() const {
    string buffer;
    if (_format_value_fn != nullptr)
        buffer += _format_value_fn(_value);
    else
        buffer += _value;
    return buffer;
}

void NumericMenuItem::set_value(float value) {
    _value = value;
}

void NumericMenuItem::set_min_value(float value) {
    _min_value = value;
}

void NumericMenuItem::set_max_value(float value) {
    _max_value = value;
}

bool NumericMenuItem::next(bool loop) {
    _value += _increment;
    if (_value > _max_value) {
        if (loop)
            _value = _min_value;
        else
            _value = _max_value;
    }
    return true;
}

bool NumericMenuItem::prev(bool loop) {
    _value -= _increment;
    if (_value < _min_value) {
        if (loop)
            _value = _max_value;
        else
            _value = _min_value;
    }
    return true;
}

// *********************************************************
// MenuSystem
// *********************************************************

MenuSystem::MenuSystem(
  MenuComponentRenderer const& renderer, const char* name):
  _p_root_menu(new Menu(name, nullptr)),
  _p_current_menu(_p_root_menu),
  _renderer(renderer) {
  _p_root_menu->set_current(true);
  _p_root_menu->set_active(true);
}

bool MenuSystem::next(bool loop) {
    if (_p_current_menu->_p_current_component->is_active())
        return _p_current_menu->_p_current_component->next(loop);
    else
        return _p_current_menu->next(loop);
}

bool MenuSystem::prev(bool loop) {
    if (_p_current_menu->_p_current_component->is_active())
        return _p_current_menu->_p_current_component->prev(loop);
    else
        return _p_current_menu->prev(loop);
}

void MenuSystem::reset() {
  // go to root menu
  _p_current_menu->set_active(false);
  _p_current_menu->reset();
  _p_current_menu = _p_root_menu;
  _p_root_menu->set_active(true);
  _p_root_menu->reset();
}

void MenuSystem::activate() {
    Menu* pMenu = _p_current_menu->activate_menucomponent();

    if (pMenu != nullptr)
        _p_current_menu = pMenu;
}

bool MenuSystem::back() {
  // Deactivate current component if it has focus
  if (_p_current_menu->_p_current_component->is_active()){
    _p_current_menu->_p_current_component->set_active(false);
    return true;
  }
  // Go 1 level up if no component was active
  // and reset current menu
  if (_p_current_menu != _p_root_menu){
    // reset the items in the current menu and deactivate it
    _p_current_menu->set_active(false);
    _p_current_menu->reset();
    // activate the parent menu
    _p_current_menu = const_cast<Menu *>(_p_current_menu->get_parent());
    _p_current_menu->set_active(true);
    return true;
  }

  // We are already in the root menu
  return false;
}

Menu& MenuSystem::get_root_menu() const {
    return *_p_root_menu;
}

Menu const* MenuSystem::get_current_menu() const {
    return _p_current_menu;
}

void MenuSystem::display() const {
  if (_p_current_menu != nullptr){
    _renderer.render(*_p_current_menu);
  }
  else{
    _renderer.render(*_p_root_menu);
  }
}
