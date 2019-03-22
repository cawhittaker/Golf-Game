#include "NetworkPlayer.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int num)	{
	this->game = game;
	playerNum  = num;
}

NetworkPlayer::~NetworkPlayer()	{

}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject) {

}

void NetworkPlayer::OnCollisionBegin2(GameObject* otherObject) {

}