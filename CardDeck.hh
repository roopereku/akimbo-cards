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
	CardDeck(Akimbo::TextureAtlas& atlas, Scatter scatter, std::function <void(CardDeck&, Card*)>& onClick)
		: scatter(scatter), atlas(atlas), onClick(onClick)
	{
		cardSize = Card::size() * atlas.getAspectRatio();
		//setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	void onRender(Akimbo::Render& render) override;
	void onMouseClick(Vec2 at, int which) override;
	void onResize() override;

	void toggleCount();
	void limitHorizontalVisibility(unsigned count);

	size_t moveTo(CardDeck& to, Card& card, bool top = true, bool flip = false, bool inverse = false);
	size_t moveTo(CardDeck& to, size_t count, bool top = true, bool flip = false, bool inverse = false);

	void flipAll(bool flipped);

	void add(Card::Type t, unsigned value, bool top = true);
	void add(Card card, bool top = true);

	void shuffle();
	void pop(size_t dive = 0);

	bool empty() { return cards.empty(); }
	size_t count() { return cards.size(); }

	size_t getCardIndex(Card& card);
	Card& operator[](size_t index) { return cards[index]; }

	Card& getTop(size_t dive = 0) { return cards[cards.size() - 1 - dive]; }
	Vec2 getTopPosition();

private:
	void fitVertically();
	void renderCard(Akimbo::Render& render, Card& card, Vec2 position);

	Vec2 cardSize;
	Vec2 shift;

	Scatter scatter;

	bool displayCount = false;
	unsigned maxVisible = 0;

	std::deque <Card> cards;
	Akimbo::TextureAtlas& atlas;
	std::function <void(CardDeck&, Card*)>& onClick;
};

#endif
