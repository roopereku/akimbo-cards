#include "CardDeck.hh"

void CardDeck::renderCard(Akimbo::Render& render, Card& card, Vec2 position)
{
	int atlasX = card.value == Card::Name::Ace ? 12 : card.value - 2;
	int atlasY = static_cast <int> (card.t);

	if(card.highlighted)
	{
		render.color(card.highlight);
		render.box(position, cardSize, true);
		render.color(1.0f, 1.0f, 1.0f);
	}

	Vec2 gap(0.01f, 0.01f);
	render.fromAtlas(atlas, atlasX, atlasY, position + gap, cardSize - gap * 2.0f);
}

void CardDeck::onRender(Akimbo::Render& render)
{
	Vec2 fill = render.radius * 2.0f;
	Vec2 shift;

	switch(scatter)
	{
		case Scatter::Vertically:
			cardSize.x = fill.x;
			cardSize.y = fill.x / atlas.getAspectRatio().x;

			shift = Vec2(0.0f, cardSize.y / 3.0f);
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

	render.color(1.0f, 1.0f, 1.0f);
	Vec2 position = render.topLeft;

	for(auto& card : cards)
	{
		renderCard(render, card, position);
		position += shift;
	}
}

void CardDeck::onMouseClick(Vec2 at, int)
{
	if(onClick)
	{
		onClick(*this);
		render();
	}
}

Card CardDeck::pop()
{
	Card card = cards.back();
	cards.pop_back();

	render();
	return card;
}

void CardDeck::add(Card card)
{
	cards.push_back(card);
	render();
}

void CardDeck::add(Card::Type t, unsigned value)
{
	cards.emplace_back(t, value);
	render();
}
