/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org
Copyright (c) 2008-2011 Ricardo Quesada
Copyright (c) 2011 Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __ACTION_CCINTERVAL_ACTION_H__
#define __ACTION_CCINTERVAL_ACTION_H__

#include "CCNode.h"
#include "CCAction.h"
#include "CCProtocols.h"
#include "CCSpriteFrame.h"
#include "CCAnimation.h"

namespace cocos2d {

	/** 
	@brief An interval action is an action that takes place within a certain period of time.
	It has an start time, and a finish time. The finish time is the parameter
	duration plus the start time.

	These CCActionInterval actions have some interesting properties, like:
	- They can run normally (default)
	- They can run reversed with the reverse method
	- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

	For example, you can simulate a Ping Pong effect running the action normally and
	then running it again in Reverse mode.

	Example:

	CCAction *pingPongAction = CCSequence::actions(action, action->reverse(), NULL);
	*/
	class CC_DLL CCActionInterval : public CCFiniteTimeAction
	{
	public:
		/** how many seconds had elapsed since the actions started to run. */
		inline float getElapsed(void) { return m_elapsed; }

		/** initializes the action */
		bool initWithDuration(float d);

		/** returns true if the action has finished */
		virtual bool isDone(void);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void step(float dt);
		virtual void startWithTarget(CCNode *pTarget);
		/** returns a reversed action */
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action */
		static CCActionInterval* create(float d);

	public:
		//extension in CCGridAction 
		void setAmplitudeRate(CGFloat amp);
		CGFloat getAmplitudeRate(void);

	protected:
		float m_elapsed;
		bool   m_bFirstTick;
	};

	/** @brief Runs actions sequentially, one after another
	*/
	class CC_DLL CCSequence : public CCActionInterval
	{
	public:
		~CCSequence(void);

		/** initializes the action */
		bool initWithTwoActions(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void stop(void);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		///** helper constructor to create an array of sequenceable actions */
		//static CCFiniteTimeAction* actions(CCFiniteTimeAction *pAction1, ...);
		///** helper contructor to create an array of sequenceable actions given an array */
		//static CCFiniteTimeAction* actionsWithArray(CCArray *actions);
		/** for Lua */
		static CCSequence* actionsWithArrayLua(CCArray *actions);

		/** helper constructor to create an array of sequenceable actions */
		static CCSequence* create(CCFiniteTimeAction *pAction1, ...);
		/** helper constructor to create an array of sequenceable actions given an array */
		static CCSequence* create(CCArray *arrayOfActions);
		/** helper constructor to create an array of sequence-able actions */
		static CCSequence* createWithVariableList(CCFiniteTimeAction *pAction1, va_list args);
		/** creates the action */
		static CCSequence* createWithTwoActions(CCFiniteTimeAction *pActionOne, CCFiniteTimeAction *pActionTwo);
	protected:
		CCFiniteTimeAction *m_pActions[2];
		float m_split;
		int m_last;
	};

	/** @brief Repeats an action a number of times.
	* To repeat an action forever use the CCRepeatForever action.
	*/
	class CC_DLL CCRepeat : public CCActionInterval
	{
	public:
		~CCRepeat(void);

		/** initializes a CCRepeat action. Times is an unsigned integer between 1 and pow(2,30) */
		bool initWithAction(CCFiniteTimeAction *pAction, unsigned int times);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void stop(void);
		virtual void update(float time);
		virtual bool isDone(void);
		virtual CCActionInterval* reverse(void);

		inline void setInnerAction(CCFiniteTimeAction *pAction)
		{
			if (m_pInnerAction != pAction)
			{
				CC_SAFE_RELEASE(m_pInnerAction);
				m_pInnerAction = pAction;
				CC_SAFE_RETAIN(m_pInnerAction);
			}
		}

		inline CCFiniteTimeAction* getInnerAction()
		{
			return m_pInnerAction;
		}

