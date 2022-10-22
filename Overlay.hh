#ifndef OVERLAY_HH
#define OVERLAY_HH

#include "CardDeck.hh"

#include <deque>

class Overlay : public Akimbo::UI::Widget
{
public:
	Overlay()
	{
		setBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
		receiveMouseEvents = false;
	}

	void onRender(Akimbo::Render& render) override;
	void onMouseClick(Vec2, int) override;
	void onUpdate(double delta) override;

	void animateCard(CardDeck& from, CardDeck& to, double seconds);

private:
	struct Animation
	{
		CardDeck* from;
		CardDeck* to;

		double stepDuration;
		double elapsed;

		unsigned steps;
		unsigned stepsMax;

		Vec2 position;
		Vec2 direction;
	};

	std::deque <Animation> animations;
};

#endif
