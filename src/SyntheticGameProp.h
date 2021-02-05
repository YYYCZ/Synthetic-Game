#pragma once
#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_
#ifndef _SYNTHETIC_GAME_PROP_
#define _SYNTHETIC_GAME_PROP_

namespace YYYCZ
{
	//游戏道具球（所有其他道具球继承这个类）
	class SyntheticGameProp :
		public SyntheticGameProp_Pre
	{
		friend class SyntheticGameEngine;
	public:
		using String = std::string;
		template<class _Ty>
		using Vector = std::vector<_Ty>;

		using BodyInfo = SyntheticGameEngine::BodyInfo;

		SyntheticGameEngine* engine = nullptr;

		String name;

		olc::Pixel color = olc::BLACK;

		String image_path;
		olc::Decal* image = nullptr;
		olc::vf2d image_center;
		olc::vf2d image_2inv;

		SyntheticGameProp() = default;

		//设置道具图片路径
		inline void SetPropImagePath(const String& _image_path_)
		{
			image_path = _image_path_;
		}

		//加载音频，成功返回音频 Id ，失败返回 -1
		inline int32_t LoadAudio(const String& audio_path)
		{
			return olc::SOUND::LoadAudioSample(audio_path);
		}

		//游戏开始，主要用于图片的初始化等
		virtual void GameStart()
		{
			if (!image_path.empty())
			{
				if (image != nullptr)
					return;

				olc::Sprite* new_sprite = new olc::Sprite;
				if (new_sprite->LoadFromFile(image_path) == olc::rcode::OK)
				{
					image = new olc::Decal(new_sprite);

					image_center = { new_sprite->width * 0.5f, new_sprite->height * 0.5f };
					image_2inv = { 2.0f / new_sprite->width,2.0f / new_sprite->height };
				}
				else
				{
					delete new_sprite;
				}
			}
		}

		//道具球生成处理
		virtual void Generate(b2Body* _self_) {}

		//道具球扔下处理
		virtual void Down(b2Body* _self_) {}

		//道具球碰撞处理
		virtual void Crash(b2Body* _self_, b2Body* crash_ball) {}

		//道具球计时器
		virtual void Clock(b2Body* _self_, float time) {}

		//道具球销毁处理
		virtual void Free(b2Body* _self_) {}

		//道具球绘制处理
		virtual void Draw(b2Body* _self_)
		{
			if (image == nullptr)
			{
				b2Vec2 center{ _self_->GetPosition().x * engine->SGE_Scale_B2,
				_self_->GetPosition().y * engine->SGE_Scale_B2 };
				float Radius = _self_->GetFixtureList()->GetShape()->m_radius * engine->SGE_Scale_B2;

				olc::Pixel ball_color = color,
					line_color = engine->BackGroundColor;

				for (auto& x : engine->flash_list)
					if (x.body == _self_ && x.clock > 0.0f)
						ball_color.a = uint8_t(x.alpha),
						line_color.a = uint8_t(x.alpha);

				engine->FillCircle(center.x, center.y, Radius, ball_color);

				engine->DrawLine(center.x, center.y,
					center.x + Radius * cosf(_self_->GetAngle()),
					center.y + Radius * sinf(_self_->GetAngle()),
					line_color);

				auto pos = engine->GetTextSize(name);
				engine->DrawString(center.x - pos.x * 0.5f, center.y - pos.y * 0.5f, name, engine->FontColor);
			}
			else
			{
				olc::Pixel ball_color = olc::WHITE;
				float Radius = _self_->GetFixtureList()->GetShape()->m_radius * engine->SGE_Scale_B2;

				for (auto& x : engine->flash_list)
					if (x.body == _self_ && x.clock > 0.0f)
						ball_color.a = uint8_t(x.alpha);

				engine->DrawRotatedDecal({ _self_->GetPosition().x * engine->SGE_Scale_B2,
					_self_->GetPosition().y * engine->SGE_Scale_B2 },
					image,
					_self_->GetAngle(),
					image_center,
					{ Radius * image_2inv.x, Radius * image_2inv.y },
					ball_color);
			}
		}

	protected:
		//以下为编写道具时可以用到的一些函数

		//判读一个 body 是否是道具球
		inline bool IsProp(b2Body* _judge_body_)
		{
			BodyInfo* info = (BodyInfo*)(_judge_body_->GetUserData().pointer);
			return (info->prop != nullptr && info->level != -2);
		}

		//判断一个 body 是否是普通球
		inline bool IsNormal(b2Body* _judge_body_)
		{
			BodyInfo* info = (BodyInfo*)(_judge_body_->GetUserData().pointer);
			return (info->prop == nullptr && info->level != -2);
		}

		//判断一个 body 是否为球体（还有可能是墙体）
		inline bool IsBall(b2Body* _judge_body_)
		{
			BodyInfo* info = (BodyInfo*)(_judge_body_->GetUserData().pointer);
			return (info->level != -2);
		}

		//判断一个 body 是否被扔下
		inline bool IsDown(b2Body* _judge_body_)
		{
			return (IsBall(_judge_body_) && _judge_body_ != engine->next_ball);
		}

		//设置一个球体的大小（也可以用于把道具改为非道具）
		void SetLevel(b2Body* _body_, int32_t _level_)
		{
			if (!IsBall(_body_))
				return;

			BodyInfo* info = (BodyInfo*)(_body_->GetUserData().pointer);
			info->level = _level_;

			_body_->GetFixtureList()->GetShape()->m_radius = engine->BallRadius[info->level];
			_body_->SetAwake(true);
			engine->SGESetStretch(_body_);
			engine->SGEFreeProp(_body_);
		}

		//提供位置，生成一个球体
		inline b2Body * CreateBall(float pos_x,float pos_y)
		{
			return engine->CreateNewBall(pos_x, pos_y, 0, b2Vec2_zero, false);
		}

		//破坏一个球体
		inline void DestroyBall(b2Body* _body_)
		{
			if (IsBall(_body_))
			{
				for (auto x : engine->destroy_list)
				{
					if (x == _body_)
					{
						return;
					}
				}
				engine->destroy_list.push_back(_body_);
			}
		}

		//获取初始生成球的最大等级
		inline int32_t GetGenerateLevel()
		{
			return engine->GenerateNum - 1;
		}

		//获取球的最大等级
		inline int32_t GetMaxLevel()
		{
			return engine->BallTypeNum - 1;
		}

		//获取游戏分数
		inline int32_t GetScore()
		{
			return engine->score;
		}

		//加减分数（输入负参数即为减）
		inline void AddScore(int32_t _change_score_)
		{
			engine->score += _change_score_;
		}

		//播放音频 - 游戏引擎加载的音频
		inline void PlayAudio(AudioName Type, bool loop = false)
		{
			if (engine->AudioId[int(Type)] != -1)
			{
				olc::SOUND::PlaySample(engine->AudioId[int(Type)], loop);
			}
		}

		//播放音频 - 自己加载的音频
		inline void PlayAudio(int32_t Id, bool loop = false)
		{
			if (Id != -1)
			{
				olc::SOUND::PlaySample(Id, loop);
			}
		}

		//停止播放音频 - 游戏引擎加载的音频
		inline void StopAudio(AudioName Type)
		{
			if (engine->AudioId[int(Type)] != -1)
			{
				olc::SOUND::StopSample(engine->AudioId[int(Type)]);
			}
		}

		//停止播放音频 - 自己加载的音频
		inline void StopAudio(int32_t Id)
		{
			if (Id != -1)
			{
				olc::SOUND::StopSample(Id);
			}
		}

		//获取图片路径
		inline String GetImagePath()
		{
			return engine->picture_path;
		}

		//获取重力大小
		inline float GetGravity()
		{
			return engine->m_world->GetGravity().y;
		}

		//获取场上所有的球
		Vector<b2Body*> GetAllBalls()
		{
			Vector<b2Body*> result;

			b2Body* body = engine->m_world->GetBodyList();
			int32_t body_num = engine->m_world->GetBodyCount();
			for (int32_t i = 0; i < body_num; i++, body = body->GetNext())
			{
				if (IsDown(body))
				{
					result.push_back(body);
				}
			}

			return result;
		}

		//获取场上所有普通球
		Vector<b2Body*> GetAllNormalBalls()
		{
			Vector<b2Body*> result;

			b2Body* body = engine->m_world->GetBodyList();
			int32_t body_num = engine->m_world->GetBodyCount();
			for (int32_t i = 0; i < body_num; i++, body = body->GetNext())
			{
				if (IsDown(body) && !IsProp(body))
				{
					result.push_back(body);
				}
			}

			return result;
		}

		//获取场上所有道具球
		inline Vector<b2Body*> GetAllProps()
		{
			return engine->prop_body;
		}

		//设置为道具球
		inline void SetPropBall(b2Body* body)
		{
			BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);
			info->prop = this;
			info->level = -1;
			engine->prop_body.push_back(body);
			info->prop->Generate(body);
		}

