#pragma once

#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_

//���ר�������������������
inline void YYYCZ::SyntheticGameEngine::CreateNewBall()
{
	next_ball = CreateNewBall(0.0f, 0.0f, BallRand(), b2Vec2_zero, true);

	if (last_mouse != b2Vec2_zero)
	{
		MouseMove(last_mouse);
	}
	else
	{
		next_ball->SetTransform(b2Vec2(B2_Width * 0.5f + SGE_RandomError,
			B2_Generate - next_ball->GetFixtureList()->GetShape()->m_radius), 0.0f);
	}

	next_ball->SetEnabled(false);
	next_ball->SetType(b2_staticBody);
}

inline void YYYCZ::SyntheticGameEngine::DestroyBall(b2Body* body)
{
	BodyInfo* info = (BodyInfo*)(body->GetUserData().pointer);
	SGEDeleteStretch(body);

	if (info->prop != nullptr)
		SGEFreeProp(body);

	delete info;
	m_world->DestroyBody(body);
}

b2Body* YYYCZ::SyntheticGameEngine::CreateNewBall(float x, float y, int level, const b2Vec2& velocity, bool contain_prop)
{
	//��ΪԲ��
	b2CircleShape shape;
	shape.m_p.SetZero();
	shape.m_radius = BallRadius[level];

	//����λ��
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position.Set(x, y);

	//���ô�С�ȼ�
	BodyInfo* new_info = new BodyInfo(level);

	body_def.userData.pointer = (uintptr_t)new_info;

	//���ɲ������ٶ�
	b2Body* body = m_world->CreateBody(&body_def);
	body->SetLinearVelocity(velocity);

	//�����������ԣ�������ѶȽ������ã�
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = 1.0f;
	fixture_def.friction = 0.2f / difficulty;
	fixture_def.restitution = 0.1f * difficulty;

	body->CreateFixture(&fixture_def);

	//������ɵ���
	if (contain_prop)
		SGESetProp(body);

	return body;
}

#endif