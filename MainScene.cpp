#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "Constants.h"
#include "FileHandler.h"
#include "GameObject.h"

USING_NS_CC;


Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setGravity(Vec2(0, 0));//setting up world's gravity
    // 'layer' is an autorelease object
    auto layer = MainScene::create();
	layer->_setPhysicsWorld(scene->getPhysicsWorld());

    // add layer as a child to scene
    scene->addChild(layer);
    // return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    //super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/**game settings*/
	FileHandler::readSettingsFromFile(&_targetsAmount,&_speed,&_roundTime);//reading settings from input file

	/**add new directory for searching paths*/
	FileUtils::getInstance()->addSearchPath("Sprites");
	FileUtils::getInstance()->addSearchPath("Particles");
	FileUtils::getInstance()->addSearchPath("fonts");

	/**bacground*/
	Sprite *backgroundSprite = Sprite::create(cocos2d::FileUtils::getInstance()->fullPathForFilename(backgroundSpriteName));
	backgroundSprite->setPosition(Vec2(visibleSize.width/2 + origin.x,visibleSize.height/2 + origin.y));
	this->addChild(backgroundSprite);

	/**labels*/
	_scoreLabel = addLabel(smallFontSize, "Score 0", ccp(0.5f, 0.5f), Vec2(visibleSize.width/2, visibleSize.height * 0.93f),true, Color3B::RED);
	_gameOverLabel = addLabel(largeFontSize, "Game Over", ccp(0.5f, 0.5f), Vec2(visibleSize.width/2, visibleSize.height *0.75f),false, Color3B::RED);
	_gameOverLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	_restartLabel = addLabel(mediumFontSize, "Click to RESTART", ccp(0.5f, 0.5f), Vec2(visibleSize.width / 2, visibleSize.height*0.25f), false, Color3B::WHITE);
	
	Size edgeSize = Size(visibleSize.width*0.8f, visibleSize.height*0.85f); //creating edge box with size of a screen
	auto edgeBody = PhysicsBody::createEdgeBox(edgeSize, PhysicsMaterial(0.1f, 1, 0.0f), 3);//box area
	edgeBody->setDynamic(false);//it is dynamic body now
	edgeBody->setCategoryBitmask((int) GameObject::PhysicsCategory::Edges);  //object’s type
	edgeBody->setCollisionBitmask((((int) GameObject::PhysicsCategory::Targets))); //body will collide with..

	//creating node for edge body and setting up it's position on screen
	auto edgeNode = Node::create();
	edgeNode->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	edgeNode->setPhysicsBody(edgeBody);

	//adding edge node to the scene
	this->addChild(edgeNode);

	/**player**/
	_player = GameObject::createWithFileName(cocos2d::FileUtils::getInstance()->fullPathForFilename(playerSpriteName),               //creating player's sprite
		                                     GameObject::Type::PLAYER, 
		                                     Vec2(visibleSize.width / 2, 0));
	_player->setAnchorPoint(Vec2(0.5f,0.65f));
	this->addChild(_player,50);

	/**prepare projectile**/
	overcharge();

	/**touch listener**/
	auto eventListener = EventListenerTouchOneByOne::create();
	eventListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);//on click event
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, _player);

	/**contact listener**/
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(MainScene::onContactBegan, this);//when body starts to collide we'll know it
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

	_scores = 0;//reset score

	_screenRect = this->getBoundingBox();//screen limits
	
	_isTheGameOver = false;

	/**targets**/
	_createTargetsBodies(_targetsAmount, visibleSize, origin);//create target's objects

	this->scheduleUpdate();//schedule updating every frame
	scheduleOnce(schedule_selector(MainScene::gameOver),_roundTime);//count down timer

    return true;
}


bool MainScene::onTouchBegan(Touch *touch, Event *unused_event) {
	if (!_isTheGameOver){
		if (_currentProjectile != nullptr && _currentProjectile->getPhysicsBody()->getVelocity()== Vec2::ZERO) {//if projectile prepared for launch and not launched already
			launchProjectile(touch);//we just launch it
		}
	}
	else
	{
		restartGame();//if game is over, we restarting it by click
	}
	return true;
}

void MainScene::_createTargetsBodies(int amount, Size visibleSize, Vec2 origin) {
	srand(time(0));//input seed based on time to random generator
	Vec2 position = Vec2(visibleSize.width / 2, visibleSize.height/2);//start position of targets
	for (int i = 0; i < amount; i++) {
		auto targetSprite = GameObject::createWithFileName(cocos2d::FileUtils::getInstance()->fullPathForFilename(targetSpriteName),
			                                               GameObject::Type::TARGET,
			                                               position);//creating sprite for current target
		this->addChild(targetSprite);
    }
}

