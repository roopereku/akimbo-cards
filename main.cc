#include "CardDeck.hh"
#include "Overlay.hh"
#include "Card.hh"

#include <akimbo/Core.hh>

#include <random>

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

			return card.t == deck.back().t && card.value == deck.back().value - 1;
		}
	}

	Cards() : cardAtlas("resources/cardatlas.jpg", 13, 5)
	{
		cardSize = Card::size() * cardAtlas.getAspectRatio();
		dealCount = 1;

		onClick = [this](CardDeck& deck)
		{
			if(&deck == decks[Deal + 1])
			{
				if(selected)
					return;

				if(decks[Deal + 0]->count() == 3)
				{
					while(!decks[Deal + 0]->empty())
						decks[Deal + 0]->pop();
				}

				std::random_device rd;
				std::mt19937 gen(rd());

				std::uniform_int_distribution <unsigned> distV(Card::Name::Ace, Card::Name::King);
				std::uniform_int_distribution <unsigned> distT(Card::Type::Hearts, Card::Type::Spades);

				decks[Deal + 0]->add(Card::Type::Spades, distV(gen));
				//decks[Deal + 0]->add(static_cast <Card::Type> (distT(gen)), Card::Name::Ace);
				return;
			}

			if(selected == nullptr)
			{
				if(deck.empty())
					return;

				selected = &deck;
				selectedCard = &deck.back();

				selectedCard->highlighted = true;
				selectedCard->highlight = Vec3(1.0f, 1.0f, 0.0f);
			}

			else if(selected == &deck)
			{
				selectedCard->highlighted = false;
				selected = nullptr;
			}

			else if(&deck != decks[Deal + 0])
			{
				if(!cardFits(deck, *selectedCard))
					return;

				selectedCard->highlighted = false;
				deck.add(*selectedCard);

				selected->pop();
				selected->render();

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

	std::function <void(CardDeck&)> onClick;
	Akimbo::TextureAtlas cardAtlas;

	Overlay* overlay;
	CardDeck* decks[14];
};

int main()
{
	Cards c;
	c.start();
}