	public:
		/** creates a CCRepeat action. Times is an unsigned integer between 1 and pow(2,30) */
		static CCRepeat* create(CCFiniteTimeAction *pAction, unsigned int times);

	protected:
		unsigned int m_uTimes;
		unsigned int m_uTotal;
		/** Inner action */
		CCFiniteTimeAction *m_pInnerAction;
	};

	/** @brief Repeats an action for ever.
	To repeat the an action for a limited number of times use the Repeat action.
	@warning This action can't be Sequenceable because it is not an IntervalAction
	*/
	class CC_DLL CCRepeatForever : public CCActionInterval
	{
	public:
		CCRepeatForever()
			: m_pInnerAction(NULL)
		{}
		virtual ~CCRepeatForever();

		/** initializes the action */
		bool initWithAction(CCActionInterval *pAction);
		virtual CCObject* copyWithZone(CCZone *pZone);
		virtual void startWithTarget(CCNode* pTarget);
		virtual void step(float dt);
		virtual bool isDone(void);
		virtual CCActionInterval* reverse(void);

		inline void setInnerAction(CCActionInterval *pAction)
		{
			if (m_pInnerAction != pAction)
			{
				CC_SAFE_RELEASE(m_pInnerAction);
				m_pInnerAction = pAction;
				CC_SAFE_RETAIN(m_pInnerAction);
			}
		}

		inline CCActionInterval* getInnerAction()
		{
			return m_pInnerAction;
		}

	public:
		/** creates the action */
		static CCRepeatForever* create(CCActionInterval *pAction);

	protected:
		/** Inner action */
		CCActionInterval *m_pInnerAction;
	};


class CC_DLL CCSpawn : public CCActionInterval
{
public:
    ~CCSpawn(void);

    /** initializes the Spawn action with the 2 actions to spawn */
    bool initWithTwoActions(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2);

    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void stop(void);
    virtual void update(float time);
    virtual CCActionInterval* reverse(void);

public:

    /** helper constructor to create an array of spawned actions */
    static CCSpawn* create(CCFiniteTimeAction *pAction1, ...);
    
    /** helper constructor to create an array of spawned actions */
    static CCSpawn* createWithVariableList(CCFiniteTimeAction *pAction1, va_list args);

    /** helper constructor to create an array of spawned actions given an array */
    static CCSpawn* create(CCArray *arrayOfActions);

    /** creates the Spawn action */
    static CCSpawn* createWithTwoActions(CCFiniteTimeAction *pAction1, CCFiniteTimeAction *pAction2);

protected:
    CCFiniteTimeAction *m_pOne;
    CCFiniteTimeAction *m_pTwo;
};


	class CC_DLL CCRotateTo : public CCActionInterval
	{
	public:
		/** initializes the action */
		bool initWithDuration(float duration, float fDeltaAngle);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);
		virtual bool initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
	public:
		/** creates the action */
		static CCRotateTo* create(float duration, float fDeltaAngle);
		static CCRotateTo* create(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
	protected:
		float m_fDstAngle;
		float m_fStartAngle;
		float m_fDiffAngle;

		float m_fDstAngleX;
		float m_fDstAngleY;
	};

	/** @brief Rotates a CCNode object clockwise a number of degrees by modifying it's rotation attribute.
	*/
	class CC_DLL CCRotateBy : public CCActionInterval
	{
	public:
		/** initializes the action */
		bool initWithDuration(float duration, float fDeltaAngle);
		bool initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action */
		static CCRotateBy* create(float duration, float fDeltaAngle);
		static CCRotateBy* create(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
	protected:
		float m_fAngle;
		float m_fStartAngle;
		float m_fAngleX;
		float m_fAngleY;
	};

	/** @brief Moves a CCNode object to the position x,y. x and y are absolute coordinates by modifying it's position attribute.
	*/
	class CC_DLL CCMoveTo : public CCActionInterval
	{
	public:
		/** initializes the action */
		bool initWithDuration(float duration, const CCPoint& position);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);

