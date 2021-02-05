#pragma once

#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_

void YYYCZ::SyntheticGameEngine::MouseMove(const b2Vec2& p)
{
	if (next_ball == nullptr)
	{
		if (rebuild_time > 0.0f)
			return;
		CreateNewBall();
		SGESetStretch(next_ball);
	}
	else
	{
		if (p.x - next_ball->GetFixtureList()->GetShape()->m_radius < B2_Left)
			next_ball->SetTransform(b2Vec2(B2_Left + next_ball->GetFixtureList()->GetShape()->m_radius + SGE_RandomError,
				B2_Generate - next_ball->GetFixtureList()->GetShape()->m_radius), 0.0f);
		else if (p.x + next_ball->GetFixtureList()->GetShape()->m_radius > B2_Right)
			next_ball->SetTransform(b2Vec2(B2_Right - next_ball->GetFixtureList()->GetShape()->m_radius + SGE_RandomError,
				B2_Generate - next_ball->GetFixtureList()->GetShape()->m_radius), 0.0f);
		else
			next_ball->SetTransform(b2Vec2(p.x + SGE_RandomError,
				B2_Generate - next_ball->GetFixtureList()->GetShape()->m_radius), 0.0f);

	}
}

void YYYCZ::SyntheticGameEngine::MouseDown(const b2Vec2& p)
{
	if (next_ball != nullptr)
	{
		combo = 0;
		score++;

		next_ball->SetType(b2_dynamicBody);
		next_ball->SetEnabled(true);

		BodyInfo* info = (BodyInfo*)(next_ball->GetUserData().pointer);
		if (info->prop != nullptr)
			info->prop->Down(next_ball);

		next_ball = nullptr;
		rebuild_time = RebuilTime;
		MouseMove(p);
	}
}

bool YYYCZ::SyntheticGameEngine::OnUserCreate()
{
	//加载图片
	LoadPictures(picture_path);

	//再创建两个图层
	//总共三个图层：字体层、球层、背景层

	//字体层（最上层）
	Clear(olc::BLANK);

	//球层
	uint32_t layer_index = CreateLayer();
	EnableLayer(layer_index, true);
	SetDrawTarget(layer_index);
	Clear(olc::BLANK);

	//背景层
	layer_index = CreateLayer();
	EnableLayer(layer_index, true);
	SetDrawTarget(layer_index);
	if (background != nullptr)
	{
		DrawDecal({ 0.0f,0.0f }, background, backgroud_scale);
	}
	else
	{
		Clear(BackGroundColor);
	}

	//创建一个物理世界
	if (m_world != nullptr)
		delete m_world;
	m_world = new b2World(b2Vec2(0.0f, B2_Gravity));
	m_world->SetContactListener(this);

	//塑造物理世界
	b2BodyDef body_def;
	BodyInfo* new_info = new BodyInfo(-2);
	body_def.userData.pointer = (uintptr_t)new_info;

	b2EdgeShape shape;
	shape.SetTwoSided(b2Vec2(0.0f, B2_Height), b2Vec2(B2_Width, B2_Height));
	b2Body* body = m_world->CreateBody(&body_def);
	body->CreateFixture(&shape, 1.0f);

	shape.SetTwoSided(b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, B2_Height));
	body = m_world->CreateBody(&body_def);
	body->CreateFixture(&shape, 1.0f);

	shape.SetTwoSided(b2Vec2(B2_Width, 0.0f), b2Vec2(B2_Width, B2_Height));
	body = m_world->CreateBody(&body_def);
	body->CreateFixture(&shape, 1.0f);

	CreateNewBall();

	//播放音频
	olc::SOUND::InitialiseAudio();
	if (AudioId[int(AudioName::BackgroundAudio)] != -1)
		olc::SOUND::PlaySample(AudioId[int(AudioName::BackgroundAudio)]);

	//道具初始化
	for (auto x : props)
		x->GameStart();

	return true;
}

