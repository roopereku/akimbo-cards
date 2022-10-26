#ifndef SOLITAIRE_HH
#define SOLITAIRE_HH

#include "Game.hh"
#include "Overlay.hh"
#include "CardDeck.hh"

#include <functional>

class Solitaire : public Game
{
public:
	enum Deck
	{
		Safe = 0,
		Deal = 4,
		Field = 6
	};

	Solitaire(Akimbo::TextureAtlas& cardAtlas);

	void restart(bool first) override;
	void update(double delta) override;

private:
	CardDeck* selected = nullptr;
	Card* selectedCard = nullptr;

	Akimbo::UI::Label* timerLabel;
	Akimbo::UI::Label* movesLabel;
	Akimbo::UI::Label* scoreLabel;

	int score = -1;
	int moves = -1;

	void updateInfo(int newScore, int newMoves);

	CardDeck& deck(Deck type, size_t index);
	CardDeck& deck(size_t index);

	double elapsed = 0.0;
	int secondsElapsed = 0;

	Vec2 cardSize;
	unsigned dealCount = 1;
	bool keepScore = true;

	std::function <void(CardDeck&, Card*)> onClick;
	Overlay* overlay;

	bool cardFits(CardDeck& deck, Card& card, bool& isSafe);
};

#endif
