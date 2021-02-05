#pragma once
#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_
#ifndef _SYNTHETIC_GAME_PROP_
#define _SYNTHETIC_GAME_PROP_

namespace YYYCZ
{
	//��Ϸ��������������������̳�����ࣩ
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

		//���õ���ͼƬ·��
		inline void SetPropImagePath(const String& _image_path_)
		{
			image_path = _image_path_;
		}

		//������Ƶ���ɹ�������Ƶ Id ��ʧ�ܷ��� -1
		inline int32_t LoadAudio(const String& audio_path)
		{
			return olc::SOUND::LoadAudioSample(audio_path);
		}

		//��Ϸ��ʼ����Ҫ����ͼƬ�ĳ�ʼ����
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

		//���������ɴ���
		virtual void Generate(b2Body* _self_) {}

		//���������´���
		virtual void Down(b2Body* _self_) {}

		//��������ײ����
		virtual void Crash(b2Body* _self_, b2Body* crash_ball) {}

		//�������ʱ��
		virtual void Clock(b2Body* _self_, float time) {}

		//���������ٴ���
		virtual void Free(b2Body* _self_) {}

		//��������ƴ���
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
		//����Ϊ��д����ʱ�����õ���һЩ����

		//�ж�һ�� body �Ƿ��ǵ�����
		inline bool IsProp(b2Body* _judge_body_)
		{
			BodyInfo* info = (BodyInfo*)(_judge_body_->GetUserData().pointer);
			return (info->prop != nullptr && info->level != -2);
		}

		//�ж�һ�� body �Ƿ�����ͨ��
		inline bool IsNormal(b2Body* _judge_body_)
		{
			BodyInfo* info = (BodyInfo*)(_judge_body_->GetUserData().pointer);
			return (info->prop == nullptr && info->level != -2);
		}

		//�ж�һ�� body �Ƿ�Ϊ���壨���п�����ǽ�壩
		inline bool IsBall(b2Body* _judge_body_)
		{
			BodyInfo* info = (BodyInfo*)(_judge_body_->GetUserData().pointer);
			return (info->level != -2);
		}

		//�ж�һ�� body �Ƿ�����
		inline bool IsDown(b2Body* _judge_body_)
		{
			return (IsBall(_judge_body_) && _judge_body_ != engine->next_ball);
		}

		//����һ������Ĵ�С��Ҳ�������ڰѵ��߸�Ϊ�ǵ��ߣ�
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

		//�ṩλ�ã�����һ������
		inline b2Body * CreateBall(float pos_x,float pos_y)
		{
			return engine->CreateNewBall(pos_x, pos_y, 0, b2Vec2_zero, false);
		}

		//�ƻ�һ������
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

		//��ȡ��ʼ����������ȼ�
		inline int32_t GetGenerateLevel()
		{
			return engine->GenerateNum - 1;
		}

		//��ȡ������ȼ�
		inline int32_t GetMaxLevel()
		{
			return engine->BallTypeNum - 1;
		}

		//��ȡ��Ϸ����
		inline int32_t GetScore()
		{
			return engine->score;
		}

		//�Ӽ����������븺������Ϊ����
		inline void AddScore(int32_t _change_score_)
		{
			engine->score += _change_score_;
		}

		//������Ƶ - ��Ϸ������ص���Ƶ
		inline void PlayAudio(AudioName Type, bool loop = false)
		{
			if (engine->AudioId[int(Type)] != -1)
			{
				olc::SOUND::PlaySample(engine->AudioId[int(Type)], loop);
			}
		}

		//������Ƶ - �Լ����ص���Ƶ
		inline void PlayAudio(int32_t Id, bool loop = false)
		{
			if (Id != -1)
			{
				olc::SOUND::PlaySample(Id, loop);
			}
		}

		//ֹͣ������Ƶ - ��Ϸ������ص���Ƶ
		inline void StopAudio(AudioName Type)
		{
			if (engine->AudioId[int(Type)] != -1)
			{
				olc::SOUND::StopSample(engine->AudioId[int(Type)]);
			}
		}

		//ֹͣ������Ƶ - �Լ����ص���Ƶ
		inline void StopAudio(int32_t Id)
		{
			if (Id != -1)
			{
				olc::SOUND::StopSample(Id);
			}
		}

		//��ȡͼƬ·��
		inline String GetImagePath()
		{
			return engine->picture_path;
		}

		//��ȡ������С
		inline float GetGravity()
		{
			return engine->m_world->GetGravity().y;
		}

		//��ȡ�������е���
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

		//��ȡ����������ͨ��
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

		//��ȡ�������е�����
		inline Vector<b2Body*> GetAllProps()
		{
			return engine->prop_body;
		}

		//����Ϊ������
		inline void SetPropBall(b2Body* body)
		{
			BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);
			info->prop = this;
			info->level = -1;
			engine->prop_body.push_back(body);
			info->prop->Generate(body);
		}

		//����Ϊ���������
		inline void SetRandomPropBall(b2Body* body)
		{
			engine->SGESetProp(body);
		}

		//������˸Ч��
		inline void SetFlash(b2Body * body,float flash_time = 0.0f)
		{
			engine->flash_list.emplace_back(body);
			if (flash_time != 0.0f)
			{
				engine->flash_list.back().clock = flash_time;
			}
		}

		//���ÿ����ƶ�Ч��
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

		//���úϳ���Ч
		inline void SetSyntheticEffect(const b2Vec2& effect_pos, int32_t effect_level)
		{
			engine->SGESetSEffect(effect_pos.x * engine->SGE_Scale_B2, effect_pos.y * engine->SGE_Scale_B2, effect_level);
		}

		//��ȡ�ϳ��б�
		inline Vector<Vector<b2Fixture*>>& GetTouchBallList()
		{
			return engine->touch_balls;
		}

		//�ϳ�������
		inline void Synthesis(b2Body* bodyA, b2Body* bodyB, int32_t level)
		{
			engine->PreSyntheticOperation(bodyA, bodyB, level);
		}

		//�Ƿ��Ѿ����������Ƿ���Ϸ�Ѿ�������
		inline bool IsDead()
		{
			return engine->dead;
		}

		//����ʤ��������Ч
		inline void SetSuccessEffect(int32_t effect_level)
		{
			engine->SGESetSuccess(effect_level);
		}
	};
}

#endif
#endif