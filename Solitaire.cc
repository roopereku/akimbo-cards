#include "Solitaire.hh"

#include <sstream>
#include <iomanip>

Solitaire::Solitaire(Akimbo::TextureAtlas& cardAtlas) : Game(cardAtlas)
{
	setBackgroundColor(0.0f, 0.3f, 0.0f, 1.0f);
	cardSize = Card::size() * cardAtlas.getAspectRatio();

	onClick = [this](CardDeck& cardDeck, Card* card)
	{
		if(selectedCard && (card == selectedCard))
		{
			selectedCard->highlighted = false;
			selected->render();

			selectedCard = nullptr;
			selected = nullptr;

			return;
		}

		if(&cardDeck == &deck(Deal, 1))
		{
			if(selected)
				return;

			if(deck(Deal, 1).count() == 0)
				deck(Deal, 0).moveTo(deck(Deal, 1), deck(Deal, 0).count(), false, true);

			else
			{
				size_t count = deck(Deal, 1).count();
				deck(Deal, 1).moveTo(deck(Deal, 0), dealCount > count ? count : dealCount, true, true);
			}

			return;
		}

		if(selected == nullptr)
		{
			if(cardDeck.empty() || card == nullptr)
				return;

			if(card->flipped)
				return;

			//	Only the top card of the deal deck can be selected
			if(&cardDeck == &ui[Deal + 0] && card != &cardDeck.getTop())
				return;

			selected = &cardDeck;
			selectedCard = card;

			selectedCard->highlighted = true;
			selectedCard->highlight = Vec3(1.0f, 1.0f, 0.0f);
		}

		else if(&cardDeck != &deck(Deal, 0))
		{
			bool isSafe;
			if(!cardFits(cardDeck, *selectedCard, isSafe))
				return;

			//	FIXME With 3 card deals the underlying card might disapper when 2 cards are present
			selectedCard->highlighted = false;
			selected->moveTo(cardDeck, *selectedCard);

			if(!selected->empty())
			{
				selected->getTop().flipped = false;
				selected->render();
			}

			if(keepScore)
				updateInfo(score + 5 * (isSafe + 1), moves);

			selectedCard = nullptr;
			selected = nullptr;
		}
	};

	Vec2 offset(0.0f, 0.0f);

	offset.x = -(cardSize.x * 7) / 2;
	float leftDeckOffset = -(cardSize.x * 3.5f) / 2;


	//	Create safes

	ui.add <CardDeck> (
		ui.left(50).then(offset.x), ui.top(offset.y),
		ui.left(50).then(offset.x + cardSize.x), ui.top(offset.y + cardSize.y),
		cardAtlas, Scatter::None, onClick
	);

	for(int i = 0; i < 3; i++)
	{
		ui.add <CardDeck> (
			deck(Safe, i).right(), deck(Safe, i).top(),
			deck(Safe, i).right(cardSize.x), deck(Safe, i).bottom(),
			cardAtlas, Scatter::None, onClick
		);
	}

	//	Create deals
	
	ui.add <CardDeck> (
		ui.left(50).then(-leftDeckOffset - cardSize.x), ui.top(),
		ui.left(50).then(-leftDeckOffset + cardSize.x / 2), ui.top(cardSize.y),
		cardAtlas, Scatter::Horizontally, onClick
	);

	ui.add <CardDeck> (
		ui.left(50).then(-offset.x - cardSize.x), ui.top(),
		ui.left(50).then(-offset.x), ui.top(cardSize.y),
		cardAtlas, Scatter::None, onClick
	);

	offset.y += cardSize.y * 1.5f;

	//	Create fields

	ui.add <CardDeck> (
		ui.left(50).then(offset.x), ui.top(offset.y),
		ui.left(50).then(offset.x + cardSize.x), ui.bottom(),
		cardAtlas, Scatter::Vertically, onClick
	);

	for(int i = 0; i < 6; i++)
	{
		ui.add <CardDeck> (
			deck(Field, i).right(), deck(Field, i).top(),
			deck(Field, i).right(cardSize.x), deck(Field, i).bottom(),
			cardAtlas, Scatter::Vertically, onClick
		);
	}

	//	Create info fields

	timerLabel = &ui.add <Akimbo::UI::Label> (
		deck(Deal, 0).left(), deck(Deal, 0).bottom().then(0.03f),
		deck(Deal, 1).right(), deck(Field, 0).top()
	);

	auto& info = ui.add <Akimbo::UI::Container> (
		deck(Safe, 0).left(), timerLabel->top(),
		deck(Safe, 3).right(), timerLabel->bottom()
	);

	scoreLabel = &info.add <Akimbo::UI::Label> (
		info.left(), info.top(),
		info.left(45), info.bottom()
	);

	movesLabel = &info.add <Akimbo::UI::Label> (
		info.left(55), info.top(),
		info.right(), info.bottom()
	);

	info.setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
	timerLabel->setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
	movesLabel->setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
	scoreLabel->setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);

	overlay = &ui.add <Overlay> (
		ui.left(), ui.top(),
		ui.right(), ui.bottom()
	);

	addOption("3-card deals", [this](bool threeCard)
	{
		dealCount = threeCard ? 3 : 1;
		deck(Deal, 0).limitHorizontalVisibility(dealCount);
		restart(false);
	});

	addOption("relax mode", [this](bool on)
	{
		keepScore = !on;
		restart(false);
	});
}

