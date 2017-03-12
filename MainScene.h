#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <cstdlib>
#include <ctime>
using namespace cocos2d;

class GameObject;//class factory of main game's objects

/**this class is the main world's scene**/
class HelloWorld : public cocos2d::Layer
{
public:
    static Scene* createScene();

    virtual bool init();	
    CREATE_FUNC(HelloWorld);

private:
	cocos2d::PhysicsWorld *_sceneWorld;

	bool _isTheGameOver;

	GameObject *_player;
	Label *_scoreLabel;
	Label *_gameOverLabel;
	Label *_restartLabel;

	Label* addLabel(const float fontSize, const char *text, Vec2 anchor, Vec2 position, bool visible, Color3B color);
    
	int _targetsAmount;
	int _roundTime;
	int _speed;

	Rect _screenRect;

	Sprite *_currentProjectile;

	int _scores;
	
	void updateScoreLabel();

	
	/**assign physics world to a pointer**/
	void _setPhysicsWorld(PhysicsWorld *world) { _sceneWorld = world; };

	/**this method create target's bodies**/
	void _createTargetsBodies(int amount, Size visualSize, Vec2 origin);

    //contact event
	bool onContactBegan(PhysicsContact &contact);

	//touch event
	bool onTouchBegan(Touch *touch, Event *unused_event);

	//game over event
	void gameOver(float dt);

    //update scene
	void update(float dt);

	//calculating projectile's velocity and  launch it
	void launchProjectile(Touch *touch);

	//restart game and delete old objects
	void restartGame();

	//animation of player when start shooting
	void playerThrow(Vec2 direction);

	//overcharging projectile and prepare it to launch
	void overcharge();

	//checking if projectile is out off screen
	void outScreenCheck();
};

#endif // __HELLOWORLD_SCENE_H__
