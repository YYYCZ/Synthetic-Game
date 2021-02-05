#pragma once

#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_

inline YYYCZ::SyntheticGameEngine::SyntheticGameEngine()
{
	sAppName = "Synthetic Game - By YYYCZ";
}

inline YYYCZ::SyntheticGameEngine::~SyntheticGameEngine()
{
	if (m_world != nullptr)
		delete m_world,
		m_world = nullptr;

	ClearPictures();
}

inline bool YYYCZ::SyntheticGameEngine::GameStart(bool vsync)
{
	GetLayers().clear();

	if (Construct(SGE_Width, SGE_Height, 1, 1, false, vsync) != olc::rcode::OK ||
		Start() != olc::rcode::OK)
		return false;

	return true;
}

inline void YYYCZ::SyntheticGameEngine::SetPicturePath(const String& path)
{
	picture_path = path;
}

inline std::string YYYCZ::SyntheticGameEngine::GetPicturePath()
{
	return picture_path;
}

inline void YYYCZ::SyntheticGameEngine::SetScale(float scale)
{
	SGE_Scale_B2 = scale;
	ScaleMatch();
}

inline void YYYCZ::SyntheticGameEngine::SetSGESize(int32_t width, int32_t height)
{
	SGE_Width = width;
	SGE_Height = height;
	FloatSet();
	ScaleMatch();
}

inline void YYYCZ::SyntheticGameEngine::SetLineHeight(int32_t height)
{
	SGE_Line = height;
	SGE_Generate = SGE_Line * 4 / 5;
	FloatSet();
	ScaleMatch();
}

inline void YYYCZ::SyntheticGameEngine::SetColor(ColorArea area, const olc::Pixel& color)
{
	switch (area)
	{
	case ColorArea::BackgroundArea:
		BackGroundColor = color;
		break;
	case ColorArea::FontArea:
		FontColor = color;
		break;
	}
}

inline void YYYCZ::SyntheticGameEngine::SetFileName(FileName file_type, const String& file_name)
{
	switch (file_type)
	{
	case FileName::BackgroundName:
		BackgroundPictureName = file_name;
		break;
	case FileName::LineName:
		LinePictureName = file_name;
		break;
	case FileName::BallName:
		BallPicturesName = file_name;
		break;
	case FileName::BallBehindName:
		BehindName = file_name;
		break;
	case FileName::EffectWaterName:
		WaterName = file_name;
		break;
	case FileName::EffectFlowerAName:
		FlowerAName = file_name;
		break;
	case FileName::EffectFlowerBName:
		FlowerBName = file_name;
		break;
	case FileName::SuccessName:
		SuccessPictureName = file_name;
		break;
	}
}

inline void YYYCZ::SyntheticGameEngine::SetDifficulty(float _difficulty_)
{
	if (_difficulty_ < 1.0f || _difficulty_ > 10.0f)
		return;
	GameDifficulty = _difficulty_;
	difficulty = GameDifficulty / 5.0f;
}

inline void YYYCZ::SyntheticGameEngine::SetSGEName(const String& app_name)
{
	sAppName = app_name;
}

inline void YYYCZ::SyntheticGameEngine::SetPropProbability(float _p_)
{
	if (_p_ < 0.0f || _p_ > 1.0f)
		return;
	PProp = _p_;
}

inline void YYYCZ::SyntheticGameEngine::LoadAudio(AudioName AudioType, const String& AudioPath)
{
	AudioId[int(AudioType)] = olc::SOUND::LoadAudioSample(AudioPath);
}

template<class _Ty>
inline void YYYCZ::SyntheticGameEngine::LoadProp(const String& picture_file_name)
{
	_Ty* new_prop = new _Ty;
	new_prop->engine = this;

	if (picture_file_name.empty())
		new_prop->SetPropImagePath(GetPicturePath() + "\\" + new_prop->name + ".png");
	else
		new_prop->SetPropImagePath(GetPicturePath() + "\\" + picture_file_name);

	props.push_back(new_prop);
}

#endif