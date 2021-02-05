#include "SyntheticGame.h"
#define PHOTO_LOAD_TEST1
#define SOUND_LOAD_TEST1

#ifdef PHOTO_LOAD_TEST
class PLTest :public olc::PixelGameEngine
{
	using String = std::string;

public:
	PLTest()
	{
		sAppName = "Picture Load Test";
	}

	olc::Sprite * spr_test = nullptr;
	olc::Decal* dec_test = nullptr;
	olc::vf2d scale;

public:
	bool OnUserCreate() override
	{
		String file_name;

		if (std::cout << "������ͼƬ��·����" << std::endl && std::cin >> file_name)
		{
			spr_test = new olc::Sprite;
			if (spr_test->LoadFromFile(file_name) == olc::rcode::OK)
			{
				dec_test = new olc::Decal(spr_test);
				std::cout << ScreenWidth() << std::endl;
				std::cout << ScreenHeight() << std::endl;
				scale = { ScreenWidth() / float(dec_test->sprite->width),
					ScreenHeight() / float(dec_test->sprite->height) };
				std::cout << "��ͼƬ�ɹ���" << std::endl;
				return true;
			}
		}

		std::cout << "��ͼƬʧ�ܣ�" << std::endl;
		return false;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		
		DrawDecal({0,0}, dec_test, scale);
		return true;
	}

	bool OnUserDestroy() override
	{
		if (spr_test != nullptr)
			delete spr_test,
			spr_test = nullptr;

		if (dec_test != nullptr)
			delete dec_test,
			dec_test = nullptr;

		return true;
	}
};
#elif SOUND_LOAD_TEST
class WAVTest :public olc::PixelGameEngine
{
	using String = std::string;

public:
	WAVTest()
	{
		sAppName = "Wave Load Test";
	}

	int WavId = -1;

public:
	bool OnUserCreate() override
	{
		String wave_path;
		olc::SOUND::InitialiseAudio(44100U, 2U);

		if (std::cin >> wave_path)
		{
			WavId = olc::SOUND::LoadAudioSample(wave_path);
			if (WavId != -1)
			{
				std::cout << "��Ƶ���سɹ���" << std::endl;
				olc::SOUND::PlaySample(WavId, true);
				return true;
			}
		}

		std::cout << "��Ƶ����ʧ�ܣ�" << std::endl;
		return false;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		return true;
	}

	bool OnUserDestroy() override
	{
		olc::SOUND::DestroyAudio();
	}
};
#endif

int main()
{
#ifdef PHOTO_LOAD_TEST
	//ͼƬ���ز���
	PLTest Test;
	if (Test.Construct(800, 600, 1, 1))
		Test.Start();
#else 
#ifdef SOUND_LOAD_TEST
	//��Ƶ���ز���
	WAVTest Test;
	if (Test.Construct(100, 100, 1, 1))
		Test.Start();
#else
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
	YYYCZ::GameStart();
#endif
#endif

	return 0;
}