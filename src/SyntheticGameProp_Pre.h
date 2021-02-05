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
		//������뾶
		float radius = 1.0f;

		//��Ϸ��ʼ����Ҫ����ͼƬ�ĳ�ʼ����
		virtual void GameStart() = 0;

		//���������ɴ���
		virtual void Generate(b2Body* _self_) = 0;

		//���������´���
		virtual void Down(b2Body* _self_) = 0;

		//��������ײ����
		virtual void Crash(b2Body* _self_, b2Body* crash_ball) = 0;

		//�������ʱ��
		virtual void Clock(b2Body* _self_, float time) = 0;

		//���������ٴ���
		virtual void Free(b2Body* _self_) = 0;

		//��������ƴ���
		virtual void Draw(b2Body* _self_) = 0;
	};
}

#endif