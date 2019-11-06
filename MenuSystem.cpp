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

MenuComponent::MenuComponent(const char* name, SelectFnPtr select_fn)
: _name(name),
  _has_focus(false),
  _is_current(false),
  _select_fn(select_fn) {
}

const char* MenuComponent::get_name() const {
    return _name;
}

void MenuComponent::set_name(const char* name) {
    _name = name;
}

bool MenuComponent::has_focus() const {
    return _has_focus;
}

void MenuComponent::set_focus(bool has_focus) {
  _has_focus = has_focus;
}

bool MenuComponent::is_current() const {
    return _is_current;
}

void MenuComponent::set_current(bool is_current) {
    _is_current = is_current;
}

Menu* MenuComponent::select() {
    if (_select_fn != nullptr)
        _select_fn(this);

    return nullptr;
}

void MenuComponent::set_select_function(SelectFnPtr select_fn) {
    _select_fn = select_fn;
}

// *********************************************************
// Menu
// *********************************************************

Menu::Menu(const char* name, SelectFnPtr select_fn)
: MenuComponent(name, select_fn),
  _p_current_component(nullptr),
  _menu_components(nullptr),
  _p_parent(nullptr),
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

    return pComponent->select();
}

Menu* Menu::select() {
    MenuComponent::select();
    this->set_focus(true);
    return this;
}

void Menu::reset() {
  // Makes first menuitem current
  _p_current_component->set_current(false);
  _p_current_component->set_focus(false);
  _previous_component_num = 0;
  _current_component_num = 0;
  _p_current_component = _num_components ? _menu_components[0] : nullptr;
  _p_current_component->set_current();
}

void Menu::add_item(MenuItem* p_item) {
    add_component((MenuComponent*) p_item);
    p_item->set_parent(this);
}

void Menu::add_menu(Menu* p_menu) {
    add_component((MenuComponent*) p_menu);
    p_menu->set_parent(this);
}

void Menu::add_component(MenuComponent* p_component) {
    // Resize menu component list, keeping existing items.
    // If it fails, then the item is not added and the function returns.
    _menu_components = (MenuComponent**) realloc(_menu_components,
                                                (_num_components + 1)
                                                * sizeof(MenuComponent*));
    if (_menu_components == nullptr)
      return;

    _menu_components[_num_components] = p_component;

    if (_num_components == 0) {
        _p_current_component = p_component;
        _p_current_component->set_current();
    }

    _num_components++;
}

Menu const* Menu::get_parent() const {
    return _p_parent;
}

void Menu::set_parent(Menu* p_parent) {
    _p_parent = p_parent;
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

BackMenuItem::BackMenuItem(const char* name, SelectFnPtr select_fn,
                           MenuSystem* ms)
: MenuItem(name, select_fn),
  _menu_system(ms) {
}

Menu* BackMenuItem::select() {
    if (_select_fn!=nullptr)
        _select_fn(this);

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

MenuItem::MenuItem(const char* name, SelectFnPtr select_fn)
: MenuComponent(name, select_fn) {
}

Menu* MenuItem::select() {
  // call attached function
  MenuComponent::select();
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

Menu const* MenuItem::get_parent() const {
    return _p_parent;
}

void MenuItem::set_parent(Menu* p_parent) {
    _p_parent = p_parent;
}

// *********************************************************
// NumericMenuItem
// *********************************************************

NumericMenuItem::NumericMenuItem(
   const char* basename, SelectFnPtr select_fn,
   float value, float min_value, float max_value,
   float increment,
   FormatValueFnPtr format_value_fn): MenuItem(basename, select_fn),
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
  FormatValueFnPtr format_value_fn){
  _format_value_fn = format_value_fn;
}

Menu* NumericMenuItem::select() {
    _has_focus = !_has_focus;

    // Only run _select_fn when the user is done editing the value
    if (!_has_focus && _select_fn != nullptr)
        _select_fn(this);
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
  _p_curr_menu(_p_root_menu),
  _renderer(renderer) {
  _p_root_menu->set_current(true);
  _p_root_menu->set_focus(true);
}

bool MenuSystem::next(bool loop) {
    if (_p_curr_menu->_p_current_component->has_focus())
        return _p_curr_menu->_p_current_component->next(loop);
    else
        return _p_curr_menu->next(loop);
}

bool MenuSystem::prev(bool loop) {
    if (_p_curr_menu->_p_current_component->has_focus())
        return _p_curr_menu->_p_current_component->prev(loop);
    else
        return _p_curr_menu->prev(loop);
}

void MenuSystem::reset() {
  // go to root menu
  _p_curr_menu->reset();
  _p_curr_menu->set_focus(false);
  _p_curr_menu = _p_root_menu;
  _p_root_menu->reset();
  _p_root_menu->set_focus(true);
}

void MenuSystem::select() {
    Menu* pMenu = _p_curr_menu->activate_menucomponent();

    if (pMenu != nullptr)
        _p_curr_menu = pMenu;
}

bool MenuSystem::back() {
  // Deactivate current component if it has focus
  if (_p_curr_menu->_p_current_component->has_focus()){
    _p_curr_menu->_p_current_component->set_focus(false);
    return true;
  }
  // Go 1 level up if no component was active
  // and reset current menu
  if (_p_curr_menu != _p_root_menu){
    // reset the items in the current menu and deactivate it
    _p_curr_menu->reset();
    _p_curr_menu->set_focus(false);
    // activate the parent menu
    _p_curr_menu = const_cast<Menu*>(_p_curr_menu->get_parent());
    _p_curr_menu->set_focus(true);
    return true;
  }

  // We are already in the root menu
  return false;
}

Menu& MenuSystem::get_root_menu() const {
    return *_p_root_menu;
}

Menu const* MenuSystem::get_current_menu() const {
    return _p_curr_menu;
}

void MenuSystem::display() const {
  if (_p_curr_menu != nullptr){
        _renderer.render(*_p_curr_menu);
  }
  else{
    _renderer.render(*_p_root_menu);
  }
}