	public:
		/** creates the action */
		static CCMoveTo* create(float duration, const CCPoint& position);

	protected:
		CCPoint m_endPosition;
		CCPoint m_startPosition;
		CCPoint m_delta;
	};

	/** @brief Moves a CCNode object x,y pixels by modifying it's position attribute.
	x and y are relative to the position of the object.
	Duration is is seconds.
	*/ 
	class CC_DLL CCMoveBy : public CCMoveTo
	{
	public:
		/** initializes the action */
		bool initWithDuration(float duration, const CCPoint& position);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action */
		static CCMoveBy* create(float duration, const CCPoint& position);
	};

	/** Skews a CCNode object to given angles by modifying it's skewX and skewY attributes
	@since v1.0
	*/
	class CC_DLL CCSkewTo : public CCActionInterval
	{
	public:
		CCSkewTo();
		virtual bool initWithDuration(float t, float sx, float sy);
		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);

	public:
		static CCSkewTo* create(float t, float sx, float sy);

	protected:
		float m_fSkewX;
		float m_fSkewY;
		float m_fStartSkewX;
		float m_fStartSkewY;
		float m_fEndSkewX;
		float m_fEndSkewY;
		float m_fDeltaX;
		float m_fDeltaY;
	};

	/** Skews a CCNode object by skewX and skewY degrees
	@since v1.0
	*/
	class CC_DLL CCSkewBy : public CCSkewTo
	{
	public:
		virtual bool initWithDuration(float t, float sx, float sy);
		virtual void startWithTarget(CCNode *pTarget);
		virtual CCActionInterval* reverse(void);

	public:
		static CCSkewBy* create(float t, float deltaSkewX, float deltaSkewY);
	};

	/** @brief Moves a CCNode object simulating a parabolic jump movement by modifying it's position attribute.
	*/
	class CC_DLL CCJumpBy : public CCActionInterval
	{
	public:
		/** initializes the action */
		bool initWithDuration(float duration, const CCPoint& position, float height, unsigned int jumps);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action */
		static CCJumpBy* create(float duration, const CCPoint& position, float height, unsigned int jumps);

	protected:
		CCPoint			m_startPosition;
		CCPoint			m_delta;
		float			m_height;
		unsigned int    m_nJumps;
		CCPoint         m_previousPos;
	};

	/** @brief Moves a CCNode object to a parabolic position simulating a jump movement by modifying it's position attribute.
	*/ 
	class CC_DLL CCJumpTo : public CCJumpBy
	{
	public:
		virtual void startWithTarget(CCNode *pTarget);
		virtual CCObject* copyWithZone(CCZone* pZone);

	public:
		/** creates the action */
		static CCJumpTo* create(float duration, const CCPoint& position, float height, int jumps);
	};

	/** @typedef bezier configuration structure
	*/
	typedef struct _ccBezierConfig {
		//! end position of the bezier
		CCPoint endPosition;
		//! Bezier control point 1
		CCPoint controlPoint_1;
		//! Bezier control point 2
		CCPoint controlPoint_2;
	} ccBezierConfig;

	/** @brief An action that moves the target with a cubic Bezier curve by a certain distance.
	*/
	class CC_DLL CCBezierBy : public CCActionInterval
	{
	public:
		/** initializes the action with a duration and a bezier configuration */
		bool initWithDuration(float t, const ccBezierConfig& c);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action with a duration and a bezier configuration */
		static CCBezierBy* create(float t, const ccBezierConfig& c);

	protected:
		ccBezierConfig m_sConfig;
		CCPoint m_startPosition;
	};

	/** @brief An action that moves the target with a cubic Bezier curve to a destination point.
	@since v0.8.2
	*/
	class CC_DLL CCBezierTo : public CCBezierBy
	{
	public:
		virtual void startWithTarget(CCNode *pTarget);
		virtual CCObject* copyWithZone(CCZone* pZone);

