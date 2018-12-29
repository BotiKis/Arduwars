#pragma once

// license

#include <Arduboy2.h>

template<typename GameContextType, typename GameSceneType>
class EngineBoy;

template<typename GameContextType, typename GameSceneType>
class GameScene;

//////////////////////////////
// EngineBoy Engine

template<typename GameContextType, typename GameSceneType>
class EngineBoy
{
public:
	using GameSceneID = GameSceneType;
	using GameContext = GameContextType;

public:
	Arduboy2 arduboy;

private:
  // Used to calculate deltatime
  uint32_t _lastUpdateTimestamp = 0;
	uint32_t _deltaTime = 0;

protected:
  GameScene<GameContext, GameSceneID> *_currentScene;

public:
	// getter fopr deltatime
	uint32_t deltaTime() {return _deltaTime;}

	virtual GameContext & getContext(void) = 0;
	virtual const GameContext & getContext(void) const = 0;

	virtual void setup(void)
    {
        // Init arduboy related stuff
        arduboy.begin();
        arduboy.initRandomSeed();
        arduboy.setFrameRate(60);

        // init internal stuff
        _currentScene = nullptr;
        _lastUpdateTimestamp = millis();
    }

	virtual void update(void){
		// calculate delta time
		uint32_t currentTime = millis();
		_deltaTime = currentTime - _lastUpdateTimestamp;
		_lastUpdateTimestamp = currentTime;

		// Update arduboy
		arduboy.pollButtons();

		// Tell current scene to update
		if(_currentScene != nullptr)
			_currentScene->update(*this);
	}

	virtual void display(void)
    {
        // Render current scene
        arduboy.clear();
        if(_currentScene)
            _currentScene->render(*this);
        arduboy.display();
    }

	void changeToScene(GameSceneID sceneID)
	{
	  // Remember old scene
	  GameScene<GameContext, GameSceneID> *oldScene = _currentScene;

	  // get new scene
	  GameScene<GameContext, GameSceneID> *nextScene = this->gameSceneForSceneID(sceneID);

	  // call callbacks for will change
	  this->willChangeToScene(oldScene, nextScene);
	  if(oldScene != nullptr)
	      oldScene->willBecomeInactive(*this);
	  if(nextScene != nullptr)
	      nextScene->willBecomeActive(*this);

	  // store new scene
	  _currentScene = nextScene;

	  // call callbacks for didChange event
		if(oldScene != nullptr)
				oldScene->didBecomeInActive(*this);
	  if(nextScene != nullptr)
	      nextScene->didBecomeActive(*this);
	  this->didChangeToScene(oldScene, nextScene);
	}

protected:

  virtual GameScene<GameContext, GameSceneID>* gameSceneForSceneID(GameSceneID sceneID) = 0;

  virtual void willChangeToScene(GameScene<GameContext, GameSceneID> *previousScene, GameScene<GameContext, GameSceneID> *nextScene)
	{
		// Get rid of 'unused parameter' warnings
		(void)previousScene;
		(void)nextScene;
	}

	virtual void didChangeToScene(GameScene<GameContext, GameSceneID> *previousScene, GameScene<GameContext, GameSceneID> *nextScene)
	{
		// Get rid of 'unused parameter' warnings
		(void)previousScene;
		(void)nextScene;
	}
};


//////////////////////////////
// EngineBoy GameScene

template<typename GameContextType, typename GameSceneType>
class GameScene
{
public:
	using GameSceneID = GameSceneType;
	using GameContext = GameContextType;

	// In the implementation set this to the correct ID
	GameSceneID gameSceneID;

public:

  // Methods need to be implemented by subclass
	virtual void update(EngineBoy<GameContext, GameSceneID> & engine) = 0;
	virtual void render(EngineBoy<GameContext, GameSceneID> & engine) = 0;

  // Other methods
	virtual ~GameScene(void) {};

	virtual void willBecomeActive(EngineBoy<GameContext, GameSceneID> & engine)
	{
		// Get rid of 'unused parameter' warnings
		(void)engine;
	}

	virtual void didBecomeActive(EngineBoy<GameContext, GameSceneID> & engine)
	{
		// Get rid of 'unused parameter' warnings
		(void)engine;
	}

	virtual void willBecomeInactive(EngineBoy<GameContext, GameSceneID> & engine)
	{
		// Get rid of 'unused parameter' warnings
		(void)engine;
	}

	virtual void didBecomeInActive(EngineBoy<GameContext, GameSceneID> & engine)
	{
		// Get rid of 'unused parameter' warnings
		(void)engine;
	}
};