void Solitaire::updateInfo(int newScore, int newMoves)
{
	if(newScore != score)
	{
		score = newScore;
		if(score < 0) score = 0;
		scoreLabel->setText("Score: " + std::to_string(score));
	}

	if(newMoves != moves)
	{
		moves = newMoves;
		movesLabel->setText("Moves: " + std::to_string(moves));
	}
}

void Solitaire::restart(bool first)
{
	if(first)
	{
		for(int t = Card::Type::Hearts; t <= Card::Type::Spades; t++)
		{
			for(int v = Card::Name::Ace; v <= Card::Name::King; v++)
			{
				deck(Deal, 1).add(static_cast <Card::Type> (t), v);
				deck(Deal, 1).getTop().flipped = true;
			}
		}

		deck(Deal, 0).limitHorizontalVisibility(dealCount);
		deck(Deal, 1).toggleCount();
	}

	else
	{
		for(size_t i = 0; i < 13; i++)
		{
			if(i == Deal + 1)
				continue;

			deck(i).moveTo(deck(Deal, 1), deck(i).count());
		}

		deck(Deal, 1).flipAll(true);
	}

	deck(Deal, 1).shuffle();
	for(size_t i = 0; i < 7; i++)
	{
		deck(Deal, 1).getTop().flipped = false;
		deck(Deal, 1).moveTo(deck(Field, i), i + 1, true, false);
	}

	elapsed = 0.0;
	secondsElapsed = 0;

	if(keepScore)
	{
		timerLabel->setText("00:00:00");
		updateInfo(0, 0);
	}

	else
	{
		timerLabel->setText("");
		movesLabel->setText("");
		scoreLabel->setText("");
	}
}

void Solitaire::update(double delta)
{
	if(!keepScore)
		return;

	elapsed += delta;
	if(elapsed >= 1.0)
	{
		secondsElapsed++;
		elapsed = 0;

		int seconds = secondsElapsed;
		int minutes = 0;
		int hours = 0;

		//	Decrement score every 10 seconds
		if(seconds % 10 == 0)
			updateInfo(score - 2, moves);

		if(seconds >= 60)
		{
			minutes = seconds / 60;
			seconds %= 60;

			if(minutes >= 60)
			{
				hours = minutes / 60;
				minutes %= 60;
			}
		}

		std::ostringstream ss;
		ss	<< std::setfill('0')
			<< std::setw(2) << hours << ':'
			<< std::setw(2) << minutes << ':'
			<< std::setw(2) << seconds;

		timerLabel->setText(ss.str());
	}
}

CardDeck& Solitaire::deck(Deck type, size_t index)
{
	return ui.get <CardDeck> (type + index);
}

CardDeck& Solitaire::deck(size_t index)
{
	return ui.get <CardDeck> (index);
}

bool Solitaire::cardFits(CardDeck& cardDeck, Card& card, bool& isSafe)
{
	int safe = 0;
	for(size_t i = Safe + 0; i < Safe + 4; i++)
		safe += &cardDeck == &deck(i);

	if(safe)
	{
		isSafe = true;
		if(cardDeck.empty())
			return card.value == Card::Name::Ace;

		return card.t == cardDeck.getTop().t && card.value == cardDeck.getTop().value + 1;
	}

	else
	{
		isSafe = false;
		if(cardDeck.empty())
			return card.value == Card::Name::King;

		bool sameColor = card.isRed() == cardDeck.getTop().isRed();
		return !sameColor && card.value == cardDeck.getTop().value - 1;
	}
}
