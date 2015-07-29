#include "enginepch.h"
#include "Audio.h"
#include "DXUtil.h"

namespace GameEngine
{
	//  AudioEngine
	AudioEngine::~AudioEngine()
	{
		if(mMasterVoice)
			mMasterVoice->DestroyVoice();

		HRESULT hr = MFShutdown();
		if(FAILED(hr)) {
			Debug("MFShutdown Failed");
		}

		CoUninitialize();
	}

	AudioEngine::AudioEngine()
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		HRESULT hr = XAudio2Create(&mXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
		if(FAILED(hr)) {
			Debug("Failed to XAudio2Create");
		}
		hr = mXAudio2->CreateMasteringVoice(&mMasterVoice);
		if(FAILED(hr)) {
			Debug("Failed to CreateMasteringVoice");
		}

		hr = MFStartup(MF_VERSION);
		if(FAILED(hr)) {
			Debug("MFCStartup Failed");
		}
	}

	CComPtr<IXAudio2>& AudioEngine::GetEngine()
	{
		return mXAudio2;
	}

	// MediaStreamer

	MediaStreamer::MediaStreamer()
	{
		mWaveFormat = nullptr;
		mReader = nullptr;
	}

	MediaStreamer::~MediaStreamer()
	{
		if(mWaveFormat)
			CoTaskMemFree(mWaveFormat);
	}

	bool MediaStreamer::LoadWAVFile(const std::wstring& filename)
	{
		if(mWaveFormat)
			CoTaskMemFree(mWaveFormat);
		HRESULT hr = MFCreateSourceReaderFromURL(filename.c_str(), nullptr, &mReader.p);

		if(FAILED(hr)) {
			Debug("MFCreateSourceReaderFromURL Failed");
			return false;
		}

		CComPtr<IMFMediaType> mediaType, outMediaType;
		hr = MFCreateMediaType(&mediaType.p);
		if(FAILED(hr)) {
			Debug("MFCreateMediaType Failed");
			return false;
		}

		mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

		mediaType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, UINT32(false));

		mReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, mediaType.p);

		hr = mReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &outMediaType.p);
		if(FAILED(hr)) {
			Debug("GetCurrentMediaType Failed");
			return false;
		}

		unsigned int formatSize = 0;
		hr = MFCreateWaveFormatExFromMFMediaType(outMediaType.p, &mWaveFormat, &formatSize);
		if(FAILED(hr)) {
			Debug("MFCreateWaveFormatExFromMFMediaType Failed");
			return false;
		}

		PROPVARIANT var;
		hr = mReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var);

		if(FAILED(hr)) {
			Debug("GetPresentationAttribute Failed");
			return false;
		}

		LONGLONG duration = var.uhVal.QuadPart;
		double durationInSeconds = (duration / (double)(10000000));
		mLengthInSeconds = (float)durationInSeconds;
		unsigned int bytes = (unsigned int)(durationInSeconds * mWaveFormat->nAvgBytesPerSec);
		mLengthInBytes = ((bytes + 3) >> 2) << 2;
		return true;
	}

	bool MediaStreamer::GetNextBuffer(BYTE* buffer, DWORD bufferSize, DWORD* outBufferLen, bool* outEndofstream)
	{
		if(!mReader)
			return false;

		CComPtr<IMFSample> sample;
		CComPtr<IMFMediaBuffer> mediaBuffer;
		DWORD dwFlags = 0;

		HRESULT hr = mReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, 0, &dwFlags, NULL, &sample.p);
		if(FAILED(hr)) {
			Debug("ReadSample failed");
			return false;
		}

		if(dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
			*outEndofstream = true;
			return true;
		}

		sample->ConvertToContiguousBuffer(&mediaBuffer.p);
		BYTE* bytes = nullptr;
		mediaBuffer->Lock(&bytes, 0, outBufferLen);
		mediaBuffer->Unlock();
		if(bufferSize >= *outBufferLen) {
			memcpy(buffer, bytes, *outBufferLen);
			return true;
		}
		else
			return false;
	}

	void MediaStreamer::ResetBuffer()
	{
		PROPVARIANT var = { 0 };
		var.vt = VT_I8;
		mReader->SetCurrentPosition(GUID_NULL, var);
	}

	size_t MediaStreamer::GetEntireBuffer(BYTE* buffer, int maxBufferSize)
	{
		if(!mReader)
			return 0;

		if(buffer == nullptr)
			return 0;

		CComPtr<IMFSample> sample;
		CComPtr<IMFMediaBuffer> mediaBuffer;
		DWORD dwFlags = 0;
		DWORD cbBuffer = 0;

		while(true) {
			HRESULT hr = mReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, 0, &dwFlags, NULL, &sample.p);
			if(FAILED(hr)) {
				Debug("ReadSample failed");
				return 0;
			}

			if(dwFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
				sample.Release();
				return cbBuffer;
			}

			sample->ConvertToContiguousBuffer(&mediaBuffer.p);
			BYTE* bytes = nullptr;
			DWORD size;
			mediaBuffer->Lock(&bytes, 0, &size);
			if(cbBuffer + size > (DWORD)maxBufferSize)
				size = maxBufferSize - cbBuffer;
			memcpy(&buffer[cbBuffer], bytes, size);
			mediaBuffer->Unlock();
			cbBuffer += size;
			sample.Release();
			mediaBuffer.Release();
		}
	}

	// AudioClip

	AudioClip::AudioClip()
	{
		bLoop = false;
		mSourceVoice = nullptr;
		mVolume = 0.5f;
		state = AudioClip::eStopped;
	}

	AudioClip::~AudioClip()
	{
		if(mSourceVoice) {
			mSourceVoice->Stop();
			mSourceVoice->FlushSourceBuffers();
			mSourceVoice->DestroyVoice();
			mSourceVoice = nullptr;
		}
	}

	void AudioClip::SetVolume(float volume)
	{
		if(mSourceVoice) {
			mVolume = clamp(volume, 0.0f, 1.0f);
			mSourceVoice->SetVolume(mVolume);
		}
	}

	float AudioClip::GetVolume()
	{
		return mVolume;
	}

	void AudioClip::Stop()
	{
		if(mSourceVoice) {
			mSourceVoice->Stop();
			mSourceVoice->FlushSourceBuffers();
			state = AudioClip::eStopped;
		}
	}

	void AudioClip::Play()
	{
		if(mSourceVoice) {
			mSourceVoice->SetVolume(mVolume);
			mSourceVoice->Start();
			state = AudioClip::ePlaying;
		}
	}
	void AudioClip::Pause()
	{
		if(mSourceVoice) {
			mSourceVoice->Stop();
			state = AudioClip::ePaused;
		}
	}

	///////////////////////////////////////////////////////////////////////

	void AudioMusic::Stream()
	{
		XAUDIO2_VOICE_STATE state;
		DWORD bufferLength;
		XAUDIO2_BUFFER buf = { 0 };
		bool endOfStream = false;

		if(!mSourceVoice)
			return;

		mSourceVoice->GetState(&state);
		while(state.BuffersQueued <= STREAMING_BUFFER_COUNT - 1) {
			mStreamer.GetNextBuffer(mAudioBuffers[mCurrentBuffer], STREAMING_BUFFER_SIZE, &bufferLength, &endOfStream);
			if(bufferLength > 0) {
				buf.AudioBytes = bufferLength;
				buf.pAudioData = mAudioBuffers[mCurrentBuffer];
				buf.Flags = endOfStream ? XAUDIO2_END_OF_STREAM : 0;

				mSourceVoice->SubmitSourceBuffer(&buf);
				mCurrentBuffer++;
				mCurrentBuffer %= STREAMING_BUFFER_COUNT;
			}
			if(endOfStream) {
				if(bLoop)
					mStreamer.ResetBuffer();
				break;
			}
			mSourceVoice->GetState(&state);
		}
	}

	AudioMusic::AudioMusic()
	{
		mCurrentBuffer = 0;
	}

	std::shared_ptr<AudioMusic> AudioMusic::CreateAudioClipFromFile(const std::wstring& filename)
	{
		std::shared_ptr<AudioMusic> clip = std::make_shared<AudioMusic>();
		clip->name = filename;
		if(!clip->mStreamer.LoadWAVFile(filename)) {
			clip = std::make_shared<AudioMusic>();
		}
		else
			clip->Initialize();
		return clip;
	}

	void AudioMusic::Initialize()
	{
		auto engine = AudioEngine::GetInstance()->GetEngine();
		engine->CreateSourceVoice(&mSourceVoice, mStreamer.mWaveFormat);
	}

	void AudioMusic::Stop()
	{
		if(mSourceVoice) {
			mSourceVoice->Stop();
			mSourceVoice->FlushSourceBuffers();
			ZeroMemory(&mAudioBuffers, STREAMING_BUFFER_COUNT * STREAMING_BUFFER_SIZE * sizeof(BYTE));
			mStreamer.ResetBuffer();
			mCurrentBuffer = 0;
			state = AudioClip::eStopped;
		}
	}
	/////////////////////////////////////////////////////////////////////////////

	std::shared_ptr<AudioSFX> AudioSFX::CreateAudioClipFromFile(const std::wstring& filename)
	{
		std::shared_ptr<AudioSFX> clip = std::make_shared<AudioSFX>();
		clip->name = filename;
		if(!clip->mStreamer.LoadWAVFile(filename)) {
			clip = std::make_shared<AudioSFX>();
		}
		else
			clip->Initialize();
		return clip;
	}

	void AudioSFX::Initialize()
	{
		auto engine = AudioEngine::GetInstance()->GetEngine();
		engine->CreateSourceVoice(&mSourceVoice, mStreamer.mWaveFormat);

		int len = (int)mStreamer.mLengthInBytes;

		mBuffer.reset(new BYTE[len], ArrayDeleter<BYTE>());
		size_t n = mStreamer.GetEntireBuffer(mBuffer.get(), len);
		mStreamer.mReader.Release();
	}

	void AudioSFX::Play()
	{
		if(mSourceVoice) {
			Stop();
			mSourceVoice->SetVolume(mVolume);
			XAUDIO2_BUFFER buffer = { 0 };
			buffer.AudioBytes = mStreamer.mLengthInBytes;
			buffer.pAudioData = mBuffer.get();
			buffer.Flags = XAUDIO2_END_OF_STREAM;
			mSourceVoice->SubmitSourceBuffer(&buffer);
			state = AudioClip::ePlaying;
			mSourceVoice->Start();
		}
	}

	std::shared_ptr<AudioSFX> AudioSFX::Clone()
	{
		auto sfx = std::make_shared<AudioSFX>();
		auto engine = AudioEngine::GetInstance()->GetEngine();
		engine->CreateSourceVoice(&sfx->mSourceVoice, mStreamer.mWaveFormat);
		sfx->mBuffer = mBuffer;
		sfx->mStreamer.mLengthInBytes = mStreamer.mLengthInBytes;
		sfx->mStreamer.mLengthInSeconds = mStreamer.mLengthInSeconds;
		return sfx;
	}

	AudioSFX::~AudioSFX()
	{
	}

	AudioSFX::AudioSFX()
	{
	}
}