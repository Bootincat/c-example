#pragma once
#include "cocos2d.h"

/**this class is the factory of main game's objects**/

class GameObject : public cocos2d::Sprite
{
public:
	/**types of game's objects**/
	enum class Type {
		PLAYER,
		TARGET,
		PROJECTILE
	};

	/**categories for collision filtering**/
	enum class PhysicsCategory {
		None = 0,
		Targets = (0x01),    // 1
		Projectile = (0x02), // 2
		Edges = (0x03) // 3
	};

	/**create game objects with defferent types, declared in enum "Type"**/
	static GameObject* createWithFileName(std::string fileName, Type type, cocos2d::Vec2 position);

	

	GameObject();
	~GameObject();
};

