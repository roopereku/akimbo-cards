#include "Solitaire.hh"

#include <sstream>
#include <iomanip>

Solitaire::Solitaire(Akimbo::TextureAtlas& cardAtlas) : Game(cardAtlas)
{
	setBackgroundColor(0.0f, 0.3f, 0.0f, 1.0f);
	cardSize = Card::size() * cardAtlas.getAspectRatio();

	onClick = [this](CardDeck& deck, Card* card)
	{
		if(selectedCard && (card == selectedCard))
		{
			selectedCard->highlighted = false;
			selected->render();

			selectedCard = nullptr;
			selected = nullptr;

			return;
		}

		if(&deck == decks[Deal + 1])
		{
			if(selected)
				return;

			if(decks[Deal + 1]->count() == 0)
				decks[Deal + 0]->moveTo(*decks[Deal + 1], decks[Deal + 0]->count(), false, true);

			else
			{
				size_t count = decks[Deal + 1]->count();
				decks[Deal + 1]->moveTo(*decks[Deal + 0], dealCount > count ? count : dealCount, true, true);
			}

			return;
		}

		if(selected == nullptr)
		{
			if(deck.empty() || card == nullptr)
				return;

			if(card->flipped)
				return;

			//	Only the top card of the deal deck can be selected
			if(&deck == decks[Deal + 0] && card != &deck.getTop())
				return;

			selected = &deck;
			selectedCard = card;

			selectedCard->highlighted = true;
			selectedCard->highlight = Vec3(1.0f, 1.0f, 0.0f);
		}

		else if(&deck != decks[Deal + 0])
		{
			bool isSafe;
			if(!cardFits(deck, *selectedCard, isSafe))
				return;

			//	FIXME With 3 card deals the underlying card might disapper when 2 cards are present
			selectedCard->highlighted = false;
			selected->moveTo(deck, *selectedCard);

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

	decks[Safe + 0] = &ui.add <CardDeck> (
		ui.left(50).then(offset.x), ui.top(offset.y),
		ui.left(50).then(offset.x + cardSize.x), ui.top(offset.y + cardSize.y),
		cardAtlas, Scatter::None, onClick
	);

	for(int i = 0; i < 3; i++)
	{
		decks[Safe + i + 1] = &ui.add <CardDeck> (
			decks[Safe + i]->right(), decks[Safe + i]->top(),
			decks[Safe + i]->right(cardSize.x), decks[Safe + i]->bottom(),
			cardAtlas, Scatter::None, onClick
		);
	}

	offset.y += cardSize.y * 1.5f;

	decks[Field + 0] = &ui.add <CardDeck> (
		ui.left(50).then(offset.x), ui.top(offset.y),
		ui.left(50).then(offset.x + cardSize.x), ui.bottom(),
		cardAtlas, Scatter::Vertically, onClick
	);

	for(int i = 0; i < 6; i++)
	{
		decks[Field + i + 1] = &ui.add <CardDeck> (
			decks[Field + i]->right(), decks[Field + i]->top(),
			decks[Field + i]->right(cardSize.x), decks[Field + i]->bottom(),
			cardAtlas, Scatter::Vertically, onClick
		);
	}

	decks[Deal + 1] = &ui.add <CardDeck> (
		ui.left(50).then(-offset.x - cardSize.x), ui.top(),
		ui.left(50).then(-offset.x), ui.top(cardSize.y),
		cardAtlas, Scatter::None, onClick
	);

	offset.x = -(cardSize.x * 3.5f) / 2;

	decks[Deal + 0] = &ui.add <CardDeck> (
		ui.left(50).then(-offset.x - cardSize.x), ui.top(),
		ui.left(50).then(-offset.x + cardSize.x / 2), ui.top(cardSize.y),
		cardAtlas, Scatter::Horizontally, onClick
	);

	timerLabel = &ui.add <Akimbo::UI::Label> (
		decks[Deal + 0]->left(), decks[Deal + 0]->bottom().then(0.03f),
		decks[Deal + 1]->right(), decks[Field + 0]->top()
	);

	auto& info = ui.add <Akimbo::UI::Container> (
		decks[Safe + 0]->left(), timerLabel->top(),
		decks[Safe + 3]->right(), timerLabel->bottom()
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
		decks[Deal + 0]->limitHorizontalVisibility(dealCount);
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
				decks[Deal + 1]->add(static_cast <Card::Type> (t), v);
				decks[Deal + 1]->getTop().flipped = true;
			}
		}

		decks[Deal + 1]->toggleCount();
	}

	else
	{
		for(size_t i = 0; i < 13; i++)
		{
			if(i == Deal + 1)
				continue;

			decks[i]->moveTo(*decks[Deal + 1], decks[i]->count());
		}

		decks[Deal + 1]->flipAll(true);
	}

	decks[Deal + 1]->shuffle();
	for(size_t i = 0; i < 7; i++)
	{
		decks[Deal + 1]->getTop().flipped = false;
		decks[Deal + 1]->moveTo(*decks[Field + i], i + 1, true, false);
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

	DBG_LOG("%d", dealCount);
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

bool Solitaire::cardFits(CardDeck& deck, Card& card, bool& isSafe)
{
	int safe = 0;
	for(size_t i = Safe + 0; i < Safe + 4; i++)
		safe += &deck == decks[i];

	if(safe)
	{
		isSafe = true;
		if(deck.empty())
			return card.value == Card::Name::Ace;

		return card.t == deck.getTop().t && card.value == deck.getTop().value + 1;
	}

	else
	{
		isSafe = false;
		if(deck.empty())
			return card.value == Card::Name::King;

		bool sameColor = card.isRed() == deck.getTop().isRed();
		return !sameColor && card.value == deck.getTop().value - 1;
	}
}
