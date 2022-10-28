#include "BlackJack.hh"
#include "Solitaire.hh"
#include "CardDeck.hh"
#include "Overlay.hh"
#include "Card.hh"

#include <akimbo/Core.hh>

class Cards : public Akimbo::Core
{
public:
	Cards()
		: f(setDefault()),
		  cardAtlas("resources/cardatlas.jpg", 13, 5),
		  game(ui.add <BlackJack> (
				ui.left(), ui.top(),
				ui.right(), ui.bottom(),
				cardAtlas
		  ))
	{
		game.addOption("restart", [this]()
		{
			game.restart(false);
		});

		game.restart(true);
	}

	void onUpdate(double delta) override
	{
		game.update(delta);
	}

private:
	Akimbo::Font& f;
	Akimbo::Font& setDefault()
	{
		Akimbo::Font& f = loadFont("/usr/share/fonts/TTF/FiraCode-Medium.ttf");
		setDefaultFont(f);
		return f;
	}
	Akimbo::TextureAtlas cardAtlas;
	Game& game;
};

int main()
{
	Cards c;
	c.start();
}
