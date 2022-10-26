#include "CardDeck.hh"
#include <akimbo/Core.hh>

#include <algorithm>
#include <random>

void CardDeck::renderCard(Akimbo::Render& render, Card& card, Vec2 position)
{
	int atlasX;
	int atlasY;

	if(card.flipped)
	{
		atlasX = 0;
		atlasY = 4;
	}

	else
	{
		atlasX = card.value == Card::Name::Ace ? 12 : card.value - 2;
		atlasY = static_cast <int> (card.t);
	}

	if(card.highlighted)
	{
		render.color(card.highlight);
		render.box(position, cardSize, true);
		render.color(1.0f, 1.0f, 1.0f);
	}

	Vec2 gap(0.01f, 0.01f);
	render.fromAtlas(atlas, atlasX, atlasY, position + gap, cardSize - gap * 2.0f);
}

void CardDeck::onResize()
{
	Vec2 fill = view.radius * 2.0f;

	switch(scatter)
	{
		case Scatter::Vertically:
			cardSize.x = fill.x;
			cardSize.y = fill.x / atlas.getAspectRatio().x;

			shift = Vec2(0.0f, cardSize.y / 4);
		break;

		case Scatter::Horizontally:
			cardSize = Vec2(fill.y * atlas.getAspectRatio().x, fill.y);
			shift = Vec2(fill.x / 6, 0.0f);
		break;

		case Scatter::None:
			cardSize = fill;
			shift = Vec2(0.0f, 0.0f);
		break;
	}
}

void CardDeck::onRender(Akimbo::Render& render)
{
	render.color(1.0f, 1.0f, 1.0f);
	Vec2 position = view.topLeft;

	size_t start = maxVisible == 0 ? 0 : cards.size() - maxVisible;
	for(size_t i = start; i < cards.size(); i++)
	{
		renderCard(render, cards[i], position);
		position += shift;
	}

	if(displayCount > 0)
	{
		std::string c = std::to_string(cards.size());
		render.text(c, core->getDefaultFont(), view.topLeft, view.radius * Vec2(1, 0.5f));
	}
}

void CardDeck::onMouseClick(Vec2 at, int)
{
	if(onClick)
	{
		Vec2 position = view.topLeft;
		Card* selected = nullptr;

		size_t start = maxVisible == 0 ? 0 : cards.size() - maxVisible;
		for(size_t i = start; i < cards.size(); i++)
		{
			if(at >= position && at <= position + cardSize)
				selected = &cards[i];

			position += shift;
		}

		onClick(*this, selected);
		render();
	}
}

void CardDeck::shuffle()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::shuffle(cards.begin(), cards.end(), gen);
	render();
}

void CardDeck::pop()
{
	cards.pop_back();
	render();
}

size_t CardDeck::moveTo(CardDeck& to, size_t count, bool top, bool flip)
{
	if(count > cards.size())
		count = cards.size();

	if(count == 0)
		return 0;

	Card& card = cards[cards.size() - count];
	return moveTo(to, card, top, flip);
}

size_t CardDeck::moveTo(CardDeck& to, Card& card, bool top, bool flip)
{
	auto it = std::find(cards.begin(), cards.end(), card);

	size_t index = std::distance(cards.begin(), it);
	size_t count = cards.size() - index;

	for(size_t i = index; i < cards.size(); i++)
	{
		if(flip) cards[i].flipped = !cards[i].flipped;
		to.add(cards[i], top);
	}

	cards.erase(cards.begin() + index, cards.end());
	to.render();
	render();

	return count;
}

void CardDeck::add(Card card, bool top)
{
	if(top) cards.push_back(card);
	else cards.push_front(card);

	render();
}

void CardDeck::flipAll(bool flipped)
{
	for(auto& c : cards)
		c.flipped = flipped;

	render();
}

void CardDeck::toggleCount()
{
	displayCount = !displayCount;
	render();
}

void CardDeck::limitVisible(unsigned count)
{
	maxVisible = count;
	render();
}

void CardDeck::add(Card::Type t, unsigned value, bool top)
{
	add(Card(t, value), top);
}
