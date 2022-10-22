#ifndef CARD_DECK_HH
#define CARD_DECK_HH

#include "Card.hh"

#include <akimbo/ui/Widget.hh>
#include <akimbo/TextureAtlas.hh>

#include <functional>
#include <deque>

enum class Scatter
{
	None,
	Vertically,
	Horizontally
};


class CardDeck : public Akimbo::UI::Widget
{
public:
	CardDeck(Akimbo::TextureAtlas& atlas, Scatter scatter, std::function <void(CardDeck&)>& onClick)
		: scatter(scatter), atlas(atlas), onClick(onClick)
	{
		cardSize = Card::size() * atlas.getAspectRatio();
		//setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	void onRender(Akimbo::Render& render) override;
	void onMouseClick(Vec2 at, int which) override;

	void add(Card card);
	void add(Card::Type t, unsigned value);

	Card pop();

	bool empty() { return cards.empty(); }
	size_t count() { return cards.size(); }

	Card& front() { return cards.front(); }
	Card& back() { return cards.back(); }

private:
	void renderCard(Akimbo::Render& render, Card& card, Vec2 position);
	Vec2 cardSize;

	Scatter scatter;

	std::deque <Card> cards;
	Akimbo::TextureAtlas& atlas;
	std::function <void(CardDeck&)>& onClick;
};

#endif