bool MainScene::onContactBegan(PhysicsContact &contact) {
	PhysicsBody* bodyA = contact.getShapeA()->getBody();
	PhysicsBody* bodyB = contact.getShapeB()->getBody();

	if (bodyA->getCategoryBitmask() != bodyB->getCategoryBitmask()) {
		auto nodeA = bodyA->getNode();
		auto nodeB = bodyB->getNode();
		Vec2 pos;
		/**aren't they already deleted?**/
		if (nodeA != nullptr) {
			nodeA->removeFromParentAndCleanup(true);
		}
		if (nodeB != nullptr) {
			pos = nodeB->getPosition();//position of explosion in the future
			nodeB->removeFromParentAndCleanup(true);
		}
		if (pos!=Vec2::ZERO) {//if position of collision was updated, we can explode collider
			ParticleSystemQuad *_emitter;
			_emitter = ParticleSystemQuad::create(cocos2d::FileUtils::getInstance()->fullPathForFilename(explosionParticleName));
			_emitter->setPositionType(ParticleSystem::PositionType::FREE);//free positioning in world space
			_emitter->setPosition(pos);
			_emitter->setAutoRemoveOnFinish(true);//release memory when done
			this->addChild(_emitter, 100);
		}
		_currentProjectile = nullptr;
		_scores++;
		return true;
	}

	return false;
}

void MainScene::gameOver(float dt) {
	/**earned points label**/
	char ScoreBuff[64];//buffer
	sprintf(ScoreBuff, "You've scored\n%d ", _scores);
	std::string scoreStr1 = ScoreBuff;

	sprintf(ScoreBuff, "of %d\npoints", _targetsAmount);
	std::string scoreStr2 = ScoreBuff;
	scoreStr1 += scoreStr2;
	_gameOverLabel->setString(scoreStr1);
	_gameOverLabel->setVisible(true);

	/**restart label**/
	_restartLabel->enableShadow();
	_restartLabel->setVisible(true);
	_isTheGameOver = true;

	/**scores label**/
	_scoreLabel->setVisible(false);
	this->getScene()->getPhysicsWorld()->setAutoStep(false);
}

void MainScene::update(float dt) {
	if (!_isTheGameOver)//if the game isn't over
	{
		if (_scores == _targetsAmount) {//if all targets destroyed, then the game is over
			gameOver(dt);
		}
		else {//if we have non-destroyed targets
			updateScoreLabel();//we just updating score label value
			if (_currentProjectile == nullptr)//if projectile is out of screen or it is already destroyed
			{
				overcharge();//then we have to prepare projectile for the next shoot
			}
			else
			{
				outScreenCheck();//if projectile isn't destroyed and don't colliding with targets, we are checking for it's position
			}
		}
	}
}

/**this method is for creating label with custom settings**/
Label* MainScene::addLabel(const float fontSize, const char *text, Vec2 anchor, Vec2 position, bool visible, Color3B color) {
	
	Label *theLabel = Label::createWithTTF(text, cocos2d::FileUtils::getInstance()->fullPathForFilename(fontname), fontSize);//create a label's pointer
	theLabel->setAnchorPoint(anchor);//setting up anchor point
	theLabel->setPosition(position.x , position.y);//setiing up position
	theLabel->setColor(color);//setting up color
	theLabel->setVisible(visible);
	this->addChild(theLabel,40);

	return theLabel;
}

void MainScene::updateScoreLabel() {
	char ScoreString[64];//buffer
	sprintf(ScoreString, "Score: %d", _scores);
	_scoreLabel->setString(ScoreString);
}

/**creating projectile node with physic body inside and launching it**/
void MainScene::launchProjectile(Touch *touch) {
	Vec2 touchLocation = touch->getLocation();
	Vec2 offset = touchLocation - _player->getPosition();//compute direction for launching vector
	offset.normalize();

	/**rotate projectile and player accordingly direction of a touch**/
	float angle = CC_RADIANS_TO_DEGREES(offset.getAngle());
	_currentProjectile->setRotation(90- angle);
	_player->setRotation(90 -angle);

	playerThrow(offset);//push animation

	auto shootAmount = offset * 10 * _speed;//velocity vector of projectile

	/**apply body's velocity**/
	_currentProjectile->getPhysicsBody()->setVelocity(shootAmount);
}

void MainScene::restartGame() {
	auto scene = createScene();//creating new scene
	Director::getInstance()->replaceScene(scene);//replace new scene with old one, that will release all old game's objects
}

void MainScene::playerThrow(Vec2 direction) {
	Vec2 startPosition = _player->getPosition();
	Vec2 endPosition = startPosition + direction * 15;
	auto moveUp = MoveTo::create(0.05f, endPosition);//push animation
	
	auto moveDown = MoveTo::create(0.05f, startPosition);//return sprite to start position
	
	auto sequence = Sequence::create(moveUp, moveDown, nullptr);
	_player->runAction(sequence);//running sequence of actions

}

void MainScene::overcharge() {
	Vec2 position = Vec2(getContentSize().width / 2, 0);//start position of projectile
	_currentProjectile = GameObject::createWithFileName(cocos2d::FileUtils::getInstance()->fullPathForFilename(projectileSpriteName), GameObject::Type::PROJECTILE, position);//creating projectile sprite
	_currentProjectile->setAnchorPoint(Vec2(0.5f, 0));//middle-bot
	this->addChild(_currentProjectile, 30);
}

void MainScene::outScreenCheck() {
	/**if projectile is out of screen, then we are removing it**/
	if (!_screenRect.containsPoint(_currentProjectile->getPosition()))
	{
		_currentProjectile->removeFromParentAndCleanup(true);
		_currentProjectile = nullptr;
	}
}


