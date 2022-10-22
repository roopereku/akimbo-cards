#include "Overlay.hh"
#include "akimbo/Debug.hh"

void Overlay::onRender(Akimbo::Render& render)
{
}

void Overlay::onMouseClick(Vec2, int)
{
	DBG_LOG("overlay click");
}

void Overlay::onUpdate(double delta)
{
	for(auto& a : animations)
	{
		a.elapsed += delta;

		if(a.elapsed >= a.stepDuration)
		{
		}
	}
}

void Overlay::animateCard(CardDeck& from, CardDeck& to, double seconds)
{
	Animation a;

	a.from = &from;
	a.to = &to;
}
