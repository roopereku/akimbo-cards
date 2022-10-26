#include "Solitaire.hh"

#include <sstream>
#include <iomanip>

Solitaire::Solitaire(Akimbo::TextureAtlas& cardAtlas) : Game(cardAtlas)
{
	setBackgroundColor(0.0f, 0.3f, 0.0f, 1.0f);
	cardSize = Card::size() * cardAtlas.getAspectRatio();
	dealCount = 1;

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

			//	Only the last card of the deal deck can be selected
			if(&deck == decks[Deal + 0] && card != &deck.back())
				return;

			selected = &deck;
			selectedCard = card;

			selectedCard->highlighted = true;
			selectedCard->highlight = Vec3(1.0f, 1.0f, 0.0f);
		}

		else if(&deck != decks[Deal + 0])
		{
			if(!cardFits(deck, *selectedCard))
				return;

			selectedCard->highlighted = false;
			selected->moveTo(deck, *selectedCard);

			if(!selected->empty())
			{
				selected->back().flipped = false;
				selected->render();
			}

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
	});

	addOption("relax mode", [this](bool on)
	{
		keepScore = !on;
		restart(false);
	});
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
				decks[Deal + 1]->back().flipped = true;
			}
		}

		decks[Deal + 1]->toggleCount();
		decks[Deal + 0]->limitVisible(dealCount);
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
		decks[Deal + 1]->back().flipped = false;
		decks[Deal + 1]->moveTo(*decks[Field + i], i + 1, true, false);
	}

	elapsed = 0.0;
	secondsElapsed = 0;

	timerLabel->setText("");
	movesLabel->setText("");
	scoreLabel->setText("");
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
		scoreLabel->setText("Score: 3218");
		movesLabel->setText("Moves: 169");
	}
}

bool Solitaire::cardFits(CardDeck& deck, Card& card)
{
	int safe = 0;
	for(size_t i = Safe + 0; i < Safe + 4; i++)
		safe += &deck == decks[i];

	if(safe)
	{
		if(deck.empty())
			return card.value == Card::Name::Ace;

		return card.t == deck.back().t && card.value == deck.back().value + 1;
	}

	else
	{
		if(deck.empty())
			return card.value == Card::Name::King;

		bool sameColor = card.isRed() == deck.back().isRed();
		return !sameColor && card.value == deck.back().value - 1;
	}
}
