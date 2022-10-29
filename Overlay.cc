#include "Overlay.hh"
#include "akimbo/Debug.hh"

void Overlay::renderCard(Akimbo::Render& render, Animation& a)
{
	int atlasX;
	int atlasY;

	if(a.card.flipped)
	{
		atlasX = 0;
		atlasY = 4;
	}

	else
	{
		atlasX = a.card.value == Card::Name::Ace ? 12 : a.card.value - 2;
		atlasY = static_cast <int> (a.card.t);
	}

	render.fromAtlas(cardAtlas, atlasX, atlasY, a.position, cardSize);
	
	//render.color(1.0f, 1.0f, 0.0f, 0.5f);
	//render.box(a.position, Vec2(0.2f, 0.2f), true);

	//render.color(0.0f, 1.0f, 1.0f, 0.5f);
	//render.box(a.destination, Vec2(0.2f, 0.2f), true);
}

void Overlay::onRender(Akimbo::Render& render)
{
	render.color(1.0f, 1.0f, 1.0f);
	for(auto& a : animations)
		renderCard(render, a);
}

void Overlay::onMouseClick(Vec2, int)
{
	DBG_LOG("overlay click");
}

void Overlay::onUpdate(double delta)
{
	bool dirty = false;

	for(size_t i = 0; i < animations.size(); i++)
	{
		Animation& a = animations[i];

		if(a.timer <= 0.0)
		{
			a.to->add(a.card);
			animations.erase(animations.begin() + i);
			dirty = true;

			if(animations.empty())
				receiveMouseEvents = false;

			continue;
		}

		Vec2 shift = a.direction * a.distance * static_cast <float> (delta);
		a.position += shift;

		dirty = true;
		a.timer -= delta;
	}

	if(dirty) render();
}

void Overlay::animateCardIndex(CardDeck& from, CardDeck& to, size_t index, double seconds)
{
	Animation a;

	a.to = &to;
	a.position = from.getRealPosition(from.getTopPosition(), *parent);

	Vec2 dest = to.getRealPosition(to.getTopPosition(), *parent);
	a.destination = dest;

	a.distance = glm::distance(a.position, dest) / seconds;
	float angle = atan2(a.position.y - dest.y, a.position.x - dest.x);

	a.direction = -Vec2(cos(angle), sin(angle));
	a.card = from.getTop(index);
	a.timer = seconds;

	from.pop(index);
	animations.push_back(a);
	receiveMouseEvents = true;
}

void Overlay::animateCard(CardDeck& from, CardDeck& to, size_t count, double seconds, bool inverse)
{
	if(!inverse)
	{
		for(size_t i = 0; i < count; i++)
		{
			animateCardIndex(from, to, i, seconds);
			seconds += 0.1f;
		}
	}

	else
	{
		for(int i = count - 1; i >= 0; i--)
		{
			animateCardIndex(from, to, i, seconds);
			seconds += 0.1f;
		}
	}
}

void Overlay::animateCard(CardDeck& from, CardDeck& to, Card& start, double seconds, bool inverse)
{
	size_t count = from.count() - from.getCardIndex(start);
	animateCard(from, to, count, seconds, inverse);
}
