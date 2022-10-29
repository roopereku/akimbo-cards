#ifndef OVERLAY_HH
#define OVERLAY_HH

#include "CardDeck.hh"

#include <deque>

class Overlay : public Akimbo::UI::Widget
{
public:
	Overlay(Akimbo::TextureAtlas& cardAtlas) : cardAtlas(cardAtlas)
	{
		cardSize = Card::size() * cardAtlas.getAspectRatio();

		setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
		receiveMouseEvents = false;
	}

	void onRender(Akimbo::Render& render) override;
	void onMouseClick(Vec2, int) override;
	void onUpdate(double delta) override;

	void animateCard(CardDeck& from, CardDeck& to, Card& start, double seconds, bool inverse = false);
	void animateCard(CardDeck& from, CardDeck& to, size_t count, double seconds, bool inverse = false);

private:
	void animateCardIndex(CardDeck& from, CardDeck& to, size_t index, double seconds);

	struct Animation
	{
		Card card;
		CardDeck* to;

		double timer;

		unsigned steps;

		Vec2 position;
		Vec2 destination;

		Vec2 direction;
		float distance;
	};

	void renderCard(Akimbo::Render& render, Animation& a);

	Akimbo::TextureAtlas& cardAtlas;
	std::deque <Animation> animations;

	Vec2 cardSize;
};

#endif
