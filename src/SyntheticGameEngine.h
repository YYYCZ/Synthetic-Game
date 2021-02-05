#pragma once
#ifndef _YYYCZ_SYNTHETIC_GAME_ENGINE_
#define _YYYCZ_SYNTHETIC_GAME_ENGINE_

#include "SyntheticGameProp_Pre.h"

#define RAND_LIMIT 32767
#define SGE_RandomError FloatRandom(-0.01f, 0.01f)

namespace YYYCZ
{
	inline float FloatRandom(float lo, float hi)
	{
		float r = (float)(rand() & (RAND_LIMIT));
		r /= RAND_LIMIT;
		r = (hi - lo) * r + lo;
		return r;
	}

	inline int PropRand(int32_t prop_num,float _p_)
	{
		if (FloatRandom(0.0f, 1.0f) > _p_)
			return -1;
		return abs(rand() % prop_num);
	}

	enum class ColorArea { BackgroundArea, FontArea };

	enum class FileName { BackgroundName, LineName, BallName, BallBehindName, 
		EffectWaterName, EffectFlowerAName, EffectFlowerBName, SuccessName};

	enum class AudioName { BackgroundAudio, SynthesisAudio, DeadAudio, SuccessAudio };

	//游戏引擎
	class SyntheticGameEngine :
		protected olc::PixelGameEngine, 
		protected b2ContactListener, 
		protected SyntheticGameSettings
	{
		template<class _Ty>
		using Vector = std::vector<_Ty>;
		using String = std::string;

		friend class SyntheticGameProp;

		inline int BallRand()
		{
			srand(time(nullptr));
			return abs(rand() % GenerateNum);
		}

	public:
		SyntheticGameEngine();

		~SyntheticGameEngine();

		//游戏开始
		bool GameStart(bool vsync = false);

		//设置图片所在的文件夹路径
		void SetPicturePath(const String& path);

		//获取图片所在路径
		String GetPicturePath();

		//设置尺寸
		void SetScale(float scale);

		//设置窗口长宽
		void SetSGESize(int32_t width, int32_t height);

		//设置死亡线的位置
		void SetLineHeight(int32_t height);

		//设置字体或背景颜色
		void SetColor(ColorArea area, const olc::Pixel& color);

		//设置图片名称
		void SetFileName(FileName file_type, const String& file_name);

		//设置游戏难度（1.0 - 10.0）
		void SetDifficulty(float _difficulty_);

		//设置窗口名称
		void SetSGEName(const String& app_name);

		//设置道具生成的概率（0 - 1）
		void SetPropProbability(float _p_);

		//加载音频
		void LoadAudio(AudioName AudioType, const String& AudioPath);

		//加载道具
		template<class _Ty>
		void LoadProp(const String& picture_file_name = String());

		//Body的信息
		struct BodyInfo
		{
			int32_t level = 0;
			SyntheticGameProp_Pre* prop = nullptr;

			BodyInfo() = default;
			BodyInfo(int32_t _level_):level(_level_) {}
		};

	protected:
		//加载的图片
		String picture_path;

		Vector<olc::Decal*> pictures;
		olc::Decal* background = nullptr;
		olc::Decal* line = nullptr;
		olc::Decal* success = nullptr;

		Vector<olc::vf2d> pictures_center;
		Vector<olc::vf2d> pictures_2inv;

		olc::vf2d backgroud_scale;
		olc::vf2d line_scale;
		olc::vf2d success_scale;

		//特效图片
		Vector<olc::Decal*> effect_water;
		Vector<olc::Decal*> effect_flower1;
		Vector<olc::Decal*> effect_flower2;

		Vector<olc::vf2d> effect_water_center;
		Vector<olc::vf2d> effect_flower1_center;
		Vector<olc::vf2d> effect_flower2_center;

		//加载的音频
		int AudioId[4] = { -1,-1,-1,-1 };

		//物理世界
		b2World* m_world = nullptr;

		//上次鼠标位置，用于鼠标移动这一事件
		b2Vec2 last_mouse = b2Vec2_zero;

		//游戏内一些数据
		Vector<Vector<b2Fixture *>> touch_balls;
		b2Body* next_ball = nullptr;
		int32_t score = 0;
		bool dead = false;

		//游戏道具球类
		Vector<SyntheticGameProp_Pre *> props;
		Vector<b2Body*> prop_body;

		//提供给游戏道具球的摧毁列表
		Vector<b2Body*> destroy_list;

		//字符串缓存
		char string_buf[128];

		//计时器
		float rebuild_time = 0.0f;
		float dead_clock = DeadTime;
		float dead_bomb_clock = DeadBombTime;

		//连击
		int32_t combo = 0;

		//闪烁效果
		struct Flash
		{
			b2Body* body = nullptr;
			float alpha = UINT8_MAX;
			float clock = 1.0f;
			bool minus = true;

			Flash() = default;
			Flash(b2Body* _body_) :body(_body_) {}
		};

		//快速移动效果
		struct QuickMove
		{
			float sge_radius = 0.0f;

			olc::Pixel color = olc::WHITE;

			olc::Decal* picture = nullptr;
			olc::vf2d picture_center;
			olc::vf2d picture_scale;

			float angle = 0.0f;
			b2Vec2 now_pos;
			b2Vec2 end_pos;

			float speed = 25.0f;

			QuickMove() = default;
			QuickMove(float _sge_radius_, olc::Decal * _picture_,olc::Pixel _color_,
				float _angle_,const b2Vec2& _now_pos_, const b2Vec2& _end_pos_) :
				sge_radius(_sge_radius_), picture(_picture_), color(_color_),
				angle(_angle_),now_pos(_now_pos_), end_pos(_end_pos_) 
			{
				sge_radius *= 0.8f;
				if (picture != nullptr)
				{
					picture_center = { picture->sprite->width * 0.5f,
						picture->sprite->height * 0.5f };
					picture_scale = { sge_radius / picture_center.x ,
						sge_radius / picture_center.y };
				}
			}
		};

		//伸缩效果
		struct Stretch
		{
			b2Body* body = nullptr;
			float step = 0.0f;
			std::list<float> target_radius;
		};

		//合成特效
		struct SEffect
		{
			//特效中心点
			olc::vf2d pos;

			// 2.2
			olc::Decal* water = nullptr;
			olc::vf2d water_center;

			float clock = 0.5f;
			float alpha = 255.0f;
			float step = -510.0f;
			float wscale;

			// 0.5 -> 1.5
			olc::Decal* flower1 = nullptr;
			olc::Decal* flower2 = nullptr;
			olc::vf2d flower1_center;
			olc::vf2d flower2_center;

			struct Center
			{
				olc::vf2d center;
				float center_angle;

				olc::vf2d speed;
				bool is_one;
				
				float falpha;

				Center() = default;
				Center(const olc::vf2d& _center_, float _center_angle_, const olc::vf2d& _speed_, bool _is_one_, float _falpha_) :
					center(_center_), center_angle(_center_angle_), speed(_speed_), is_one(_is_one_), falpha(_falpha_){}
			};

			Vector<Center> centers;
			float wspeed = 25.0f;

			int32_t number = 10;
			
			float half_clock = 0.25f;
			float fscale = 1.0f;
			float fscale_step;

			float falpha_step = -100.0f;

			SEffect() = default;
			SEffect(olc::vf2d _pos_,
				olc::Decal* _water_, olc::vf2d _water_center_,
				olc::Decal* _flower1_, olc::vf2d _flower1_center,
				olc::Decal* _flower2_, olc::vf2d _flower2_center,
				float _clock_, int32_t _number_, float _ball_radius_)
				:pos(_pos_),
				water(_water_), water_center(_water_center_),
				flower1(_flower1_), flower1_center(_flower1_center),
				flower2(_flower2_), flower2_center(_flower2_center),
				clock(_clock_), number(_number_)
			{
				half_clock = clock * 0.5f;

				const float start_rate = 0.5f,
					end_rate = 1.5f;

				const float water_rate = 2.2f;

				if (water != nullptr)
				{
					step = - alpha / clock;
					float real_2radius = water_rate * _ball_radius_;
					real_2radius += real_2radius;
					wscale = real_2radius * real_2radius
						/ (water->sprite->width * water->sprite->height);
				}

				int32_t flower_size = int32_t(flower1 != nullptr) + int32_t(flower2 != nullptr);
				if (flower_size > 0)
				{
					srand(time(nullptr));
					auto OneOrTwo = [=]()->bool
					{
						if (flower_size == 1) 
							return (flower1 != nullptr);

						return (rand() % 2 == 0);
					};

					float radius = start_rate * _ball_radius_;
					float max_radius = end_rate * _ball_radius_;
					
					fscale_step = - fscale / half_clock;
					falpha_step = - 50.0f / clock;

					//花瓣的角度数据
					float angle = FloatRandom(0.0f, 6.283185f);
					float angle_distance = 6.283185f / number;

					//获取每个花瓣的中心点位置
					float _cos_ = cosf(angle);
					float _sin_ = sinf(angle);

					float _cos_add_ = cosf(angle_distance);
					float _sin_add_ = sinf(angle_distance);

					float half_clock_inv = 1.0f / half_clock;

					for (int32_t i = 0; i < number; i++)
					{
						//实际半径，不等距产生美
						float radius_error = radius * FloatRandom(0.5f, 1.5f);
						//花瓣速度的模长
						float speed = (max_radius - radius) * half_clock_inv;

						//获取花瓣的中心点并随机设置花瓣角度
						centers.emplace_back(olc::vf2d(pos.x + radius_error * _cos_,
							pos.y + radius_error * _sin_), FloatRandom(0.0f, 6.283185f),
							olc::vf2d(speed * _cos_, speed * _sin_), OneOrTwo(), FloatRandom(200.0f,255.0f));

						//更新 cos 和 sin 值
						if (i != number - 1)
						{
							float _new_cos_ = _cos_ * _cos_add_ - _sin_ * _sin_add_;
							float _new_sin_ = _sin_ * _cos_add_ + _cos_ * _sin_add_;

							_cos_ = _new_cos_;
							_sin_ = _new_sin_;
						}
					}
				}
			}
		};

		//闪烁对象列表
		Vector<Flash> flash_list;

		//快速移动对象列表
		Vector<QuickMove> quick_move_list;

		//伸缩对象列表
		Vector<Stretch> stretch_list;

		//合成特效列表
		Vector<SEffect> seffect_list;

		//胜利闪光对象
		//变大 -> 旋转 -> 变小
		struct 
		{
			float clock = 0.0f;
			int32_t level = 0;
			float angle = 0.0f;
			float step = 1.0f;

			float scale = 0.0f;
			float scale_step = 5.0f;
			float scale_clock = 0.0f;
		} success_effect;

		//加载图片
		void LoadPictures(const String& path);
		
		//清除已加载的图片
		void ClearPictures();

		//设置为道具
		void SGESetProp(b2Body* body);

		//解除道具
		void SGEFreeProp(b2Body* body);

		//清空道具
		void SGEClearProp();

		//闪烁效果
		void SGEFlash(float fElapsedTime);

		//快速移动效果
		void SGEQuickMove(float fElapsedTime);

		//设置为伸缩对象
		void SGESetStretch(b2Body* body);
		
		//删除伸缩对象
		void SGEDeleteStretch(b2Body* body);

		//伸缩效果
		void SGEStretch(float fElapsedTime);

		//设置合成效果
		void SGESetSEffect(float sge_x_, float sge_y_, int32_t level);

		//绘制合成效果
		void SGEDrawSEffect();

		//合成效果
		void SGEEffect(float fElapsedTime);

		//设置胜利闪光效果
		void SGESetSuccess(int32_t level);

		//胜利闪光效果
		void SGESuccess(float fElapsedTime);

		//绘制窗口
		void SGEDrawWindows();
		
		//创造新的球
		void CreateNewBall();//在出生点进行生成
		b2Body* CreateNewBall(float x, float y, int level, 
			const b2Vec2& velocity = b2Vec2_zero,bool contain_prop = false);

		//删除球体
		void DestroyBall(b2Body* body);
		
		//死亡判定
		bool DeadJudge();
		
		//死亡闪烁
		void DeadFlash();

		//死亡破裂
		void DeadBomb();

		//用于多接触的处理
		int FindFixture(b2Fixture* _find_);

		//进行合成
		void PreSyntheticOperation(b2Body * body1,b2Body * body2, int32_t level);
		void SyntheticOperation();

		//道具事件处理
		void PropEvent(float fElapsedTime);

		//球体是否未被删除，主要应对道具删除球体
		bool NotBeDestroy(b2Body* body);
	private:
		//鼠标移动事件
		void MouseMove(const b2Vec2& p);
		
		//鼠标落下
		void MouseDown(const b2Vec2& p);
		
		//窗口创建
		bool OnUserCreate() override;
		
		//窗口更新
		bool OnUserUpdate(float fElapsedTime) override;
		
		//窗口销毁
		bool OnUserDestroy() override;
		
		//用于判断是否可以合成
		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	};

	
}

#include "SyntheticGameEngine_Public.h"
#include "SyntheticGameEngine_Picture.h"
#include "SyntheticGameEngine_Effect.h"
#include "SyntheticGameEngine_Draw.h"
#include "SyntheticGameEngine_Ball.h"
#include "SyntheticGameEngine_Process.h"
#include "SyntheticGameEngine_Core.h"

#endif