	public:
		/** creates the action with a duration and a bezier configuration */
		static CCBezierTo* create(float t, const ccBezierConfig& c);
	};

	/** @brief Scales a CCNode object to a zoom factor by modifying it's scale attribute.
	@warning This action doesn't support "reverse"
	*/
	class CC_DLL CCScaleTo : public CCActionInterval
	{
	public:
		/** initializes the action with the same scale factor for X and Y */
		bool initWithDuration(float duration, float s);

		/** initializes the action with and X factor and a Y factor */
		bool initWithDuration(float duration, float sx, float sy);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);

	public:
		/** creates the action with the same scale factor for X and Y */
		static CCScaleTo* create(float duration, float s);

		/** creates the action with and X factor and a Y factor */
		static CCScaleTo* create(float duration, float sx, float sy);
	protected:
		float m_fScaleX;
		float m_fScaleY;
		float m_fStartScaleX;
		float m_fStartScaleY;
		float m_fEndScaleX;
		float m_fEndScaleY;
		float m_fDeltaX;
		float m_fDeltaY;
	};

	/** @brief Scales a CCNode object a zoom factor by modifying it's scale attribute.
	*/
	class CC_DLL CCScaleBy : public CCScaleTo
	{
	public:
		virtual void startWithTarget(CCNode *pTarget);
		virtual CCActionInterval* reverse(void);
		virtual CCObject* copyWithZone(CCZone* pZone);

	public:
		/** creates the action with the same scale factor for X and Y */
		static CCScaleBy* create(float duration, float s);

		/** creates the action with and X factor and a Y factor */
		static CCScaleBy* create(float duration, float sx, float sy);
	};

	/** @brief Blinks a CCNode object by modifying it's visible attribute
	*/
	class CC_DLL CCBlink : public CCActionInterval
	{
	public:
		/** initializes the action */
		bool initWithDuration(float duration, unsigned int uBlinks);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action */
		static CCBlink* create(float duration, unsigned int uBlinks);
	protected:
		unsigned int m_nTimes;
	};

	/** @brief Fades In an object that implements the CCRGBAProtocol protocol. It modifies the opacity from 0 to 255.
	The "reverse" of this action is FadeOut
	*/
	class CC_DLL CCFadeIn : public CCActionInterval
	{
	public:
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);
		virtual CCObject* copyWithZone(CCZone* pZone);

	public:
		/** creates the action */
		static CCFadeIn* create(float d);
	};

	/** @brief Fades Out an object that implements the CCRGBAProtocol protocol. It modifies the opacity from 255 to 0.
	The "reverse" of this action is FadeIn
	*/
	class CC_DLL CCFadeOut : public CCActionInterval
	{
	public:
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);
		virtual CCObject* copyWithZone(CCZone* pZone);

	public:
		/** creates the action */
		static CCFadeOut* create(float d);
	};

	/** @brief Fades an object that implements the CCRGBAProtocol protocol. It modifies the opacity from the current value to a custom one.
	@warning This action doesn't support "reverse"
	*/
	class CC_DLL CCFadeTo : public CCActionInterval
	{
	public:
		/** initializes the action with duration and opacity */
		bool initWithDuration(float duration, CCubyte opacity);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);

	public:
		/** creates an action with duration and opacity */
		static CCFadeTo* create(float duration, CCubyte opacity);

	protected:
		CCubyte m_toOpacity;
		CCubyte m_fromOpacity;
	};

	/** @brief Tints a CCNode that implements the CCNodeRGB protocol from current tint to a custom one.
	@warning This action doesn't support "reverse"
	@since v0.7.2
	*/
	class CC_DLL CCTintTo : public CCActionInterval
	{
	public:
		/** initializes the action with duration and color */
		bool initWithDuration(float duration, CCubyte red, CCubyte green, CCubyte blue);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);

	public:
		/** creates an action with duration and color */
		static CCTintTo* create(float duration, CCubyte red, CCubyte green, CCubyte blue);

	protected:
		ccColor3B m_to;
		ccColor3B m_from;
	};

	/** @brief Tints a CCNode that implements the CCNodeRGB protocol from current tint to a custom one.
	@since v0.7.2
	*/
	class CC_DLL CCTintBy : public CCActionInterval
	{
	public:
		/** initializes the action with duration and color */
		bool initWithDuration(float duration, CCshort deltaRed, CCshort deltaGreen, CCshort deltaBlue);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates an action with duration and color */
		static CCTintBy* create(float duration, CCshort deltaRed, CCshort deltaGreen, CCshort deltaBlue);

	protected:
		CCshort m_deltaR;
		CCshort m_deltaG;
		CCshort m_deltaB;

		CCshort m_fromR;
		CCshort m_fromG;
		CCshort m_fromB;
	};

	/** @brief Delays the action a certain amount of seconds
	*/
	class CC_DLL CCDelayTime : public CCActionInterval
	{
	public:
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);
		virtual CCObject* copyWithZone(CCZone* pZone);

	public:
		/** creates the action */
		static CCDelayTime* create(float d);
	};

	/** @brief Executes an action in reverse order, from time=duration to time=0

	@warning Use this action carefully. This action is not
	sequenceable. Use it as the default "reversed" method
	of your own actions, but using it outside the "reversed"
	scope is not recommended.
	*/
	class CC_DLL CCReverseTime : public CCActionInterval
	{
	public:
		~CCReverseTime(void);
		CCReverseTime();

		/** initializes the action */
		bool initWithAction(CCFiniteTimeAction *pAction);

		virtual CCObject* copyWithZone(CCZone* pZone);
		virtual void startWithTarget(CCNode *pTarget);
		virtual void stop(void);
		virtual void update(float time);
		virtual CCActionInterval* reverse(void);

	public:
		/** creates the action */
		static CCReverseTime* create(CCFiniteTimeAction *pAction);

	protected:
		CCFiniteTimeAction *m_pOther;
	};


	class CCTexture2D;
	/** @brief Animates a sprite given the name of an Animation */
	class CC_DLL CCAnimate : public CCActionInterval
{
public:
    CCAnimate();
    ~CCAnimate();

