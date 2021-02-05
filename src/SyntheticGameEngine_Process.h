#pragma once

#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_

bool YYYCZ::SyntheticGameEngine::DeadJudge()
{
	b2Body* body = m_world->GetBodyList();
	int32_t body_num = m_world->GetBodyCount();

	for (int32_t i = 0; i < body_num; i++, body = body->GetNext())
	{
		BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);

		if (info->level >= -1 && info->level < BallTypeNum && body != next_ball
			&& body->GetPosition().y - B2_Line < body->GetFixtureList()->GetShape()->m_radius
			&& body->GetLinearVelocity().y <= 0.0f)
			return true;
	}


	return false;
}

void YYYCZ::SyntheticGameEngine::DeadFlash()
{
	b2Body* body = m_world->GetBodyList();
	int32_t body_num = m_world->GetBodyCount();

	for (int32_t i = 0; i < body_num; i++, body = body->GetNext())
	{
		BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);

		if (info->level >= -1 && info->level < BallTypeNum && body != next_ball
			&& body->GetPosition().y - B2_Line < body->GetFixtureList()->GetShape()->m_radius)
		{
			bool flag = true;

			for (auto& x : flash_list)
			{
				if (x.body == body)
				{
					if (x.clock <= 0.0f)
					{
						flag = false;
						x.alpha = 255.0f;
						x.minus = true;
						x.clock = 1.5f;
					}
				}
			}

			if (flag)
			{
				flash_list.emplace_back(body);
			}
		}
	}
}

void YYYCZ::SyntheticGameEngine::DeadBomb()
{
	b2Body* body = m_world->GetBodyList();
	int32_t body_num = m_world->GetBodyCount();

	for (int32_t i = 0; i < body_num; i++, body = body->GetNext())
	{
		BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);

		if (info->level >= -1 && info->level < BallTypeNum && body != next_ball)
		{
			SGESetSEffect(body->GetPosition().x * SGE_Scale_B2, body->GetPosition().y * SGE_Scale_B2, info->level);
			DestroyBall(body);

			score += info->level + 1;

			if (AudioId[int(AudioName::SynthesisAudio)] != -1)
				olc::SOUND::PlaySample(AudioId[int(AudioName::SynthesisAudio)]);

			return;
		}
	}
}

int YYYCZ::SyntheticGameEngine::FindFixture(b2Fixture* _find_)
{
	int _index_ = -1;
	for (auto& x : touch_balls)
	{
		_index_++;
		for (auto y : x)
		{
			if (_find_ == y)
			{
				return _index_;
			}
		}
	}
	return -1;
}

inline bool YYYCZ::SyntheticGameEngine::NotBeDestroy(b2Body* body)
{
	for (auto x : destroy_list)
	{
		if (x == body)
		{
			return false;
		}
	}
	return true;
}

inline void YYYCZ::SyntheticGameEngine::PreSyntheticOperation(b2Body* body1, b2Body* body2, int32_t level)
{
	int _index_1 = FindFixture(body1->GetFixtureList()),
		_index_2 = FindFixture(body2->GetFixtureList());

	if (_index_1 != -1 && _index_2 != -1)
	{
		if (level < BallTypeNum - touch_balls[_index_1].size() - touch_balls[_index_2].size() + 1)
		{
			for (auto& x : touch_balls[_index_2])
			{
				touch_balls[_index_1].push_back(x);
			}
			touch_balls.erase(touch_balls.begin() + _index_2);
		}
	}
	else if (_index_1 != -1)
	{
		if (level < BallTypeNum - touch_balls[_index_1].size())
		{
			touch_balls[_index_1].push_back(body2->GetFixtureList());
		}
	}
	else if (_index_2 != -1)
	{
		if (level < BallTypeNum - touch_balls[_index_2].size())
		{
			touch_balls[_index_2].push_back(body1->GetFixtureList());
		}
	}
	else
	{
		touch_balls.push_back({ body1->GetFixtureList(),body2->GetFixtureList() });
	}
}

inline void YYYCZ::SyntheticGameEngine::SyntheticOperation()
{
	size_t _size_ = 0;
	while (_size_ < touch_balls.size())
	{
		auto& fixtures = touch_balls[_size_++];
		BodyInfo* info = (BodyInfo*)(fixtures[0]->GetBody()->GetUserData().pointer);

		int32_t level = info->level + fixtures.size() - 1;

		auto pos_sort = [=](b2Fixture* _A_, b2Fixture* _B_)->bool
		{
			if (_A_->GetBody()->GetPosition().y < _B_->GetBody()->GetPosition().y)
			{
				return false;
			}
			return true;
		};

		std::sort(fixtures.begin(), fixtures.end(), pos_sort);

		b2Vec2 position = fixtures.front()->GetBody()->GetPosition();

		auto ComboAdd = [=](int32_t _combo_, int32_t _add_)->float
		{
			return ((_combo_ + _add_) * (_combo_ + _add_ + 1) - (_combo_) * (_combo_ + 1)) * 0.5f;
		};

		score += level * (fixtures.size() - 1 + ComboAdd(combo, fixtures.size() - 2));
		combo += fixtures.size() - 1;

		auto QMf = [=](const b2Vec2& _distance_)->float
		{
			return (abs(_distance_.x) + abs(_distance_.y));
		};

		for (auto x : fixtures)
		{
			if (x->GetBody()->GetPosition() != position)
			{
				info = (BodyInfo*)(x->GetBody()->GetUserData().pointer);

				quick_move_list.emplace_back(SGE_BallRadius[info->level],
					pictures.empty() ? nullptr : pictures[info->level],
					BallColor[info->level],
					x->GetBody()->GetAngle(),
					x->GetBody()->GetPosition(),
					position);
				quick_move_list.back().speed *= QMf(quick_move_list.back().end_pos - quick_move_list.back().now_pos);
			}

			DestroyBall(x->GetBody());
		}


		if (AudioId[int(AudioName::SynthesisAudio)] != -1)
			olc::SOUND::PlaySample(AudioId[int(AudioName::SynthesisAudio)]);

		SGESetStretch(CreateNewBall(position.x, position.y, level, fixtures[0]->GetBody()->GetLinearVelocity()));
		SGESetSEffect(position.x * SGE_Scale_B2, position.y * SGE_Scale_B2, level - 1);

		if (level == BallTypeNum - 1)
		{
			score += 100;
			SGESetSuccess(BallTypeNum - 1);

			if (AudioId[int(AudioName::SuccessAudio)] != -1)
				olc::SOUND::PlaySample(AudioId[int(AudioName::SuccessAudio)]);
		}
	}
	touch_balls.clear();
}

inline void YYYCZ::SyntheticGameEngine::PropEvent(float fElapsedTime)
{
	if (!prop_body.empty())
	{
		for (auto x : prop_body)
		{
			BodyInfo* info = (BodyInfo*)(x->GetUserData().pointer);
			info->prop->Clock(x, fElapsedTime);
		}
	}
}

#endif