#include "GameObject.h"
#include "Constants.h"



GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

GameObject* GameObject::createWithFileName(std::string fileName, Type type, cocos2d::Vec2 position) {
	auto sprite = new GameObject;
	if (sprite && sprite->initWithFile(fileName)){ //if sprite created successfuly
		sprite->autorelease();

		switch (type)
		{
		case GameObject::Type::PLAYER:
			sprite->setPosition(cocos2d::Vec2(position.x, position.y));
			break;
		case GameObject::Type::TARGET:
		{
			/**target's physic body**/
			sprite->setPosition(cocos2d::Vec2(position.x - sprite->getContentSize().width / 2, position.y));//sprite's position
			auto targetBody = cocos2d::PhysicsBody::createCircle(sprite->getContentSize().width / 2, cocos2d::PhysicsMaterial(0, 1, 0));//create physics body
			targetBody->setDynamic(true);//it is dynamic body now

			/**collision filtering**/
			targetBody->setCategoryBitmask((int)PhysicsCategory::Targets);//object’s type
			targetBody->setCollisionBitmask((int)PhysicsCategory::Edges);//body will collide with..
			targetBody->setContactTestBitmask((int)PhysicsCategory::Projectile | (int)PhysicsCategory::Targets);//with which collisions should it generate notifications

			targetBody->setVelocity(cocos2d::Vec2(cocos2d::random(-300, 300), cocos2d::random(-300, 300)));//set random velocity for targets
			sprite->setPhysicsBody(targetBody);//attach physics body to sprite
		}
			break;
		case GameObject::Type::PROJECTILE:
		{
			sprite->setPosition(position);

			/**projectile's physic body**/
			auto projectileBody = cocos2d::PhysicsBody::createCircle(sprite->getContentSize().width / 2,//radius
			cocos2d::PhysicsMaterial(0, 1, 0),//material
			cocos2d::Vec2(0, 0));//offset
			projectileBody->setDynamic(true);//it is dynamic body now
			sprite->setPhysicsBody(projectileBody);//adding this body to a node

			/**particle effect**/
			cocos2d::ParticleSystemQuad *_emitter;
			_emitter = cocos2d::ParticleSystemQuad::create(cocos2d::FileUtils::getInstance()->fullPathForFilename(launchParticleName));//create new particle system from xml
			_emitter->setPositionType(cocos2d::ParticleSystem::PositionType::RELATIVE);//attach particle position to emmiter
			_emitter->setPosition(cocos2d::Vec2(sprite->getContentSize().width / 2, 0));//particle system position
			sprite->addChild(_emitter, -1);//add particle system

			/**collision filtering**/
			projectileBody->setCategoryBitmask((int)GameObject::PhysicsCategory::Projectile); //object’s type
			projectileBody->setCollisionBitmask((int)GameObject::PhysicsCategory::None); //body will collide with..
			projectileBody->setContactTestBitmask((int)GameObject::PhysicsCategory::Targets); //with which collisions should it generate notifications
		}
			break;
		}

		return sprite;
	}

	CC_SAFE_DELETE(sprite);
	return nullptr;
}