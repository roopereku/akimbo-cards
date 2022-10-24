#include "CardDeck.hh"
#include "Overlay.hh"
#include "Card.hh"

#include <akimbo/Core.hh>

class Cards : public Akimbo::Core
{
public:
	enum Deck
	{
		Safe = 0,
		Deal = 4,
		Field = 6
	};

	bool cardFits(CardDeck& deck, Card& card)
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

	Cards() : cardAtlas("resources/cardatlas.jpg", 13, 5)
	{
		cardSize = Card::size() * cardAtlas.getAspectRatio();
		dealCount = 3;

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

				else decks[Deal + 1]->moveTo(*decks[Deal + 0], dealCount, true, true);
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

		overlay = &ui.add <Overlay> (
			ui.left(), ui.top(),
			ui.right(), ui.bottom()
		);

		for(int t = Card::Type::Hearts; t <= Card::Type::Spades; t++)
		{
			for(int v = Card::Name::Ace; v <= Card::Name::King; v++)
			{
				decks[Deal + 1]->add(static_cast <Card::Type> (t), v);
				decks[Deal + 1]->back().flipped = true;
			}
		}

		decks[Deal + 1]->shuffle();
		decks[Deal + 1]->toggleCount();
		decks[Deal + 0]->limitVisible(dealCount);

		for(size_t i = 0; i < 7; i++)
		{
			decks[Deal + 1]->moveTo(*decks[Field + i], i + 1, true, false);
			decks[Field + i]->back().flipped = false;
		}
	}

	void onRender(Akimbo::Render& render) override
	{
		render.color(0.0f, 0.3f, 0.0f);
		render.clear();
	}

private:
	CardDeck* selected = nullptr;
	Card* selectedCard = nullptr;

	Vec2 cardSize;
	unsigned dealCount;

	std::function <void(CardDeck&, Card*)> onClick;
	Akimbo::TextureAtlas cardAtlas;

	Overlay* overlay;
	CardDeck* decks[14];
};

int main()
{
	Cards c;
	c.start();
}
