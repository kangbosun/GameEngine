#pragma once
#include "Singleton.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace GameEngine
{
	class GAMEENGINE_API AudioEngine : public Singleton<AudioEngine>
	{
	private:
		CComPtr<IXAudio2> mXAudio2;
		IXAudio2MasteringVoice* mMasterVoice;

	public:
		~AudioEngine();
		AudioEngine();
		CComPtr<IXAudio2>& GetEngine();
	};

#define STREAMING_BUFFER_SIZE 131072
#define STREAMING_BUFFER_COUNT 3

	class GAMEENGINE_API MediaStreamer
	{
		friend class AudioClip;
		friend class AudioSFX;
		friend class AudioMusic;
	private:
		WAVEFORMATEX* mWaveFormat;
		unsigned int mLengthInBytes;
		float mLengthInSeconds;
		CComPtr<IMFSourceReader> mReader;

	public:
		MediaStreamer();
		~MediaStreamer();
		bool LoadWAVFile(const std::wstring& filename);
		bool GetNextBuffer(BYTE* buffer, DWORD bufferSize, DWORD* outBufferLen, bool* outEndofstream);
		void ResetBuffer();
		size_t GetEntireBuffer(BYTE* buffer, int maxBufferSize);
	};

	//////////////////////////////////////////////////////////////////////////////////
	//base type
	class GAMEENGINE_API AudioClip 
	{
	public:
		enum AudioClipState
		{
			ePlaying = 0,
			eStopped,
			ePaused
		};

	protected:
		std::wstring name;
		IXAudio2SourceVoice* mSourceVoice;
		IXAudio2SubmixVoice* mSubMixVoice;

		MediaStreamer mStreamer;

		bool bLoop;
		float mVolume;
		AudioClipState state;

		virtual void Initialize() = 0;

	public:
		AudioClip();
		~AudioClip();

		unsigned int GetLengthInBytes() { return mStreamer.mLengthInBytes; }
		float GetLengthInSeconds() { return mStreamer.mLengthInSeconds; }
		WAVEFORMATEX* GetWaveFormat() { return mStreamer.mWaveFormat; }

		void SetVolume(float volume);
		float GetVolume();

		AudioClipState GetState() { return state; }

		virtual void Play();
		virtual void Pause();
		virtual void Stop();
	};

	//streaming type
	class GAMEENGINE_API AudioMusic : public AudioClip
	{
	private:
		BYTE mAudioBuffers[STREAMING_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
		int mCurrentBuffer;
		void Initialize();
	public:
		AudioMusic();
		static std::shared_ptr<AudioMusic> CreateAudioClipFromFile(const std::wstring& filename);

		void Stream();
		void Stop();
	};

	//non-streaming type
	class GAMEENGINE_API AudioSFX : public AudioClip
	{
	private:
		std::shared_ptr<BYTE> mBuffer;
		void Initialize();

	public:
		AudioSFX();
		~AudioSFX();
		static std::shared_ptr<AudioSFX> CreateAudioClipFromFile(const std::wstring& filename);
		void Play();
		std::shared_ptr<AudioSFX> Clone();
	};
}

#pragma warning(pop)