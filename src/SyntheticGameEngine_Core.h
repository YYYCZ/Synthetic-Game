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
	//����ͼƬ
	LoadPictures(picture_path);

	//�ٴ�������ͼ��
	//�ܹ�����ͼ�㣺����㡢��㡢������

	//����㣨���ϲ㣩
	Clear(olc::BLANK);

	//���
	uint32_t layer_index = CreateLayer();
	EnableLayer(layer_index, true);
	SetDrawTarget(layer_index);
	Clear(olc::BLANK);

	//������
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

	//����һ����������
	if (m_world != nullptr)
		delete m_world;
	m_world = new b2World(b2Vec2(0.0f, B2_Gravity));
	m_world->SetContactListener(this);

	//������������
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

	//������Ƶ
	olc::SOUND::InitialiseAudio();
	if (AudioId[int(AudioName::BackgroundAudio)] != -1)
		olc::SOUND::PlaySample(AudioId[int(AudioName::BackgroundAudio)]);

	//���߳�ʼ��
	for (auto x : props)
		x->GameStart();

	return true;
}

bool YYYCZ::SyntheticGameEngine::OnUserUpdate(float fElapsedTime)
{
	//��ͣ
	if (!IsFocused())
	{
		SGEDrawWindows();;
		return true;
	}

	//��˸Ч��
	if (!flash_list.empty())
		SGEFlash(fElapsedTime);

	//�����ƶ�Ч��
	if (!quick_move_list.empty())
		SGEQuickMove(fElapsedTime);

	//����Ч��
	if (!stretch_list.empty())
		SGEStretch(fElapsedTime);

	//�ϳ�Ч��
	if (!seffect_list.empty())
		SGEEffect(fElapsedTime);

	//ʤ������
	if (success_effect.clock > 0.0f)
		SGESuccess(fElapsedTime);

	//�Ƿ��Ѿ�����
	if (dead)
	{
		//��������Ч��
		if (dead_bomb_clock > 0.0f)
			dead_bomb_clock -= fElapsedTime;

		if (flash_list.empty() //��֤������˸�������ٽ��б�ը
			&& m_world->GetBodyCount() > 3 + int32_t(next_ball != nullptr)
			&& dead_bomb_clock <= 0.0f)
			dead_bomb_clock = DeadBombTime,
			DeadBomb();

		SGEDrawWindows();
		return true;
	}

	//��ʱ���Ĵ���
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

	//��������¼�
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

	//�������硰ʱ��������
	m_world->Step(fElapsedTime, 8, 3);

	//�ƻ�Ԥ����
	if (!destroy_list.empty())
	{
		for (auto x : destroy_list)
		{
			DestroyBall(x);
		}
		destroy_list.clear();
	}

	//���߼�ʱ����
	PropEvent(fElapsedTime);

	//���кϳ�
	SyntheticOperation();

	//���´���
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