/*
 * Copyright (c) 2015, 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <string> 
using namespace std;

class Menu;
class MenuComponentRenderer;
class MenuSystem;

//! \brief Abstract base class that represents a component in the menu
//! This is the abstract base class for the main components used
//! to build a
//! menu structure: Menu and MenuItem.
//!
//! MenuComponent should not be used as a base class in clients.
//! Instead use
//! either Menu or MenuItem.
//!
//! MenuComponent is the `Component` part of the composite
//! design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern).
//!
//! \see Menu
//! \see MenuItem
class MenuComponent {
    friend class MenuSystem;
    friend class Menu;
public:
    //! \brief Callback for when the MenuComponent is activated
    //!
    //! \param menu_component The menu component being activated.
    using ComponentCbPtr = void (*)(MenuComponent* menu_component);

public:
    //! \brief Construct a MenuComponent
    //! \param[in] name The name of the menu component that is
    //! displayed in clients.
  MenuComponent(const char* name, ComponentCbPtr on_activate, ComponentCbPtr on_current);

    //! \brief Set the component's name
    //! \param[in] name The name of the menu component that is
    //! displayed in clients.
    void set_name(const char* name);

    //! \brief Gets the component's name
    //! \returns The component's name.
    const char* get_name() const;

    //! \brief Renders the component using the given MenuComponentRenderer
    //!
    //! This is the `accept` method in the visitor design pattern.
    //! It should
    //! call the appropriate method on the given MenuComponentRenderer for
    //! rendering this type of component. This method should be called
    //! from the
    //! MenuComponent::render method.
    //!
    //! \param[in] renderer The MenuComponentRenderer to use
    //! to render this component
    //!
    //! \see MenuComponentRenderer
    virtual void render(MenuComponentRenderer const& renderer) const = 0;

    //! \brief Returns true if this component has focus; false otherwise
    //!
    //! A component has focus when the next and prev functions are able to
    //! change its state rather than navigate the menu structure. It's
    //! primarily used in NumericMenuItem so next and prev
    //! change the value
    //! associated with the component.
    //!
    //! Subclasses should set _is_active to true when this
    //! behaviour is desired
    //! and reset it to false when it's no longer required.
    //! The usual place to
    //! do this is in the MenuComponent::activate method.
    //!
    //! \returns true if this component has focus, false otherwise.
    //!
    //! \see MenuComponent::activate
    //! \see NumericMenuComponent
    bool is_active() const;

    //! \brief Returns true if this is the current component;
    //! false otherwise
    //!
    //! This bool registers if the component is the current
    //! activateed component.
    //!
    //! Subclasses should use set_current() when the component becomes
    //! activated and use set_previous() once the component is
    //! no longer the current component.
    //!
    //! \returns true if this component is the current component, false
    //!          otherwise.
    //! \see MenuComponent::set_current
    bool is_current() const;

    //! \brief Sets the function to call when the MenuItem is activated
    //! \param[in] on_activate The function to call when the MenuItem is
    //!                      activated.
    void set_on_activate_cb(ComponentCbPtr on_activate);

    //! \brief Sets the function to call when the MenuItem
    //! is getting current
    //! \param[in] on_current The function to call when the MenuItem
    //! becomes current.
    void set_on_current_cb(ComponentCbPtr on_current);

    //! Sets the parent of the current component
    void set_parent(Menu* p_parent);

    //! Returns pointer to the parent
     Menu const* get_parent();

protected:
    //! \brief Processes the next action
    //!
    //! The behaviour of this function can differ depending on whether
    //! MenuComponent::is_active returns true or false.
    //!
    //! When MenuComponent::is_active returns true, this method
    //! should change
    //! some state in the component; when it returns false,
    //! this method should
    //! navigate the menu structure.
    //!
    //! \param[in] loop if true the action should
    //! loop around to the beginning for finite ranges;
    //! otherwise overflow should be prevented.
    //! \returns true if the component processed the action,
    //! false otherwise.
    //!
    //! \see MenuComponent::prev
    //! \see MenuComponent::is_active
    virtual bool next(bool loop=false) = 0;

    //! \brief Processes the prev action
    //!
    //! The behaviour of this function can differ depending on whether
    //! MenuComponent::is_active returns true or false.
    //!
    //! When MenuComponent::is_active returns true,
    //! this method should change
    //! some state in the component; when it returns false,
    //! this method should
    //! navigate the menu structure.
    //!
    //! \param[in] loop if true the action should loop around to the end
    //!                 for finite ranges; otherwise overflow should be
    //!                 prevented.
    //! \returns true if the component processed the action,
    //! false otherwise.
    //!
    //! \see MenuComponent::next
    //! \see MenuComponent::is_active
    virtual bool prev(bool loop=false) = 0;

    //! \brief Resets the component to its initial state
    virtual void reset() = 0;

    //! \brief Processes the activat action
    //!
    //! When a menu component is activated by the client
    //! an action may need to performed.
    //!
    //! If the component supports focus, this method
    //! is the recommended place
    //! set _is_active to true so the MenuComponent::next and
    //! MenuComponent::prev methods can be used to change some
    //! state in the component.
    //!
    //! The default implementation calls on_activate if it's not null.
    //! Components
    //! that derive from this class must call their parent's
    //! implementation.
    //!
    //! \returns The Menu instance activated or nullptr. The returned Menu
    //!          instance is used in MenuSystem::activate to set
    //!          the current
    //!          menu in the MenuSystem.
    //!
    //! \see MenuComponent::is_active
    //! \see NumericMenuComponent
    virtual Menu* activate();

    //! \brief Set the current state of the component
    //!
    //! \paran is_current true if this component is the current one; false
    //!                   otherwise.
    //! \see is_current
    void set_current(bool is_current=true);

    //! \brief Set the focus of the component
    //!
    //! \paran is_active true if this component is active; false
    //!                   otherwise.
    //! \see is_current
    void set_active(bool is_active=true);

protected:
    const char* _name;
    bool _is_active;
    bool _is_current;
    ComponentCbPtr _on_activate;
    ComponentCbPtr _on_current;
    Menu* _p_parent;
};


//! \brief A MenuComponent that calls a callback function when activated.
//!
//! MenuItem represents the `Leaf` in the composite design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern). When a MenuItem is
//! activated, the user-defined MenuItem::_on_activate callback is called.
//!
//! \see MenuComponent
//! \see Menu
class MenuItem : public MenuComponent {
public:
    //! \brief Construct a MenuItem
    //! \param[in] name The name of the menu component that
    //! is displayed in
    //!                 clients.
    //! \param[in] on_activate The function to call when the MenuItem is
    //!                      activated.
  MenuItem(const char* name, ComponentCbPtr on_activate=nullptr, ComponentCbPtr on_current=nullptr);

    //! \copydoc MenuComponent::render
    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    //! \copydoc MenuComponent::next
    //!
    //! This method does nothing in MenyItem.
    virtual bool next(bool loop=false);

    //! \copydoc MenuComponent::prev
    //!
    //! This method does nothing in MenuItem.
    virtual bool prev(bool loop=false);

    //! \copydoc MenuComponent::reset
    //!
    //! This method does nothing in MenuItem.
    virtual void reset();

    //! \copydoc MenuComponent:activate
    virtual Menu* activate();
};


//! \brief A MenuItem that calls MenuSystem::back() when activated.
//! \see MenuItem
class BackMenuItem : public MenuItem {
public:
  BackMenuItem(const char* name, MenuSystem* ms, ComponentCbPtr on_activate=nullptr, ComponentCbPtr on_current=nullptr);

    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    virtual Menu* activate();

protected:
    MenuSystem* _menu_system;
};


class NumericMenuItem : public MenuItem {
public:
    //! \brief Callback for formatting the numeric value into a string.
    //!
    //! \param value The value to convert.
    //! \returns The string representation of value.
  using ValueCbPtr = const string (*)(const float value);

public:
    //! Constructor
    //!
    //! @param name The name of the menu item.
    //! @param on_activate The function to call when this
    //! MenuItem is activated.
    //! @param value Default value.
    //! @param min_value The minimum value.
    //! @param max_value The maximum value.
    //! @param increment How much the value should be incremented by.
    //! @param format_value_fn The custom formatter. If nullptr the string
    //!                        float formatter will be used.
    NumericMenuItem(const char* name,
                    float value, float min_value, float max_value,
                    float increment=1.0,
		    ComponentCbPtr on_activate=nullptr,
		    ComponentCbPtr on_current=nullptr,
                    ValueCbPtr format_value_fn=nullptr);

    //!
    //! \brief Sets the custom number formatter.
    //!
    //! \param numberFormat the custom formatter. If nullptr
    //! the string float
    //!                     formatter will be used (2 decimals)
    //!
    void set_number_formatter(ValueCbPtr format_value_fn);

    float get_value() const;
    float get_min_value() const;
    float get_max_value() const;

    void set_value(float value);
    void set_min_value(float value);
    void set_max_value(float value);

    string get_formatted_value() const;

    virtual void render(MenuComponentRenderer const& renderer) const;

protected:
    virtual bool next(bool loop=false);
    virtual bool prev(bool loop=false);

    virtual Menu* activate();

protected:
    float _value;
    float _min_value;
    float _max_value;
    float _increment;
    ValueCbPtr _format_value_fn;
};


//! \brief A MenuComponent that can contain other MenuComponents.
//!
//! Menu represents the branch in the composite design pattern (see:
//! https://en.wikipedia.org/wiki/Composite_pattern). When a Menu is
//! activated, the user-defined Menu::_on_activate callback is called.
//!
//! \see MenuComponent
//! \see MenuItem
class Menu : public MenuComponent {
    friend class MenuSystem;
public:
  Menu(const char* name, ComponentCbPtr on_activate=nullptr, ComponentCbPtr on_current=nullptr);

    //! \brief Adds a MenuItem to the Menu
    void add(MenuComponent* p_item);

    MenuComponent const* get_current_component() const;
    MenuComponent const* get_menu_component(uint8_t index) const;

    uint8_t get_num_components() const;
    uint8_t get_current_component_num() const;
    uint8_t get_previous_component_num() const;

    //! \copydoc MenuComponent::render
    void render(MenuComponentRenderer const& renderer) const;

protected:

    //! \brief Activates the current selection
    //!
    //! When a client makes a selection, activate_menucomponent is called
    //! on the current menu
    //! which in turn calls the menu's current item's callback.
    Menu* activate_menucomponent();

    //! \copydoc MenuComponent::next
    virtual bool next(bool loop=false);

    //! \copydoc MenuComponent::prev
    virtual bool prev(bool loop=false);

    //! \copydoc MenuComponent::activate
    virtual Menu* activate();

    //! \copydoc MenuComponent::reset
    virtual void reset();

    //void add_component(MenuComponent* p_component);

private:
    MenuComponent* _p_current_component;
    MenuComponent** _menu_components;
    uint8_t _num_components;
    uint8_t _current_component_num;
    uint8_t _previous_component_num;
};


class MenuSystem {
public:
  MenuSystem(MenuComponentRenderer const& renderer, const char* name="");

    void display() const;
    bool next(bool loop=false);
    bool prev(bool loop=false);
    void activate();
    bool back();
    void reset();

    Menu& get_root_menu() const;
    Menu const* get_current_menu() const;

private:
    Menu* _p_root_menu;
    Menu* _p_current_menu;
    MenuComponentRenderer const& _renderer;
};


class MenuComponentRenderer {
public:
    virtual void render(MenuItem const& menu_item) const = 0;
    virtual void render(BackMenuItem const& menu_item) const = 0;
    virtual void render(NumericMenuItem const& menu_item) const = 0;
    virtual void render(Menu const& menu) const = 0;
};


#endif
