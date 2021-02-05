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

	//��Ϸ����
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

		//��Ϸ��ʼ
		bool GameStart(bool vsync = false);

		//����ͼƬ���ڵ��ļ���·��
		void SetPicturePath(const String& path);

		//��ȡͼƬ����·��
		String GetPicturePath();

		//���óߴ�
		void SetScale(float scale);

		//���ô��ڳ���
		void SetSGESize(int32_t width, int32_t height);

		//���������ߵ�λ��
		void SetLineHeight(int32_t height);

		//��������򱳾���ɫ
		void SetColor(ColorArea area, const olc::Pixel& color);

		//����ͼƬ����
		void SetFileName(FileName file_type, const String& file_name);

		//������Ϸ�Ѷȣ�1.0 - 10.0��
		void SetDifficulty(float _difficulty_);

		//���ô�������
		void SetSGEName(const String& app_name);

		//���õ������ɵĸ��ʣ�0 - 1��
		void SetPropProbability(float _p_);

		//������Ƶ
		void LoadAudio(AudioName AudioType, const String& AudioPath);

		//���ص���
		template<class _Ty>
		void LoadProp(const String& picture_file_name = String());

		//Body����Ϣ
		struct BodyInfo
		{
			int32_t level = 0;
			SyntheticGameProp_Pre* prop = nullptr;

			BodyInfo() = default;
			BodyInfo(int32_t _level_):level(_level_) {}
		};

	protected:
		//���ص�ͼƬ
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

		//��ЧͼƬ
		Vector<olc::Decal*> effect_water;
		Vector<olc::Decal*> effect_flower1;
		Vector<olc::Decal*> effect_flower2;

		Vector<olc::vf2d> effect_water_center;
		Vector<olc::vf2d> effect_flower1_center;
		Vector<olc::vf2d> effect_flower2_center;

		//���ص���Ƶ
		int AudioId[4] = { -1,-1,-1,-1 };

		//��������
		b2World* m_world = nullptr;

		//�ϴ����λ�ã���������ƶ���һ�¼�
		b2Vec2 last_mouse = b2Vec2_zero;

		//��Ϸ��һЩ����
		Vector<Vector<b2Fixture *>> touch_balls;
		b2Body* next_ball = nullptr;
		int32_t score = 0;
		bool dead = false;

		//��Ϸ��������
		Vector<SyntheticGameProp_Pre *> props;
		Vector<b2Body*> prop_body;

		//�ṩ����Ϸ������Ĵݻ��б�
		Vector<b2Body*> destroy_list;

		//�ַ�������
		char string_buf[128];

		//��ʱ��
		float rebuild_time = 0.0f;
		float dead_clock = DeadTime;
		float dead_bomb_clock = DeadBombTime;

		//����
		int32_t combo = 0;

		//��˸Ч��
		struct Flash
		{
			b2Body* body = nullptr;
			float alpha = UINT8_MAX;
			float clock = 1.0f;
			bool minus = true;

			Flash() = default;
			Flash(b2Body* _body_) :body(_body_) {}
		};

		//�����ƶ�Ч��
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

		//����Ч��
		struct Stretch
		{
			b2Body* body = nullptr;
			float step = 0.0f;
			std::list<float> target_radius;
		};

		//�ϳ���Ч
		struct SEffect
		{
			//��Ч���ĵ�
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

					//����ĽǶ�����
					float angle = FloatRandom(0.0f, 6.283185f);
					float angle_distance = 6.283185f / number;

					//��ȡÿ����������ĵ�λ��
					float _cos_ = cosf(angle);
					float _sin_ = sinf(angle);

					float _cos_add_ = cosf(angle_distance);
					float _sin_add_ = sinf(angle_distance);

					float half_clock_inv = 1.0f / half_clock;

					for (int32_t i = 0; i < number; i++)
					{
						//ʵ�ʰ뾶�����Ⱦ������
						float radius_error = radius * FloatRandom(0.5f, 1.5f);
						//�����ٶȵ�ģ��
						float speed = (max_radius - radius) * half_clock_inv;

						//��ȡ��������ĵ㲢������û���Ƕ�
						centers.emplace_back(olc::vf2d(pos.x + radius_error * _cos_,
							pos.y + radius_error * _sin_), FloatRandom(0.0f, 6.283185f),
							olc::vf2d(speed * _cos_, speed * _sin_), OneOrTwo(), FloatRandom(200.0f,255.0f));

						//���� cos �� sin ֵ
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

		//��˸�����б�
		Vector<Flash> flash_list;

		//�����ƶ������б�
		Vector<QuickMove> quick_move_list;

		//���������б�
		Vector<Stretch> stretch_list;

		//�ϳ���Ч�б�
		Vector<SEffect> seffect_list;

		//ʤ���������
		//��� -> ��ת -> ��С
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

		//����ͼƬ
		void LoadPictures(const String& path);
		
		//����Ѽ��ص�ͼƬ
		void ClearPictures();

		//����Ϊ����
		void SGESetProp(b2Body* body);

		//�������
		void SGEFreeProp(b2Body* body);

		//��յ���
		void SGEClearProp();

		//��˸Ч��
		void SGEFlash(float fElapsedTime);

		//�����ƶ�Ч��
		void SGEQuickMove(float fElapsedTime);

		//����Ϊ��������
		void SGESetStretch(b2Body* body);
		
		//ɾ����������
		void SGEDeleteStretch(b2Body* body);

		//����Ч��
		void SGEStretch(float fElapsedTime);

		//���úϳ�Ч��
		void SGESetSEffect(float sge_x_, float sge_y_, int32_t level);

		//���ƺϳ�Ч��
		void SGEDrawSEffect();

		//�ϳ�Ч��
		void SGEEffect(float fElapsedTime);

		//����ʤ������Ч��
		void SGESetSuccess(int32_t level);

		//ʤ������Ч��
		void SGESuccess(float fElapsedTime);

		//���ƴ���
		void SGEDrawWindows();
		
		//�����µ���
		void CreateNewBall();//�ڳ������������
		b2Body* CreateNewBall(float x, float y, int level, 
			const b2Vec2& velocity = b2Vec2_zero,bool contain_prop = false);

		//ɾ������
		void DestroyBall(b2Body* body);
		
		//�����ж�
		bool DeadJudge();
		
		//������˸
		void DeadFlash();

		//��������
		void DeadBomb();

		//���ڶ�Ӵ��Ĵ���
		int FindFixture(b2Fixture* _find_);

		//���кϳ�
		void PreSyntheticOperation(b2Body * body1,b2Body * body2, int32_t level);
		void SyntheticOperation();

		//�����¼�����
		void PropEvent(float fElapsedTime);

		//�����Ƿ�δ��ɾ������ҪӦ�Ե���ɾ������
		bool NotBeDestroy(b2Body* body);
	private:
		//����ƶ��¼�
		void MouseMove(const b2Vec2& p);
		
		//�������
		void MouseDown(const b2Vec2& p);
		
		//���ڴ���
		bool OnUserCreate() override;
		
		//���ڸ���
		bool OnUserUpdate(float fElapsedTime) override;
		
		//��������
		bool OnUserDestroy() override;
		
		//�����ж��Ƿ���Ժϳ�
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