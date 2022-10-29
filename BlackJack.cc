#include "BlackJack.hh"

#include <akimbo/ui/Console.hh>

BlackJack::BlackJack(Akimbo::TextureAtlas& cardAtlas) : Game(cardAtlas), 
	console(ui.add<Akimbo::UI::Console>(ui.left(), ui.top(), ui.left(40), ui.bottom()))
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
		ui.left(40), ui.top(10).then(-cardSize.y), ui.left(70), ui.top(10).then(cardSize.y),
		cardAtlas, Scatter::Horizontally, onClick
	);

	//dealer.limitHorizontalVisibility(11);
	
	auto& dealerDeck = ui.add<CardDeck> (
		dealer.right(cardSize.x * 2), dealer.top(), dealer.right(cardSize.x * 4), dealer.bottom(),
		cardAtlas, Scatter::None, onClick
	);

	auto& player = ui.add<CardDeck> (
		dealer.left(), dealer.top(-40), dealer.left(-cardSize.x * 2), dealer.bottom(80),
		cardAtlas, Scatter::Vertically, onClick
	);

	Vec2 btnSize = Vec2(0.3, 0.1);
	float btnGap = 0.1f;
	auto& hit = ui.add<Akimbo::UI::Button>(
		ui.left(75), ui.top(50), ui.left(75).then(btnSize.x), ui.top(50).then(btnSize.y)
	);

	console.log.setRows(50);

	console.command("take") = [this](const std::string& str)
	{
		giveCards(deck(PlayerHand), 1);
	};

	console.command("give") = [this](const std::string& str)
	{
		deck(DealerDeck).moveTo(deck(DealerHand), 1, true, true);
	};

	hit.setText("Hit");

	hit.onClick = [this]()
	{
		performMove(Action::Hit);
	};

	auto& stand = ui.add<Akimbo::UI::Button>(
		hit.left(), hit.bottom(btnGap), hit.right(), hit.bottom(btnGap + btnSize.y)
	);
	stand.setText("Stand");

	stand.onClick = [this]()
	{
		performMove(Action::Stand);
	};
}

void BlackJack::performMove(Action action)
{	
	switch(action)
	{
		case Action::Double:
			giveCards(deck(PlayerHand), 1);
		case Action::Stand:
		{
			while(dealerSum < 17)
			{
				giveCards(deck(DealerHand), 1);
			}

			if(dealerSum > 21)
				console.log.addMessage("Dealer bust!");
			else if(playerSum > dealerSum)
				console.log.addMessage("Player wins!");
			else if(dealerSum > playerSum)
				console.log.addMessage("Dealer wins!");
			else if(dealerSum == playerSum)
				console.log.addMessage("PUSH!");

			break;
		}

		case Action::Hit:
		{
			giveCards(deck(PlayerHand), 1);
			break;
		}

		case Action::Split: break;
	}

	if(isGameOver())
	{
		console.log.addMessage("the game is over");
		restart(false);
	}
}

void BlackJack::restart(bool first)
{
	if(first)
	{
		for(int i = 0; i < 6; i++)
		{
			for(int t = Card::Type::Hearts; t <= Card::Type::Spades; t++)
			{
				for(int v = Card::Name::Ace; v <= Card::Name::King; v++)
				{
					deck(DealerDeck).add(static_cast <Card::Type> (t), v);
				}
			}
		}

		deck(Deck::DealerDeck).toggleCount();
	}
	else
	{
		// set all cards back to dealerDeck
		deck(DealerHand).moveTo(deck(DealerDeck), deck(DealerHand).count());
		deck(PlayerHand).moveTo(deck(DealerDeck), deck(PlayerHand).count());\

		deck(DealerDeck).flipAll(true);
	}
	
	// shuffle cards
	deck(DealerDeck).shuffle();

	// add cards to playerhand
	giveCards(deck(PlayerHand), 2);

	// add cards to dealerhand
	giveCards(deck(DealerHand), 2);

	deck(DealerHand).flipCard(1, true);

	if(isGameOver())
	{
		console.log.addMessage("The game is over");
		restart(false);
	}
}

void BlackJack::giveCards(CardDeck& target, int amount)
{
	deck(DealerDeck).moveTo(target, amount, true, true);

	int* ptr = nullptr;

	if(&target == &deck(PlayerHand))
		ptr = &playerSum;
	else if(&target == &deck(DealerHand))
		ptr = &dealerSum;

	if(!ptr)
		return;

	*ptr = 0;
	for(size_t i = 0; i < target.count(); i++)
	{
		if(target[i].value > 10)
			*ptr += 10;
		else if(target[i].value < 2)
		{
			if(target[i].value + 11 > 21)
				*ptr += 1;
			else
				*ptr += 11;
		}
		else
			*ptr += target[i].value;
	}

	console.log.addMessage("Sum: " + std::to_string(*ptr));
}

bool BlackJack::isGameOver()
{
	if(playerSum > 21)
		console.log.addMessage("Player bust!");
	if(playerSum >= 21 || dealerSum >= 21)	
		return true;

	return false;
}

void BlackJack::update(double delta)
{

}

CardDeck& BlackJack::deck(Deck d)
{
	return ui.get<CardDeck> ((size_t)d);
}
