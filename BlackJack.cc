#include "BlackJack.hh"

#include <akimbo/ui/Console.hh>

BlackJack::BlackJack(Akimbo::TextureAtlas& cardAtlas) : Game(cardAtlas)
{
	Vec2 cardSize = Card::size() * cardAtlas.getAspectRatio() * 0.5f;

	ui.setBackgroundColor(0.f, 0.3f, 0.0f, 1.0f);

	addOption("moi", []()
	{
				
	});

	onClick = [](CardDeck& deck, Card* card)
	{
	};

	auto& dealer = ui.add<CardDeck> (
		ui.left(50).then(-cardSize.x), ui.top(10).then(-cardSize.y), ui.left(50).then(cardSize.x), ui.top(10).then(cardSize.y),
		cardAtlas, Scatter::None, onClick
	);
	
	auto& dealerDeck = ui.add<CardDeck> (
		dealer.right(cardSize.x * 2), dealer.top(), dealer.right(cardSize.x * 4), dealer.bottom(),
		cardAtlas, Scatter::None, onClick
	);

	auto& player = ui.add<CardDeck> (
		dealer.left(), dealer.top(-50), dealer.right(), dealer.bottom(50),
		cardAtlas, Scatter::None, onClick
	);

	auto& console = ui.add<Akimbo::UI::Console>(
		ui.left(), ui.top(), dealer.left(), ui.bottom()
	);

	console.command("take") = [this, &console](const std::string& str)
	{
		size_t move = deck(Deck::DealerDeck).moveTo(deck(Deck::PlayerHand), 2, true, true);

		console.log.addMessage("dealerDeck count: " + std::to_string(deck(Deck::DealerDeck).count()));
		console.log.addMessage("move: " + std::to_string(move));


	};

}

void BlackJack::restart(bool first)
{
	if(first)
	{
		for(int i = 0; i < 2; i++)
		{
			for(int t = Card::Type::Hearts; t <= Card::Type::Spades; t++)
			{
				for(int v = Card::Name::Ace; v <= Card::Name::King; v++)
				{
					deck(Deck::DealerDeck).add(static_cast <Card::Type> (t), v);
				}
			}
		}

		deck(Deck::DealerDeck).toggleCount();
	}
	else
	{
		// set all cards back to dealerDeck
		deck(Deck::DealerHand).moveTo(deck(Deck::DealerDeck), deck(Deck::DealerHand).count());
		deck(Deck::PlayerHand).moveTo(deck(Deck::DealerDeck), deck(Deck::PlayerHand).count());
	}



	//deck(Deck::DealerDeck).shuffle();

	//deck(Deck::DealerDeck).moveTo(deck(Deck::PlayerHand), 2, true, true);


}

void BlackJack::update(double delta)
{

}

CardDeck& BlackJack::deck(Deck d)
{
	return ui.get<CardDeck> ((size_t)d);
}
