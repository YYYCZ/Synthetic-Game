#pragma once

#ifdef _YYYCZ_SYNTHETIC_GAME_ENGINE_

void YYYCZ::SyntheticGameEngine::LoadPictures(const String& path)
{
	if (path.empty())
		return;

	//清除已加载的图片
	ClearPictures();
	pictures_center.clear();
	pictures_2inv.clear();

	effect_flower1_center.clear();
	effect_flower2_center.clear();
	effect_water_center.clear();

	auto load_func = [=](const String& file_path, olc::Decal*& new_decal)->bool
	{
		olc::Sprite* new_sprite = new olc::Sprite();
		if (new_sprite->LoadFromFile(file_path) == olc::rcode::OK)
		{
			new_decal = new olc::Decal(new_sprite);
			return true;
		}
		else
		{
			delete new_sprite;
			new_decal = nullptr;
			return false;
		}
	};

	auto vec_load_func = [=](const String& file_name, Vector<olc::Decal*>& picture_vec,
		Vector<olc::vf2d>& center_vec, Vector<olc::vf2d>* _2inv_vec = nullptr)->void
	{
		olc::Decal* new_decal;
		int32_t last_index = -1;
		for (int32_t i = 0; i < BallTypeNum; i++)
		{
			sprintf_s(string_buf, "%d", i);
			if (load_func(path + "\\" + file_name + String(string_buf) + "." + BehindName, new_decal))
			{
				last_index = i;
				picture_vec.push_back(new_decal);

				center_vec.emplace_back(new_decal->sprite->width / 2.0f, new_decal->sprite->height / 2.0f);

				if (_2inv_vec != nullptr)
					_2inv_vec->emplace_back(2.0f / new_decal->sprite->width, 2.0f / new_decal->sprite->height);
			}
			else if (last_index != -1)
			{
				sprintf_s(string_buf, "%d", last_index);
				if (load_func(path + "\\" + file_name + String(string_buf) + "." + BehindName, new_decal))
				{
					picture_vec.push_back(new_decal);

					center_vec.emplace_back(new_decal->sprite->width / 2.0f, new_decal->sprite->height / 2.0f);

					if (_2inv_vec != nullptr)
						_2inv_vec->emplace_back(2.0f / new_decal->sprite->width, 2.0f / new_decal->sprite->height);
				}
			}
			else
			{
				break;
			}
		}
	};

	olc::Decal* new_decal;

	//加载球体的图片
	vec_load_func(BallPicturesName, pictures, pictures_center, &pictures_2inv);

	//加载背景图
	if (load_func(path + "\\" + BackgroundPictureName, background))
		backgroud_scale = { SGE_Width_f / background->sprite->width ,
			SGE_Height_f / background->sprite->height };

	//加载死亡判定线图
	if (load_func(path + "\\" + LinePictureName, line))
		line_scale = { SGE_Width_f / line->sprite->width , 1.0f };

	//加载胜利闪光图
	if (load_func(path + "\\" + SuccessPictureName, success))
	{
		float sq_inv = sqrtf(SGE_Width_f * SGE_Height_f) / sqrtf(success->sprite->width * success->sprite->height);
		success_scale = { sq_inv  , sq_inv };
	}

	//加载特效图
	vec_load_func(FlowerAName, effect_flower1, effect_flower1_center);
	vec_load_func(FlowerBName, effect_flower2, effect_flower2_center);
	vec_load_func(WaterName, effect_water, effect_water_center);
}

void YYYCZ::SyntheticGameEngine::ClearPictures()
{
	if (background != nullptr)
	{
		delete background;
		background = nullptr;
	}

	if (line != nullptr)
	{
		delete line;
		line = nullptr;
	}

	if (success != nullptr)
	{
		delete success;
		success = nullptr;
	}

	auto vec_clear = [=](Vector<olc::Decal*>& decal_vec)->void
	{
		if (!decal_vec.empty())
		{
			for (auto& x : decal_vec)
			{
				delete x;
			}
			decal_vec.clear();
		}
	};

	vec_clear(pictures);
	vec_clear(effect_flower1);
	vec_clear(effect_flower2);
	vec_clear(effect_water);
}

#endif