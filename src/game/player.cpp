//
//  player.cpp
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#include "player.h"

Player::Player() 
    : _leftBorder(0.f)
    , _rightBorder(0.f)
{}

void Player::setBorder(float left, float right) {
    _leftBorder = left;
    _rightBorder = right;
}

std::pair<float, float> Player::border() const {
    return std::make_pair(_leftBorder, _rightBorder);
}

void Player::setPaddle(Paddle* paddle) {
    _paddle = paddle;
}

Paddle* Player::paddle() {
    return _paddle;
}

