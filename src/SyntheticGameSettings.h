#pragma once
#ifndef _SYNTHETIC_GAME_SETTINGS_
#define _SYNTHETIC_GAME_SETTINGS_

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

namespace YYYCZ
{
	//��Ϸ��һЩ��������
	class SyntheticGameSettings
	{
		using String = std::string;
		template<class _Ty>
		using Vector = std::vector<_Ty>;

	public:
		//�޸���������������ֵ�൱���޸�Ĭ�ϲ���
		//���ڲ�����������˵��
		SyntheticGameSettings()
		{
			//�Ѷ�ϵ����ֻ�ܽ��� 1.0f �� 10.0f ֮�䣩
			GameDifficulty = 5.0f;
			difficulty = GameDifficulty / 5.0f;

			//����ͼƬ������
			BackgroundPictureName = "Background.png";
			BallPicturesName = "";
			LinePictureName = "Line.png";
			BehindName = "png";
			SuccessPictureName = "Success.png";

			WaterName = "Water";
			FlowerAName = "FlowerA";
			FlowerBName = "FlowerB";

			//����������İ뾶
			BallRadius =
			{ 1.0f,1.2f,1.8f,2.0f,2.4f,2.8f,3.1f,4.0f,4.4f,5.6f,6.0f };

			//������
			BallTypeNum = BallRadius.size();

			//�����ɵ������������ÿ�����ɸ����ӵ�����ǰ������֣�
			GenerateNum = BallTypeNum / 2;

			//�����ɫ
			BallColor =
			{ olc::MAGENTA,olc::DARK_RED,olc::Pixel(255,165,0),olc::DARK_GREEN,
				olc::DARK_YELLOW,olc::CYAN,olc::VERY_DARK_MAGENTA ,olc::GREEN,
				olc::DARK_GREY,olc::BLUE,olc::RED };

			//������ɫ
			BackGroundColor = olc::WHITE;

			//������ɫ
			FontColor = olc::BLACK;

			//����Ŀ�Ⱥ͸߶�
			SGE_Width = 400;
			SGE_Height = 800;
			SGE_Line = 150;
			SGE_Generate = SGE_Line * 4 / 5;

			FloatSet();

			//�����С�����������С�Ļ��������
			SGE_Scale_B2 = 20.0f;

			ScaleMatch();

			//ʱ�����
			RebuilTime = 0.5f;//Ͷ�����������ɵļ��ʱ��
			DeadTime = 0.4f * difficulty;//����ʱ���ж�
			StretchTime = 0.2f;//������ʱ�䣨һ����Ч��
			DeadBombTime = 0.1f;//������ը�ļ��ʱ��

			//��˸��Ч��������Ч�Ĳ���
			FlashStep = 1530.0f;
			StretchRate = 0.2f;

			//�������ɸ���
			PProp = 0.1f;
		}

		void FloatSet()
		{
			SGE_Width_f = float(SGE_Width);
			SGE_Height_f = float(SGE_Height);
			SGE_Generate_f = float(SGE_Generate);
			SGE_Line_f = float(SGE_Line);
		}

		//ʹ�����������������ĳߴ��Ӧ
		void ScaleMatch()
		{
			SGE_BallRadius.resize(BallRadius.size());
			for (size_t i = 0; i < BallRadius.size(); i++)
				SGE_BallRadius[i] = BallRadius[i] * SGE_Scale_B2;

			B2_Width = SGE_Width_f / SGE_Scale_B2;
			B2_Height = SGE_Height_f / SGE_Scale_B2;
			B2_Generate = SGE_Generate / SGE_Scale_B2;
			B2_Line = SGE_Line / SGE_Scale_B2;

			B2_Left = 0.1f;
			B2_Right = B2_Width - B2_Left;

			B2_Gravity = B2_Height * 1.25f;
		}

	protected:
		//�Ѷ�ϵ����ֻ�ܽ��� 1.0f �� 10.0f ֮�䣩
		float GameDifficulty;
		float difficulty;

		//������
		int32_t BallTypeNum;

		//�����ɵ������������ÿ�����ɸ����ӵ�����ǰ������֣�
		int32_t GenerateNum;

		//����ͼƬ������
		std::string BackgroundPictureName;
		std::string BallPicturesName;
		std::string LinePictureName;
		std::string BehindName;
		std::string SuccessPictureName;

		std::string WaterName;
		std::string FlowerAName;
		std::string FlowerBName;

		//һЩҪ�õ��ĳ�������
		Vector<float> BallRadius;//����������İ뾶

		Vector<olc::Pixel> BallColor;//��ɫ

		olc::Pixel BackGroundColor;//������ɫ
		olc::Pixel FontColor;//������ɫ

		//����Ŀ�Ⱥ͸߶�
		int32_t SGE_Width;//������
		int32_t SGE_Height;//����߶�
		int32_t SGE_Generate;//���ɵĸ߶ȣ�ע���߶ȴ������½��м��㣩
		int32_t SGE_Line;//�����ж��ߵĸ߶�

		float SGE_Width_f;
		float SGE_Height_f;
		float SGE_Generate_f;
		float SGE_Line_f;

		//�����С�����������С�Ļ��������
		float SGE_Scale_B2;

		Vector<float> SGE_BallRadius;//����뾶

		//����ʱ�����
		float B2_Width;
		float B2_Height;
		float B2_Generate;
		float B2_Line;

		//��Բ���ɵ�����ߺ����ұ�
		float B2_Left;
		float B2_Right;

		//�������ٶ�
		float B2_Gravity;

		//ʱ�����
		float RebuilTime;
		float DeadTime;
		float StretchTime;
		float DeadBombTime;

		//��˸��Ч��������Ч�Ĳ���
		float FlashStep;
		float StretchRate;

		//�������ɸ���
		float PProp;
	};
}

#endif