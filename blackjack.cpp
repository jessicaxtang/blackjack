#include <iostream>
#include <cstdlib> 
#include <ctime>
#include <iomanip>

using namespace std;

// GLOBAL CONSTANTS
const int STANDARD_CARDS = 52;
const int NUM_SUITS = 4;
const int NUM_RANKS = 13;
const int HAND_SIZE = 12;
const int BLACKJACK = 21;


// DECLARE STRUCTURES
struct Card{
  string suit;
  string description;
  int rank;
  int value;

  Card(){
    suit = "";
    description = "";
    rank = 0;
    value = 0;
  }
};

struct CardArray{
  Card* cards;
  int maxElements;
  int currentElements;

  CardArray(){
    cards  = nullptr;
    maxElements = 0;
    currentElements = 0;
  }
};

// FUNCTION PROTOTYPES
void getNewDeck(CardArray & deck);
void printDeck(const CardArray & deck);
void shuffleDeck(CardArray & deck);
int blackjack(CardArray & deck);
void deal(CardArray &hand, CardArray &deck);
void printHand(CardArray &hand);
int nextMove(CardArray &playerHand, CardArray &dealerHand, CardArray & deck);
int printDeal(CardArray &playerHand, CardArray &dealerHand, CardArray &deck);
int hit(CardArray &playerHand, CardArray &dealerHand, CardArray &deck);
int stand(CardArray &playerHand,CardArray &dealerHand, CardArray &deck);
int score(CardArray &hand);
void gameEnd(string reason, CardArray & playerHand, CardArray & dealerHand);
void playGames(CardArray &deck);
void interpretAces(CardArray &hand);
void advice(CardArray &playerHand, CardArray &dealerHand);


int main() {
  CardArray deck;
  getNewDeck(deck);
  printDeck(deck);

  cout << "\n\nshuffled";
  srand(time(0));
  shuffleDeck(deck);
  printDeck(deck);

  playGames(deck);
  //cout << endl << blackjack(deck);

  delete[] deck.cards;
  //return 0;
} 