    /** initializes the action with an Animation and will restore the original frame when the animation is over */
    bool initWithAnimation(CCAnimation *pAnimation);


    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void stop(void);
    virtual void update(float t);
    virtual CCActionInterval* reverse(void);

public:
    /** creates the action with an Animation and will restore the original frame when the animation is over */
    static CCAnimate* create(CCAnimation *pAnimation);
    CC_SYNTHESIZE_RETAIN(CCAnimation*, m_pAnimation, Animation)
protected:
    std::vector<float>* m_pSplitTimes;
    int                m_nNextFrame;
    CCSpriteFrame*  m_pOrigFrame;
       unsigned int    m_uExecutedLoops;
};
class CC_DLL CCTargetedAction : public CCActionInterval
{
public:
    CCTargetedAction();
    virtual ~CCTargetedAction();

    /** Create an action with the specified action and forced target */
    static CCTargetedAction* create(CCNode* pTarget, CCFiniteTimeAction* pAction);

    /** Init an action with the specified action and forced target */
    bool initWithTarget(CCNode* pTarget, CCFiniteTimeAction* pAction);

    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void stop(void);
    virtual void update(float time);

    /** This is the target that the action will be forced to run with */
    CC_SYNTHESIZE_RETAIN(CCNode*, m_pForcedTarget, ForcedTarget);
private:
    CCFiniteTimeAction* m_pAction;
};


}

#endif //__ACTION_CCINTERVAL_ACTION_H__
