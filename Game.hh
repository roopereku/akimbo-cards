#ifndef GAME_HH
#define GAME_HH

#include <akimbo/TextureAtlas.hh>
#include <akimbo/ui/Widget.hh>
#include <akimbo/ui/TabbedContainer.hh>

class Game : public Akimbo::UI::TabbedContainer
{
public:
	Game(Akimbo::TextureAtlas& cardAtlas);

	void addOption(const std::string& name, const std::function <void(bool)>& callback);
	void addOption(const std::string& name, const std::function <void()>& callback);

	virtual void restart(bool first) = 0;
	virtual void update(double delta) = 0;

protected:
	Akimbo::UI::Container& ui;
	Akimbo::UI::Container& options;

	Akimbo::TextureAtlas& cardAtlas;

private:
	float optionOffsetY = 0.1f;
};

#endif
