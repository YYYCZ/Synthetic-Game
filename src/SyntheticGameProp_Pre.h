#pragma once
#ifndef _SYNTHETIC_GAME_PROP_PRE_
#define _SYNTHETIC_GAME_PROP_PRE_

#include "SyntheticGameSettings.h"
#include <include/box2d/box2d.h>

#define OLC_PGEX_SOUND
#include <Extensions/olcPGEX_Sound.h>

namespace YYYCZ
{
	class SyntheticGameProp_Pre
	{
	public:
		//道具球半径
		float radius = 1.0f;

		//游戏开始，主要用于图片的初始化等
		virtual void GameStart() = 0;

		//道具球生成处理
		virtual void Generate(b2Body* _self_) = 0;

		//道具球扔下处理
		virtual void Down(b2Body* _self_) = 0;

		//道具球碰撞处理
		virtual void Crash(b2Body* _self_, b2Body* crash_ball) = 0;

		//道具球计时器
		virtual void Clock(b2Body* _self_, float time) = 0;

		//道具球销毁处理
		virtual void Free(b2Body* _self_) = 0;

		//道具球绘制处理
		virtual void Draw(b2Body* _self_) = 0;
	};
}

#endif