#include "Game.hh"

#include <akimbo/ui/Switch.hh>
#include <akimbo/ui/Button.hh>

Game::Game(Akimbo::TextureAtlas& cardAtlas) : ui(tab("game")), options(tab("options")), cardAtlas(cardAtlas)
{
	setTitleTransparency(0.1f);

	ui.setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
	options.setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Game::addOption(const std::string& name, const std::function <void(bool)>& callback)
{
	auto& l = options.add <Akimbo::UI::Label> (
		options.left(0.3f), options.top(optionOffsetY),
		options.left(50), options.top(optionOffsetY + 0.3f)
	);

	auto& s = options.add <Akimbo::UI::Switch> (
		options.left(75).then(-0.3f), options.top(optionOffsetY),
		options.left(75).then(+0.3f), options.top(optionOffsetY + 0.3f)
	);

	l.setText(name);
	l.setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);

	s.onSwitch = callback;
	optionOffsetY += 0.4f;
}

void Game::addOption(const std::string& name, const std::function <void()>& callback)
{
	auto& b = options.add <Akimbo::UI::Button> (
		options.left(50).then(-0.3f), options.top(optionOffsetY),
		options.left(50).then(+0.3f), options.top(optionOffsetY + 0.3f)
	);

	b.setText(name);
	b.setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);

	b.onClick = callback;
	optionOffsetY += 0.4f;

}
