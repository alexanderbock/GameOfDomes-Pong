set(SRC 
    game/gameengine.cpp
    game/havokengine.cpp
    game/player.cpp
    game/settings.cpp
    objects/ball.cpp
    objects/fence.cpp
    objects/goodie.cpp
    objects/object.cpp
    objects/paddle.cpp
    objects/target.cpp
    util/boundingbox.cpp
    util/cartesianpair.cpp
    util/objectfactory.cpp
    util/sphericalpair.cpp)

add_executable(GOD_Pong main ${SRC})