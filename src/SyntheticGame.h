
/*
* * * * * * * * * * *
*  欢迎玩高血压游戏 *
*      By YYYCZ     *
* * * * * * * * * * *
*/

// 合成大西瓜

#pragma once
#ifndef _SYNTHETIC_GAME_
#define _SYNTHETIC_GAME_

#include "SyntheticGameEngine.h"
#include "SyntheticGameProp.h"
#include "SyntheticGameProp_Extensions.h"

namespace YYYCZ
{
	//调用此函数快速开始游戏
	inline void GameStart()
	{
		SyntheticGameEngine* MyGame = new SyntheticGameEngine;

		//设置游戏难度
		//MyGame->SetDifficulty(5.0f);

		//设置游戏窗口名称
		//MyGame->SetSGEName("Synthetic Game - By YYYCZ");

		//设置窗口大小和对应到“物理世界”的尺寸
		//MyGame->SetSGESize(400, 800);
		//MyGame->SetScale(20.0f);
		//MyGame->SetLineHeight(150);

		//设置颜色
		//MyGame->SetColor(ColorArea::BackgroundArea,olc::WHITE);
		//MyGame->SetColor(ColorArea::FontArea, olc::BLACK);

		//设置图片路径
		MyGame->SetPicturePath(".\\Picture");
		//MyGame->SetFileName(FileName::BackgroundName, "Background.png");
		//MyGame->SetFileName(FileName::BallName, "");
		//MyGame->SetFileName(FileName::LineName, "Line.png");
		//MyGame->SetFileName(FileName::BallBehindName, "png");
		//MyGame->SetFileName(FileName::SuccessName, "Success.png");
		//MyGame->SetFileName(FileName::EffectWaterName, "Water");
		//MyGame->SetFileName(FileName::EffectFlowerAName, "FlowerA");
		//MyGame->SetFileName(FileName::EffectFlowerBName, "FlowerB");

		//加载音频
		MyGame->LoadAudio(AudioName::SynthesisAudio, ".\\Sound\\synthesis.wav");
		MyGame->LoadAudio(AudioName::DeadAudio, ".\\Sound\\dead.wav");
		MyGame->LoadAudio(AudioName::BackgroundAudio, ".\\Sound\\background.wav");
		MyGame->LoadAudio(AudioName::SuccessAudio, ".\\Sound\\success.wav");

		//加载道具
		//注：道具加载的图片为 
		//图片路径（在上面已设置） + 道具名称 + 后缀名（在上面已设置）
		MyGame->LoadProp<PropChange>();
		MyGame->LoadProp<PropDestroy>();
		MyGame->LoadProp<PropRandom>();
		MyGame->LoadProp<PropGravity>();
		MyGame->LoadProp<PropFuse>();

		//设置道具生成的概率
		//MyGame->SetPropProbability(0.1f);

		MyGame->GameStart();

		delete MyGame;
	}
}

#endif
