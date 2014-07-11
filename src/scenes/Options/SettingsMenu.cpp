#include "SettingsMenu.hpp"
#include "core/Game.hpp"
#include "core/ControlPanel.hpp"
#include "utils/I18n.hpp"


SettingsMenu::SettingsMenu()
{
	setTitle(_t("settings.title"));

	form_.SetOffset(90, 140);
	form_.SetSpacing(10, 25);
	form_.SetLabelAlignment(gui::Align::RIGHT);

	cb_fullscreen_ = new gui::CheckBox(this);
	cb_fullscreen_->SetCallbackID(1);
	cb_fullscreen_->Check(Game::getInstance().isFullscreen());
	form_.AddRow(_t("settings.fullscreen"), cb_fullscreen_);

	cb_vsync_ = new gui::CheckBox(this);
	cb_vsync_->SetCallbackID(2);
	cb_vsync_->Check(Game::getInstance().isVerticalSync());
	form_.AddRow(_t("settings.vsync"), cb_vsync_);

	opt_languages_ = new gui::OptionList<std::string>(this);
	opt_languages_->Add(L"English",  "en");
	opt_languages_->Add(L"Français", "fr");
	opt_languages_->Add(L"Deutsch",  "de");

	opt_languages_->SelectByValue(I18n::getInstance().getCurrentLanguage());
	opt_languages_->SetCallbackID(3);
	form_.AddRow(_t("settings.language"), opt_languages_);

	but_back_ = new CosmoButton(this, _t("back"));
	but_back_->setPosition(210, 340);
	but_back_->SetCallbackID(0);
}


void SettingsMenu::EventCallback(int id)
{
	switch (id)
	{
		case 0:
			Game::getInstance().setCurrentScreen(Game::SC_OptionMenu);
			break;
		case  1:
			Game::getInstance().setFullscreen(cb_fullscreen_->Checked());
			break;
		case 2:
			Game::getInstance().setVerticalSync(cb_vsync_->Checked());
			break;
		case 3:
			I18n::getInstance().loadFromCode(opt_languages_->GetSelectedValue());
			// delete other scenes
			Game::getInstance().unloadScreens();
			ControlPanel::getInstance().refreshTextTranslations();
			// re-load i18ned texts
			setTitle(_t("settings.title"));
			form_.GetLabelAt(0)->setString(_t("settings.fullscreen"));
			form_.GetLabelAt(1)->setString(_t("settings.vsync"));
			form_.GetLabelAt(2)->setString(_t("settings.language"));
			form_.AlignRows();

			but_back_->setString(_t("back"));
			break;
	}
}