bool YYYCZ::SyntheticGameEngine::OnUserUpdate(float fElapsedTime)
{
	//暂停
	if (!IsFocused())
	{
		SGEDrawWindows();;
		return true;
	}

	//闪烁效果
	if (!flash_list.empty())
		SGEFlash(fElapsedTime);

	//快速移动效果
	if (!quick_move_list.empty())
		SGEQuickMove(fElapsedTime);

	//伸缩效果
	if (!stretch_list.empty())
		SGEStretch(fElapsedTime);

	//合成效果
	if (!seffect_list.empty())
		SGEEffect(fElapsedTime);

	//胜利闪光
	if (success_effect.clock > 0.0f)
		SGESuccess(fElapsedTime);

	//是否已经死亡
	if (dead)
	{
		//死亡爆裂效果
		if (dead_bomb_clock > 0.0f)
			dead_bomb_clock -= fElapsedTime;

		if (flash_list.empty() //保证死亡闪烁结束后再进行爆炸
			&& m_world->GetBodyCount() > 3 + int32_t(next_ball != nullptr)
			&& dead_bomb_clock <= 0.0f)
			dead_bomb_clock = DeadBombTime,
			DeadBomb();

		SGEDrawWindows();
		return true;
	}

	//计时器的处理
	if (rebuild_time > 0.0f)
	{
		rebuild_time -= fElapsedTime;
		if (rebuild_time <= 0.0f)
			MouseMove(last_mouse);
	}
	if (DeadJudge())
	{
		dead_clock -= fElapsedTime;
		if (dead_clock <= 0.0f)
		{
			dead = true;
			DeadFlash();

			if (next_ball != nullptr)
			{
				SGESetSEffect(next_ball->GetPosition().x * SGE_Scale_B2, next_ball->GetPosition().y * SGE_Scale_B2,
					((BodyInfo*)(next_ball->GetUserData().pointer))->level);
				DestroyBall(next_ball);
				next_ball = nullptr;
			}

			if (AudioId[int(AudioName::BackgroundAudio)] != -1)
				olc::SOUND::StopSample(AudioId[int(AudioName::BackgroundAudio)]);
			if (AudioId[int(AudioName::DeadAudio)] != -1)
				olc::SOUND::PlaySample(AudioId[int(AudioName::DeadAudio)]);
			return true;
		}
	}
	else if (dead_clock != DeadTime)
	{
		dead_clock = DeadTime;
	}

	//处理鼠标事件
	b2Vec2 mouse_move(GetMouseX() / SGE_Scale_B2, GetMouseY() / SGE_Scale_B2);
	if (mouse_move != last_mouse)
	{
		MouseMove(mouse_move);
		last_mouse = mouse_move;
	}
	if (GetMouse(0).bPressed)
	{
		MouseDown(mouse_move);
	}

	//物理世界“时间流动”
	m_world->Step(fElapsedTime, 8, 3);

	//破坏预处理
	if (!destroy_list.empty())
	{
		for (auto x : destroy_list)
		{
			DestroyBall(x);
		}
		destroy_list.clear();
	}

	//道具计时处理
	PropEvent(fElapsedTime);

	//进行合成
	SyntheticOperation();

	//更新窗口
	SGEDrawWindows();

	return true;
}

bool YYYCZ::SyntheticGameEngine::OnUserDestroy()
{
	if (m_world != nullptr)
		delete m_world;
	m_world = nullptr;

	for (auto x : props)
	{
		delete x;
	}
	props.clear();

	last_mouse = b2Vec2_zero;

	touch_balls;
	next_ball = nullptr;
	score = 0;
	dead = false;

	rebuild_time = 0.0f;
	dead_clock = 0.0f;

	flash_list.clear();
	quick_move_list.clear();
	stretch_list.clear();

	olc::SOUND::DestroyAudio();

	return true;
}

void YYYCZ::SyntheticGameEngine::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	B2_NOT_USED(oldManifold);

	if (contact->IsTouching()
		&& NotBeDestroy(contact->GetFixtureA()->GetBody())
		&& NotBeDestroy(contact->GetFixtureB()->GetBody()))
	{
		BodyInfo* infoA = (BodyInfo*)(contact->GetFixtureA()->GetBody()->GetUserData().pointer);
		BodyInfo* infoB = (BodyInfo*)(contact->GetFixtureB()->GetBody()->GetUserData().pointer);

		if (infoA->prop != nullptr)
		{
			infoA->prop->Crash(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody());
		}
		if (infoB->prop != nullptr)
		{
			infoB->prop->Crash(contact->GetFixtureB()->GetBody(), contact->GetFixtureA()->GetBody());
		}

		if (infoA->level == infoB->level &&
			infoA->level >= 0 && infoA->level < BallTypeNum - 1)
		{
			PreSyntheticOperation(contact->GetFixtureA()->GetBody(), contact->GetFixtureB()->GetBody(), infoA->level);
		}
	}
}

#endif