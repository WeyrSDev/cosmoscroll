#ifndef SETTINGSMENU_HPP
#define SETTINGSMENU_HPP

#include "scenes/BaseMenu.hpp"

class SettingsMenu: public BaseMenu
{
public:
	SettingsMenu();

private:
	void EventCallback(int id) override;

	gui::CheckBox* cb_fullscreen_;
	gui::CheckBox* cb_vsync_;
	gui::OptionList<std::string>* opt_languages_;

	gui::Button* but_back_;
	gui::FormLayout form_;
};

#endif // SETTINGSMENU_HPP

