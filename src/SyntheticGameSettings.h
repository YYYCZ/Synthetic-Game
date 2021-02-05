#pragma once
#ifndef _SYNTHETIC_GAME_SETTINGS_
#define _SYNTHETIC_GAME_SETTINGS_

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

namespace YYYCZ
{
	//游戏的一些参数设置
	class SyntheticGameSettings
	{
		using String = std::string;
		template<class _Ty>
		using Vector = std::vector<_Ty>;

	public:
		//修改这个函数里面的数值相当于修改默认参数
		//关于参数在下面有说明
		SyntheticGameSettings()
		{
			//难度系数（只能介于 1.0f 和 10.0f 之间）
			GameDifficulty = 5.0f;
			difficulty = GameDifficulty / 5.0f;

			//加载图片的名称
			BackgroundPictureName = "Background.png";
			BallPicturesName = "";
			LinePictureName = "Line.png";
			BehindName = "png";
			SuccessPictureName = "Success.png";

			WaterName = "Water";
			FlowerAName = "FlowerA";
			FlowerBName = "FlowerB";

			//物理世界球的半径
			BallRadius =
			{ 1.0f,1.2f,1.8f,2.0f,2.4f,2.8f,3.1f,4.0f,4.4f,5.6f,6.0f };

			//种类数
			BallTypeNum = BallRadius.size();

			//可生成的种类个数（即每次生成给你扔的球是前面多少种）
			GenerateNum = BallTypeNum / 2;

			//球的颜色
			BallColor =
			{ olc::MAGENTA,olc::DARK_RED,olc::Pixel(255,165,0),olc::DARK_GREEN,
				olc::DARK_YELLOW,olc::CYAN,olc::VERY_DARK_MAGENTA ,olc::GREEN,
				olc::DARK_GREY,olc::BLUE,olc::RED };

			//背景颜色
			BackGroundColor = olc::WHITE;

			//字体颜色
			FontColor = olc::BLACK;

			//界面的宽度和高度
			SGE_Width = 400;
			SGE_Height = 800;
			SGE_Line = 150;
			SGE_Generate = SGE_Line * 4 / 5;

			FloatSet();

			//界面大小与物理世界大小的换算比例尺
			SGE_Scale_B2 = 20.0f;

			ScaleMatch();

			//时间参数
			RebuilTime = 0.5f;//投掷到重新生成的间隔时间
			DeadTime = 0.4f * difficulty;//死亡时间判定
			StretchTime = 0.2f;//伸缩的时间（一个特效）
			DeadBombTime = 0.1f;//死亡后爆炸的间隔时间

			//闪烁特效和伸缩特效的参数
			FlashStep = 1530.0f;
			StretchRate = 0.2f;

			//道具生成概率
			PProp = 0.1f;
		}

		void FloatSet()
		{
			SGE_Width_f = float(SGE_Width);
			SGE_Height_f = float(SGE_Height);
			SGE_Generate_f = float(SGE_Generate);
			SGE_Line_f = float(SGE_Line);
		}

		//使物理世界和像素世界的尺寸对应
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
		//难度系数（只能介于 1.0f 和 10.0f 之间）
		float GameDifficulty;
		float difficulty;

		//种类数
		int32_t BallTypeNum;

		//可生成的种类个数（即每次生成给你扔的球是前面多少种）
		int32_t GenerateNum;

		//加载图片的名称
		std::string BackgroundPictureName;
		std::string BallPicturesName;
		std::string LinePictureName;
		std::string BehindName;
		std::string SuccessPictureName;

		std::string WaterName;
		std::string FlowerAName;
		std::string FlowerBName;

		//一些要用到的常量参数
		Vector<float> BallRadius;//物理世界球的半径

		Vector<olc::Pixel> BallColor;//颜色

		olc::Pixel BackGroundColor;//背景颜色
		olc::Pixel FontColor;//字体颜色

		//界面的宽度和高度
		int32_t SGE_Width;//界面宽度
		int32_t SGE_Height;//界面高度
		int32_t SGE_Generate;//生成的高度（注：高度从上往下进行计算）
		int32_t SGE_Line;//死亡判定线的高度

		float SGE_Width_f;
		float SGE_Height_f;
		float SGE_Generate_f;
		float SGE_Line_f;

		//界面大小与物理世界大小的换算比例尺
		float SGE_Scale_B2;

		Vector<float> SGE_BallRadius;//界面半径

		//物理时间参数
		float B2_Width;
		float B2_Height;
		float B2_Generate;
		float B2_Line;

		//新圆生成的最左边和最右边
		float B2_Left;
		float B2_Right;

		//重力加速度
		float B2_Gravity;

		//时间参数
		float RebuilTime;
		float DeadTime;
		float StretchTime;
		float DeadBombTime;

		//闪烁特效和伸缩特效的参数
		float FlashStep;
		float StretchRate;

		//道具生成概率
		float PProp;
	};
}

#endif