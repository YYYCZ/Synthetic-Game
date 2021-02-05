
/*
* * * * * * * * * * *
*  ��ӭ���Ѫѹ��Ϸ *
*      By YYYCZ     *
* * * * * * * * * * *
*/

// �ϳɴ�����
// ����ѧϰ��������ֹ�κ���ʽ����ҵ��;

#pragma once
#ifndef _SYNTHETIC_GAME_
#define _SYNTHETIC_GAME_

#include "SyntheticGameEngine.h"
#include "SyntheticGameProp.h"
#include "SyntheticGameProp_Extensions.h"

namespace YYYCZ
{
	//���ô˺������ٿ�ʼ��Ϸ
	inline void GameStart()
	{
		SyntheticGameEngine* MyGame = new SyntheticGameEngine;

		//������Ϸ�Ѷ�
		//MyGame->SetDifficulty(5.0f);

		//������Ϸ��������
		//MyGame->SetSGEName("Synthetic Game - By YYYCZ");

		//���ô��ڴ�С�Ͷ�Ӧ�����������硱�ĳߴ�
		//MyGame->SetSGESize(400, 800);
		//MyGame->SetScale(20.0f);
		//MyGame->SetLineHeight(150);

		//������ɫ
		//MyGame->SetColor(ColorArea::BackgroundArea,olc::WHITE);
		//MyGame->SetColor(ColorArea::FontArea, olc::BLACK);

		//����ͼƬ·��
		MyGame->SetPicturePath(".\\Picture");
		//MyGame->SetFileName(FileName::BackgroundName, "Background.png");
		//MyGame->SetFileName(FileName::BallName, "");
		//MyGame->SetFileName(FileName::LineName, "Line.png");
		//MyGame->SetFileName(FileName::BallBehindName, "png");
		//MyGame->SetFileName(FileName::SuccessName, "Success.png");
		//MyGame->SetFileName(FileName::EffectWaterName, "Water");
		//MyGame->SetFileName(FileName::EffectFlowerAName, "FlowerA");
		//MyGame->SetFileName(FileName::EffectFlowerBName, "FlowerB");

		//������Ƶ
		MyGame->LoadAudio(AudioName::SynthesisAudio, ".\\Sound\\synthesis.wav");
		MyGame->LoadAudio(AudioName::DeadAudio, ".\\Sound\\dead.wav");
		MyGame->LoadAudio(AudioName::BackgroundAudio, ".\\Sound\\background.wav");
		MyGame->LoadAudio(AudioName::SuccessAudio, ".\\Sound\\success.wav");

		//���ص���
		//ע�����߼��ص�ͼƬΪ 
		//ͼƬ·���������������ã� + �������� + ��׺���������������ã�
		MyGame->LoadProp<PropChange>();
		MyGame->LoadProp<PropDestroy>();
		MyGame->LoadProp<PropRandom>();
		MyGame->LoadProp<PropGravity>();
		MyGame->LoadProp<PropFuse>();

		//���õ������ɵĸ���
		//MyGame->SetPropProbability(0.1f);

		MyGame->GameStart();

		delete MyGame;
	}
}

#endif