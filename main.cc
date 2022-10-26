#include "Solitaire.hh"
#include "CardDeck.hh"
#include "Overlay.hh"
#include "Card.hh"

#include <akimbo/Core.hh>

class Cards : public Akimbo::Core
{
public:
	Cards()
		: cardAtlas("resources/cardatlas.jpg", 13, 5),
		  game(ui.add <Solitaire> (
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
	Akimbo::TextureAtlas cardAtlas;
	Game& game;
};

int main()
{
	Cards c;
	c.start();
}
