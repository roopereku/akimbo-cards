#ifndef BLACKJACK_HH
#define BLACKJACK_HH

#include "Game.hh"
#include "CardDeck.hh"

#include <functional>

#include <akimbo/ui/Console.hh>

class BlackJack : public Game
{
public:
	enum Deck
	{
		DealerHand = 1, 
		DealerDeck,
		PlayerHand
	};

	enum Action
	{
		Hit,
		Stand,
		Double,
		Split
	};

	BlackJack(Akimbo::TextureAtlas& cardAtlas);
	void restart(bool first) override;
	void update(double delta) override;
	
	CardDeck& deck(Deck d);
private:
	Akimbo::UI::Console& console;
	std::function<void(CardDeck&, Card*)> onClick;

	void giveCards(CardDeck& target, int amount);

	void performMove(Action action);

	int dealerSum;
	int playerSum;

	bool isGameOver();

	bool playerTurn = true;
};

#endif