		//设置为随机道具球
		inline void SetRandomPropBall(b2Body* body)
		{
			engine->SGESetProp(body);
		}

		//设置闪烁效果
		inline void SetFlash(b2Body * body,float flash_time = 0.0f)
		{
			engine->flash_list.emplace_back(body);
			if (flash_time != 0.0f)
			{
				engine->flash_list.back().clock = flash_time;
			}
		}

		//设置快速移动效果
		inline void SetQuickMove(b2Body* _self_, const b2Vec2& _target_pos_)
		{
			if (_self_->GetPosition() == _target_pos_)
				return;

			auto QMf = [=](const b2Vec2& _distance_)->float
			{
				return (abs(_distance_.x) + abs(_distance_.y));
			};
			
			engine->quick_move_list.emplace_back(radius * engine->SGE_Scale_B2,
				image,
				color,
				_self_->GetAngle(),
				_self_->GetPosition(),
				_target_pos_);
			engine->quick_move_list.back().speed *= 
				QMf(engine->quick_move_list.back().end_pos - engine->quick_move_list.back().now_pos);
		}

		//设置合成特效
		inline void SetSyntheticEffect(const b2Vec2& effect_pos, int32_t effect_level)
		{
			engine->SGESetSEffect(effect_pos.x * engine->SGE_Scale_B2, effect_pos.y * engine->SGE_Scale_B2, effect_level);
		}

		//获取合成列表
		inline Vector<Vector<b2Fixture*>>& GetTouchBallList()
		{
			return engine->touch_balls;
		}

		//合成两个球
		inline void Synthesis(b2Body* bodyA, b2Body* bodyB, int32_t level)
		{
			engine->PreSyntheticOperation(bodyA, bodyB, level);
		}

		//是否已经死亡（即是否游戏已经结束）
		inline bool IsDead()
		{
			return engine->dead;
		}

		//设置胜利闪光特效
		inline void SetSuccessEffect(int32_t effect_level)
		{
			engine->SGESetSuccess(effect_level);
		}
	};
}

#endif
#endif