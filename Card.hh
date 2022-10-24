#ifndef CARD_HH
#define CARD_HH

#include <akimbo/Vector2.hh>
#include <akimbo/Vector3.hh>

struct Card
{
	enum Type
	{
		Hearts,
		Diamonds,
		Clubs,
		Spades,
		None
	};

	enum Name
	{
		Ace = 1,
		Jack = 11,
		Queen,
		King,
	};

	Card() {}
	Card(Type t, unsigned value) : value(value), t(t) {}

	bool operator==(const Card& rhs)
	{
		return value == rhs.value && t == rhs.t;
	}

	bool isRed() { return t == Type::Diamonds || t == Type::Hearts; }
	bool isBlack() { return t == Type::Clubs || t == Type::Spades; }

	bool flipped = true;
	unsigned value;
	Type t;

	bool highlighted = false;
	Vec3 highlight;

	static Vec2 size()
	{
		return Vec2(0.4f, 0.4f);
	}
};

#endif