// creating new standard deck of cards
void getNewDeck(CardArray & deck){

  string suits[] = {"spade", "heart", "diamond", "club"};
  string suitsDescription[] = {"S", "H", "D", "C"};
  string rankDescription[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
  int rank[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
  int value[] = {11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};
  
  deck.maxElements = STANDARD_CARDS;
  deck.currentElements = STANDARD_CARDS;
  deck.cards = new Card[STANDARD_CARDS];  

  for (int i = 0; i < deck.maxElements; i++){
    deck.cards[i].suit = suits[i/NUM_RANKS];
    deck.cards[i].description = rankDescription[i % NUM_RANKS];
    deck.cards[i].description += suitsDescription[i / NUM_RANKS];
    deck.cards[i].rank = rank[i % NUM_RANKS];
    deck.cards[i].value = value[i % NUM_RANKS];
  }
}

// print deck of cards
void printDeck(const CardArray & deck){
  cout << fixed;
  for (int i = 0; i < deck.maxElements; i++){
    if (i % NUM_RANKS == 0){
      cout << endl;
    }
    cout << setw(3) << deck.cards[i].description << " ";
  }
}

// shuffle deck of cards
void shuffleDeck(CardArray & deck){
  for (int i = 0; i < deck.maxElements; i++){
    int random = rand()%52;
    Card temp = deck.cards[i];
    deck.cards[i] = deck.cards[random];
    deck.cards[random] = temp;
  }
}

// playing blackjack function
int blackjack(CardArray & deck){
  // create empty dealer hand
  CardArray dealerHand;
  dealerHand.cards = new Card[HAND_SIZE];
  dealerHand.maxElements = HAND_SIZE;
  dealerHand.currentElements = 0;

  // create empty player hand
  CardArray playerHand;
  playerHand.cards = new Card[HAND_SIZE];
  playerHand.maxElements = HAND_SIZE;
  playerHand.currentElements = 0;

  return printDeal(playerHand, dealerHand, deck);

  // create second deck for continuous play
  CardArray deck2;
  shuffleDeck(deck2);
  if (deck.currentElements < 4){
    blackjack(deck2);
  }

  delete[] dealerHand.cards;
  delete[] playerHand.cards;
}

// deal cards - remove top card from deck and insert into first available space in hand
void deal(CardArray &hand, CardArray &deck){
  hand.cards[hand.currentElements] = deck.cards[deck.currentElements-1];
  hand.currentElements += 1;
  deck.currentElements -= 1;
}

// print cards in hand
void printHand(CardArray &hand){ 
  for (int i = 0; i < hand.currentElements; i++){
    cout << hand.cards[i].description << " ";
  }
}

// show the process of dealing 1st and 2nd cards
int printDeal(CardArray &playerHand, CardArray &dealerHand, CardArray &deck){
  
  // Print dealing of the first card
  cout << "\n\nDeal first card" << "\n*****************" << endl;
  deal(playerHand, deck);
  cout << "+Player+: ";
  printHand(playerHand);
  deal(dealerHand, deck);
  cout << "\n*Dealer*: ";
  printHand(dealerHand);
  
  // Print dealing of the second card
  cout << "\n\nDeal second card" << "\n*****************" << endl;
  deal(playerHand, deck);
  cout << "+Player+: ";
  printHand(playerHand);
  deal(dealerHand, deck);
  cout << "\n*Dealer*: " << dealerHand.cards[0].description << " ??";

  // check if aces should be 1 or 11 in situation
  interpretAces(playerHand);
  interpretAces(dealerHand);

  // check if anyone has won/drawn with first 2 rounds of dealing
  if(score(playerHand) == BLACKJACK && score(dealerHand) != BLACKJACK){
    gameEnd("player_blackjack", playerHand, dealerHand);
    return 1;
  }
  if(score(playerHand) != BLACKJACK && score(dealerHand) == BLACKJACK){
    gameEnd("dealer_blackjack", playerHand, dealerHand);
    return -1;
  }
  if(score(playerHand) == BLACKJACK && score(dealerHand) == BLACKJACK){
    gameEnd("draw", playerHand, dealerHand);
    return 0;
  }
  
  // Print dealing to player, with option to hit or stand
  cout << "\n\nDealing to player" << "\n*****************";
  return nextMove(playerHand, dealerHand, deck);
}

// prints nextMove as result of player's move (stand or hit)
int nextMove(CardArray &playerHand, CardArray &dealerHand, CardArray & deck){
  string move = "";
  advice(playerHand, dealerHand);
  cout << "\nEnter h to hit or s to stand: ";
  cin >> move;

  while (move != "h" && move != "H" && move != "s" && move != "S"){
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "please enter either the letter h or s: ";
    cin >> move;
  }

  if (move == "h" || move == "H"){
    return hit(playerHand, dealerHand, deck);
  }

  if (move == "s" || move == "S"){
    return stand(playerHand, dealerHand, deck);
  }
}

// if player hits
int hit(CardArray &playerHand, CardArray &dealerHand, CardArray &deck){
  deal(playerHand, deck);
  interpretAces(playerHand);
  
  if (score(playerHand) > BLACKJACK){
    gameEnd("player_bust", playerHand, dealerHand);
    return -1;
  }
  if(score(playerHand) < BLACKJACK){
    cout << "+Player+: ";
    printHand(playerHand);
    nextMove(playerHand, dealerHand, deck);
  }
  if(score(playerHand) == BLACKJACK){
    gameEnd("player_blackjack", playerHand, dealerHand);
    return 1;
  }
  //return 0;
}

// if player chooses to stand
int stand(CardArray &playerHand, CardArray &dealerHand, CardArray &deck){
  cout << "\nDealing to dealer" << "\n*****************" << endl;
  interpretAces(dealerHand);
  while (score(dealerHand) < 17){
    deal(dealerHand, deck);
    cout << "*Dealer*: ";
    printHand(dealerHand);
    cout << endl;
  }
  if(score(dealerHand) == BLACKJACK){
    gameEnd("dealer_blackjack", playerHand, dealerHand);
    return -1;    
  }
  if (score(dealerHand) > BLACKJACK){
    gameEnd("dealer_bust", playerHand, dealerHand);
    return 1;
  }
  if (score(dealerHand) > 17 && score(dealerHand) < BLACKJACK){
    if (score(dealerHand) > score(playerHand)){
      gameEnd("dealer_greater", playerHand, dealerHand);
      return -1;
    }
    if (score(dealerHand) < score(playerHand)){
      gameEnd("player_greater", playerHand, dealerHand);
      return 1;
    }
    if (score(dealerHand) == score(playerHand)){
      gameEnd("draw", playerHand, dealerHand);
      return 0;
    }
  }
  //return 0;
}

// sum of card values in hand
int score(CardArray &hand){
  int sum = 0;
  for (int i = 0; i < hand.currentElements; i++){
    sum += hand.cards[i].value;
  }
  return sum;
}

// game ending explanation 
void gameEnd(string reason, CardArray &playerHand, CardArray &dealerHand){
  cout << "\nFINAL HANDS\n*****************\n" << "+Player+: ";
  printHand(playerHand);
  cout << endl;
  cout << "*Dealer*: ";
  printHand(dealerHand);
  cout << "\n\n";
  if (reason == "dealer_bust"){
    cout << "Dealer is bust, you win!";
  }
  if (reason == "player_bust"){
    cout << "Bust, you lose!";
  }
  if (reason == "dealer_blackjack"){
    cout << "Dealer's blackjack, you lose!";
  }
  if (reason == "player_blackjack"){
    cout << "Blackjack, you win!";
  }
  if (reason == "dealer_greater"){
    cout << "Dealer's score is greater, you lose!";
  }
  if (reason == "player_greater"){
    cout << "Your score is greater, you win!";
  }
  if (reason == "draw"){
    cout << "Player score = " << score(playerHand) << ", dealer score = " << score(dealerHand);
    cout << "\nGame is tied!";
  }
}

// play multiple rounds
void playGames(CardArray &deck){
  string play = "";
  int numGames = 0;
  int wins = 0;
  int losses = 0;
  int draws = 0;
  cout << "\n\nDo you want to play a hand of blackjack (y to play or n to exit)? ";
  cin >> play;
  while (play != "y" && play != "Y" && play != "n" && play != "N"){
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "please enter either the letter y or n: ";
    cin >> play;
  }
  while(play == "y" || play == "Y"){
    int gameResult = blackjack(deck);
    numGames += 1;

    if (gameResult == 1){
      wins += 1;
    }
    if (gameResult == -1){
      losses += 1;
    }
    if (gameResult == 0){
      draws += 1;
    }

    cout << "\n\nDo you want to play another hand of blackjack (y to play or n to exit)? ";
    cin >> play;
    while (play != "y" && play != "Y" && play != "n" && play != "N"){
      cin.clear();
      cin.ignore(10000, '\n');
      cout << "please enter either the letter y or n: ";
      cin >> play;
    }
  }

  cout << "<><><><><><><><><><><><><><><><><><><><><><><><><><><><><>\n";
  cout << "Thanks for playing, you played " << numGames << " games and your record was:";
  cout << "\nwins: " << wins;
  cout << "\nlosses: " << losses;
  cout << "\ndraws: " << draws;
}

// if aces should be 1 or 11
void interpretAces(CardArray &hand){
  int handSum =  score(hand);
  int const MAX_HAND_SUM_11 = 10;
  for(int i = 0; i < hand.currentElements; i++){
    if (hand.cards[i].rank == 1){
      if (handSum > MAX_HAND_SUM_11){
        hand.cards[i].value = 1;
      }
      if (handSum <= MAX_HAND_SUM_11){
        hand.cards[i].value = 11;
      }
    }
  }
}


//advice
void advice(CardArray &playerHand, CardArray &dealerHand){
  if (dealerHand.cards[0].rank >=7 && dealerHand.cards[0].rank <= 11){
    if (score(playerHand) < 17){
      cout << "\nThe dealer's upcard is a good one (a 7, 8, 9, 10-card, or ace. You should draw since your hand total is " << score(playerHand) << ", which is less than  the strategy to draw until total sum is at least 17." << endl;
    }
    if (score(playerHand) > 17){
      cout << "\nThe dealer's upcard is a good one (a 7, 8, 9, 10-card, or ace. You should NOT draw (stand) since your hand total is " << score(playerHand) << ", which is greater than  the strategy to draw until total sum is at least 17." << endl;
    }
  }
  if (dealerHand.cards[0].rank <=7){
    if (score(playerHand) < 12){
      cout << "\nThe dealer's upcard is a bad one (a 4, 5, or 6). You should draw since your hand total is " << score(playerHand) << ", which is less than  the strategy to draw until total sum is at least 12." << endl;
    }
    if (score(playerHand) > 12){
      cout << "\nThe dealer's upcard is a bad one (a 4, 5, or 6). You should stand since your hand total is " << score(playerHand) << ", which is greater than  the strategy to draw until total sum is at least 12. Hope the dealer will bust when they hit." << endl;
    }
  }
}
