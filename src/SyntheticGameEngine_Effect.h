#pragma once

#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_

inline void YYYCZ::SyntheticGameEngine::SGESetProp(b2Body* body)
{
	BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);

	if (!props.empty())
	{
		int _index_ = PropRand(props.size(), PProp);
		if (_index_ != -1)
		{
			info->level = -1;
			info->prop = props[_index_];
			body->GetFixtureList()->GetShape()->m_radius = info->prop->radius;
			prop_body.push_back(body);
			info->prop->Generate(body);
		}
	}
}

inline void YYYCZ::SyntheticGameEngine::SGEFreeProp(b2Body* body)
{
	BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);
	for (auto x = prop_body.begin(); x != prop_body.end(); x++)
	{
		if (*x == body)
		{
			prop_body.erase(x);
			info->prop->Free(body);
			break;
		}
	}

	info->prop = nullptr;
}

inline void YYYCZ::SyntheticGameEngine::SGEClearProp()
{
	decltype(prop_body) temp_list;
	temp_list.swap(prop_body);

	for (auto x : temp_list)
	{
		BodyInfo* info = (BodyInfo*)(x->GetUserData().pointer);
		auto temp_ptr = info->prop;
		info->prop = nullptr;

		temp_ptr->Free(x);
	}
}

inline void YYYCZ::SyntheticGameEngine::SGEFlash(float fElapsedTime)
{
	bool flag = true;
	for (auto& x : flash_list)
	{
		if (x.clock > 0.0f)
		{
			x.clock -= fElapsedTime;
			if (x.clock > 0.0f)
			{
				flag = false;
				if (x.minus)
				{
					x.alpha -= FlashStep * fElapsedTime;
					if (x.alpha <= 0.0f)
					{
						x.alpha = fabsf(x.alpha);
						x.minus = false;
					}
				}
				else
				{
					x.alpha += FlashStep * fElapsedTime;
					if (x.alpha >= 255.0f)
					{
						x.alpha = 510.0f - x.alpha;
						x.minus = true;
					}
				}
			}
		}
	}

	if (flag)
		flash_list.clear();
}

inline void YYYCZ::SyntheticGameEngine::SGEQuickMove(float fElapsedTime)
{
	Vector<int32_t> clear_list;
	int32_t _index_ = -1;

	for (auto& x : quick_move_list)
	{
		_index_++;
		auto _distance_ = x.end_pos - x.now_pos;
		float x_y = abs(_distance_.x) + abs(_distance_.y),
			k_move = x.speed * fElapsedTime;

		if (k_move >= x_y)
		{
			clear_list.push_back(_index_);
		}
		else
		{
			x.now_pos.x += k_move * _distance_.x / x_y;
			x.now_pos.y += k_move * _distance_.y / x_y;
		}
	}

	for (auto x = clear_list.rbegin(); x != clear_list.rend(); x++)
		quick_move_list.erase(quick_move_list.begin() + *x);
}

inline void YYYCZ::SyntheticGameEngine::SGESetStretch(b2Body* body)
{
	BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);

	Stretch new_stretch;
	new_stretch.body = body;

	new_stretch.target_radius = { body->GetFixtureList()->GetShape()->m_radius * (1.0f + StretchRate),
	body->GetFixtureList()->GetShape()->m_radius };

	body->GetFixtureList()->GetShape()->m_radius = 0.0f;

	new_stretch.step =
		(new_stretch.target_radius.front() + fabsf(new_stretch.target_radius.front()
			- new_stretch.target_radius.back()))
		/ (StretchTime == 0.0f ? 0.2f : StretchTime);

	stretch_list.push_back(new_stretch);
}

inline void YYYCZ::SyntheticGameEngine::SGEDeleteStretch(b2Body* body)
{
	for (auto x = stretch_list.begin(); x != stretch_list.end(); x++)
	{
		if (x->body == body)
		{
			x->body->GetFixtureList()->GetShape()->m_radius = x->target_radius.back();
			stretch_list.erase(x);
			return;
		}
	}
}

inline void YYYCZ::SyntheticGameEngine::SGEStretch(float fElapsedTime)
{
	Vector<int32_t> clear_list;
	int32_t _index_ = -1;

	for (auto& x : stretch_list)
	{
		_index_++;

		if (x.target_radius.empty())
		{
			clear_list.push_back(_index_);
			continue;
		}

		float _end_radius_ = x.body->GetFixtureList()->GetShape()->m_radius + x.step * fElapsedTime;

		auto TwoSignRadius = [=](float _x_, float _y_, float _z_)->bool
		{
			if (_x_ >= _y_ && _y_ >= _z_)
			{
				return true;
			}
			else if (_x_ <= _y_ && _y_ <= _z_)
			{
				return true;
			}
			return false;
		};

		if (TwoSignRadius(x.body->GetFixtureList()->GetShape()->m_radius, x.target_radius.front(), _end_radius_))
		{
			if (x.target_radius.size() == 1)
			{
				x.body->GetFixtureList()->GetShape()->m_radius = x.target_radius.front();
				x.target_radius.clear();
				clear_list.push_back(_index_);
			}
			else if (TwoSignRadius(x.target_radius.front(), _end_radius_, *x.target_radius.begin()++))
			{
				x.body->GetFixtureList()->GetShape()->m_radius = _end_radius_;
				x.target_radius.erase(x.target_radius.begin());
			}
			else
			{
				x.step = -x.step;
				x.body->GetFixtureList()->GetShape()->m_radius =
					x.target_radius.front() + x.target_radius.front() - _end_radius_;
				x.target_radius.erase(x.target_radius.begin());
			}
		}
		else
		{
			x.body->GetFixtureList()->GetShape()->m_radius = _end_radius_;
		}
	}

	for (auto x = clear_list.rbegin(); x != clear_list.rend(); x++)
		stretch_list.erase(stretch_list.begin() + *x);
}

