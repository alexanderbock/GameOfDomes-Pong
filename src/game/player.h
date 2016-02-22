//
//  player.h
//  Game of Domes - Pong
//
//  Created by Alexander Bock and Joel Kronander
//  Copyright (c) 2012. All rights reserved.
//

#ifndef __PONG_PLAYER_H__
#define __PONG_PLAYER_H__

#include <utility>

class Paddle;

class Player {
public:
    Player();

    void setBorder(float left, float right);
    std::pair<float, float> border() const;

    void setPaddle(Paddle* paddle);
    Paddle* paddle();

private:
    Paddle* _paddle;
    float _leftBorder;
    float _rightBorder;
};

#endif // __PONG_PLAYER_H__
