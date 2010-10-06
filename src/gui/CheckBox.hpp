#ifndef GUI_CHECKBOX_HPP
#define GUI_CHECKBOX_HPP

#include "Widget.hpp"

namespace gui
{

/**
 * Une case à cocher pour un choix binaire
 * callback appelée quand : choix modifié
 */
class CheckBox: public Widget
{
public:
	CheckBox(Menu* owner, int x, int y);

	/**
	 * @return true si la case à cocher est cochée
	 */
	bool Checked() const;

	/**
	 * Check/uncheck the checkbox
	 */
	void Check(bool checked);

	// inherited callbacks
	void OnKeyPressed(sf::Key::Code code);
	void OnMouseClicked(int, int);

protected:
	// inherited
	void OnStateChanged(State::EState state);

private:
	// inherited
	void Render(sf::RenderTarget& target) const;

	bool checked_;
	sf::Shape box_;
	sf::Shape v1_;
	sf::Shape v2_;
};

}

#endif // GUI_CHECKBOX_HPP

