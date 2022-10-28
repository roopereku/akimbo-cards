#ifndef BLACKJACK_HH
#define BLACKJACK_HH

#include "Game.hh"
#include "CardDeck.hh"

#include <functional>

class BlackJack : public Game
{
public:
	enum class Deck
	{
		DealerHand, 
		DealerDeck,
		PlayerHand
	};

	BlackJack(Akimbo::TextureAtlas& cardAtlas);
	void restart(bool first) override;
	void update(double delta) override;
	
	CardDeck& deck(Deck d);
private:
	std::function<void(CardDeck&, Card*)> onClick;


};

#endif