inline void YYYCZ::SyntheticGameEngine::SGESetSEffect(float sge_x_, float sge_y_, int32_t level)
{
	if (level < 0 || level >= BallTypeNum)
		return;

	seffect_list.emplace_back(olc::vf2d(sge_x_, sge_y_),
		effect_water.empty() ? nullptr : effect_water[level],
		effect_water_center.empty() ? olc::vf2d(0, 0) : effect_water_center[level],
		effect_flower1.empty() ? nullptr : effect_flower1[level],
		effect_flower1_center.empty() ? olc::vf2d(0, 0) : effect_flower1_center[level],
		effect_flower2.empty() ? nullptr : effect_flower2[level],
		effect_flower2_center.empty() ? olc::vf2d(0, 0) : effect_flower2_center[level],
		0.5f, 5 * BallRadius[level], SGE_BallRadius[level]);
}

inline void YYYCZ::SyntheticGameEngine::SGEDrawSEffect()
{
	if (seffect_list.empty())
		return;

	for (auto& s_effect : seffect_list)
	{
		//绘制 水 效果
		if (s_effect.water != nullptr)
		{
			DrawRotatedDecal(s_effect.pos, s_effect.water,
				0.0f, s_effect.water_center,
				olc::vf2d(s_effect.wscale, s_effect.wscale),
				olc::Pixel(255, 255, 255, s_effect.alpha));
		}

		//绘制 花瓣 效果
		if (s_effect.flower1 != nullptr)
		{
			for (auto& x : s_effect.centers)
			{
				DrawRotatedDecal(x.center, x.is_one ? s_effect.flower1 : s_effect.flower2,
					x.center_angle, x.is_one ? s_effect.flower1_center : s_effect.flower1_center,
					{ s_effect.fscale,s_effect.fscale }, olc::Pixel(255, 255, 255, x.falpha));
			}
		}
	}
}

inline void YYYCZ::SyntheticGameEngine::SGEEffect(float fElapsedTime)
{
	Vector<int32_t> clear_list;
	int32_t _index_ = -1;

	for (auto& s_effect : seffect_list)
	{
		_index_++;

		if (s_effect.clock > 0.0f)
		{
			s_effect.clock -= fElapsedTime;

			//水效果的处理（渐隐处理）
			if (s_effect.water != nullptr)
			{
				s_effect.alpha += s_effect.step * fElapsedTime;
			}

			//花瓣效果的处理
			//花瓣扩散、自转、略微渐隐 -> 扩散到最大半径消失
			//临界点刚好是 half_clock
			if (s_effect.flower1 != nullptr || s_effect.flower2 != nullptr)
			{
				if (s_effect.clock > s_effect.half_clock)
				{
					float angle_add = s_effect.wspeed * fElapsedTime;
					float alpha_add = s_effect.falpha_step * fElapsedTime;

					for (auto& x : s_effect.centers)
					{
						x.center += x.speed * fElapsedTime;
						x.center_angle += angle_add;
						x.falpha += alpha_add;
					}
				}

				if (s_effect.clock <= s_effect.half_clock)
				{
					s_effect.fscale += s_effect.fscale_step * fElapsedTime;
				}
			}
		}

		if (s_effect.clock <= 0.0f)
			clear_list.push_back(_index_);
	}

	for (auto x = clear_list.rbegin(); x != clear_list.rend(); x++)
		seffect_list.erase(seffect_list.begin() + *x);
}

inline void YYYCZ::SyntheticGameEngine::SGESetSuccess(int32_t level)
{
	if (level < 0 || level >= BallTypeNum)
		return;

	success_effect.clock = 3.0f;
	success_effect.scale_clock = 2.8f;
	success_effect.level = level;
	success_effect.scale = 0.0f;
	success_effect.scale_step = 4.0f;
}

inline void YYYCZ::SyntheticGameEngine::SGESuccess(float fElapsedTime)
{
	success_effect.clock -= fElapsedTime;
	success_effect.scale_clock -= fElapsedTime;
	success_effect.angle += success_effect.step * fElapsedTime;

	if (success_effect.scale_step != 0.0f)
		success_effect.scale += success_effect.scale_step * fElapsedTime;

	if (success_effect.scale > 1.3f)
	{
		success_effect.scale_step *= -0.5f;
		success_effect.scale = 1.3f;
	}

	if (success_effect.scale_step < 0.0f && success_effect.scale <= 1.0f && success_effect.scale_clock > 0.0f)
	{
		success_effect.scale = 1.0f;
		success_effect.scale_step = 0.0f;
	}

	if (success_effect.scale_step == 0.0f && success_effect.scale_clock <= 0.0f)
	{
		success_effect.scale_step = -1.0f / success_effect.clock;
	}
}

#endif