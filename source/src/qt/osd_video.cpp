/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2015.11.26-

	[ win32 DirectShow ]
*/

#include "../emu.h"

//#if defined(USE_MOVIE_PLAYER) || defined(USE_VIDEO_CAPTURE)
void OSD_BASE::initialize_video()
{
#if 0
	pBasicAudio = NULL;
	pBasicVideo = NULL;
	pVideoWindow = NULL;
	pMediaPosition = NULL;
	pMediaSeeking = NULL;
	pMediaControl = NULL;
	pSoundCallBack = NULL;
	pSoundSampleGrabber = NULL;
	pSoundBaseFilter = NULL;
	pVideoSampleGrabber = NULL;
	pCaptureGraphBuilder2 = NULL;
	pCaptureBaseFilter = NULL;
	pVideoBaseFilter = NULL;
	pGraphBuilder = NULL;
	
	memset(&dshow_screen_buffer, 0, sizeof(screen_buffer_t));
	
	direct_show_mute[0] = direct_show_mute[1] = true;

	now_movie_play = now_movie_pause = false;
	if(get_use_video_capture()) enum_capture_devs();
	cur_capture_dev_index = -1;
#endif
}

#define SAFE_RELEASE(x) { \
	if(x != NULL) { \
		(x)->Release(); \
		(x) = NULL; \
	} \
}

void OSD_BASE::release_video()
{
#if 0
	if(pMediaControl != NULL) {
		pMediaControl->Stop();
	}
	SAFE_RELEASE(pBasicAudio);
	SAFE_RELEASE(pBasicVideo);
	SAFE_RELEASE(pVideoWindow);
	SAFE_RELEASE(pMediaPosition);
	SAFE_RELEASE(pMediaSeeking);
	SAFE_RELEASE(pMediaControl);
	SAFE_RELEASE(pSoundCallBack);
	SAFE_RELEASE(pSoundSampleGrabber);
	SAFE_RELEASE(pSoundBaseFilter);
	SAFE_RELEASE(pVideoSampleGrabber);
	SAFE_RELEASE(pCaptureGraphBuilder2);
	SAFE_RELEASE(pCaptureBaseFilter);
	SAFE_RELEASE(pVideoBaseFilter);
	SAFE_RELEASE(pGraphBuilder);
	
	release_screen_buffer(&dshow_screen_buffer);
#endif	
}

void OSD_BASE::get_video_buffer()
{
#if 0
	if(pVideoSampleGrabber != NULL) {
#if defined(_RGB555) || defined(_RGB565)
		long buffer_size = direct_show_width * direct_show_height * 2;
#elif defined(_RGB888)
		long buffer_size = direct_show_width * direct_show_height * 4;
#endif
		pVideoSampleGrabber->GetCurrentBuffer(&buffer_size, (long *)dshow_screen_buffer.lpBmp);
		if(vm_screen_width == direct_show_width && vm_screen_height == direct_show_height) {
			if(bVerticalReversed) {
				BitBlt(vm_screen_buffer.hdcDib, 0, vm_screen_height, vm_screen_width, -vm_screen_height, dshow_screen_buffer.hdcDib, 0, 0, SRCCOPY);
			} else {
				BitBlt(vm_screen_buffer.hdcDib, 0, 0, vm_screen_width, vm_screen_height, dshow_screen_buffer.hdcDib, 0, 0, SRCCOPY);
			}
		} else {
			if(bVerticalReversed) {
				StretchBlt(vm_screen_buffer.hdcDib, 0, vm_screen_height, vm_screen_width, -vm_screen_height, dshow_screen_buffer.hdcDib, 0, 0, direct_show_width, direct_show_height, SRCCOPY);
			} else {
				StretchBlt(vm_screen_buffer.hdcDib, 0, 0, vm_screen_width, vm_screen_height, dshow_screen_buffer.hdcDib, 0, 0, direct_show_width, direct_show_height, SRCCOPY);
			}
		}
	} else {
		memset(vm_screen_buffer.lpBmp, 0, vm_screen_width * vm_screen_height * sizeof(scrntype));
	}
#endif	
}

void OSD_BASE::mute_video_dev(bool l, bool r)
{
#if 0	
	if(pBasicAudio != NULL) {
		if(l && r) {
			pBasicAudio->put_Volume(-10000L);
		} else {
			pBasicAudio->put_Volume(0L);
		}
		if(l && !r) {
			pBasicAudio->put_Balance(1000L);
		} else if(!l && r) {
			pBasicAudio->put_Balance(-1000L);
		} else {
			pBasicAudio->put_Balance(0L);
		}
		direct_show_mute[0] = l;
		direct_show_mute[1] = r;
	}
#endif	
}
//#endif // #if defined(USE_MOVIE_PLAYER) || defined(USE_VIDEO_CAPTURE)


//#ifdef USE_MOVIE_PLAYER
bool OSD_BASE::open_movie_file(const _TCHAR* file_path)
{
#if 0	
	WCHAR wFile[_MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, file_path, -1, wFile, _MAX_PATH);
	
	AM_MEDIA_TYPE video_mt;
	ZeroMemory(&video_mt, sizeof(AM_MEDIA_TYPE));
	video_mt.majortype = MEDIATYPE_Video;
#if defined(_RGB555)
	video_mt.subtype = MEDIASUBTYPE_RGB555;
#elif defined(_RGB565)
	video_mt.subtype = MEDIASUBTYPE_RGB565;
#elif defined(_RGB888)
	video_mt.subtype = MEDIASUBTYPE_RGB32;
#endif
	video_mt.formattype = FORMAT_VideoInfo;
	
	AM_MEDIA_TYPE sound_mt;
	ZeroMemory(&sound_mt, sizeof(AM_MEDIA_TYPE));
	sound_mt.majortype = MEDIATYPE_Audio;
	sound_mt.subtype = MEDIASUBTYPE_PCM;
	
	if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&pGraphBuilder))) {
		return false;
	}
	if(FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pVideoBaseFilter))) {
		return false;
	}
	if(FAILED(pVideoBaseFilter->QueryInterface(IID_ISampleGrabber, (void **)&pVideoSampleGrabber))) {
		return false;
	}
	if(FAILED(pVideoSampleGrabber->SetMediaType(&video_mt))) {
		return false;
	}
	if(FAILED(pGraphBuilder->AddFilter(pVideoBaseFilter, L"Video Sample Grabber"))) {
		return false;
	}
	if(FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pSoundBaseFilter))) {
		return false;
	}
	if(FAILED(pSoundBaseFilter->QueryInterface(IID_ISampleGrabber, (void **)&pSoundSampleGrabber))) {
		return false;
	}
	if(FAILED(pSoundSampleGrabber->SetMediaType(&sound_mt))) {
		return false;
	}
	if((pSoundCallBack = new CMySampleGrabberCB(vm)) == NULL) {
		return false;
	}
	if(FAILED(pSoundSampleGrabber->SetCallback(pSoundCallBack, 1))) {
		return false;
	}
	if(FAILED(pGraphBuilder->AddFilter(pSoundBaseFilter, L"Sound Sample Grabber"))) {
		return false;
	}
	if(FAILED(pGraphBuilder->RenderFile(wFile, NULL))) {
		return false;
	}
	if(FAILED(pVideoSampleGrabber->SetBufferSamples(TRUE))) {
		return false;
	}
	if(FAILED(pVideoSampleGrabber->SetOneShot(FALSE))) {
		return false;
	}
	if(FAILED(pSoundSampleGrabber->SetBufferSamples(FALSE))) {
		return false;
	}
	if(FAILED(pSoundSampleGrabber->SetOneShot(FALSE))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&pMediaControl))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IMediaSeeking, (void **)&pMediaSeeking))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IMediaPosition, (void **)&pMediaPosition))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&pVideoWindow))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IBasicVideo, (void **)&pBasicVideo))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&pBasicAudio))) {
		return false;
	}
	if(FAILED(pVideoWindow->put_Owner((OAHWND)main_window_handle))) {
		return false;
	}
	if(FAILED(pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN))) {
		return false;
	}
	if(FAILED(pVideoWindow->SetWindowPosition(0, 0, 0, 0))) {
		return false;
	}
	if(FAILED(pVideoWindow->SetWindowForeground(FALSE))) {
		return false;
	}
	if(pMediaSeeking->IsFormatSupported(&TIME_FORMAT_FRAME) == S_OK) {
		if(FAILED(pMediaSeeking->SetTimeFormat(&TIME_FORMAT_FRAME))) {
			return false;
		}
		bTimeFormatFrame = true;
	} else {
		if(FAILED(pMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME))) {
			return false;
		}
		bTimeFormatFrame = false;
	}
	
	// get the movie frame rate
	if(FAILED(pBasicVideo->get_AvgTimePerFrame(&movie_frame_rate))) {
		return false;
	}
	movie_frame_rate = 1 / movie_frame_rate;
	
	// get the movie sound rate
	pSoundSampleGrabber->GetConnectedMediaType(&sound_mt);
	WAVEFORMATEX *wf = (WAVEFORMATEX *)sound_mt.pbFormat;
	WAVEFORMATEXTENSIBLE *wfe = (WAVEFORMATEXTENSIBLE *)sound_mt.pbFormat;
	if(!((wf->wFormatTag == WAVE_FORMAT_PCM || (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE && wfe->SubFormat == MEDIASUBTYPE_PCM)) && wf->nChannels == 2 && wf->wBitsPerSample == 16)) {
		return false;
	}
	movie_sound_rate = wf->nSamplesPerSec;
	
	// get the movie picture size
	pVideoSampleGrabber->GetConnectedMediaType(&video_mt);
	VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)video_mt.pbFormat;
	direct_show_width = pVideoHeader->bmiHeader.biWidth;
	direct_show_height = pVideoHeader->bmiHeader.biHeight;
	
	bVerticalReversed = check_file_extension(file_path, _T(".ogv"));
	
	if(dshow_screen_buffer.width != direct_show_width || dshow_screen_buffer.height != direct_show_height) {
		initialize_screen_buffer(&dshow_screen_buffer, direct_show_width, direct_show_height, COLORONCOLOR);
	}
#endif	
	return true;
}

void OSD_BASE::close_movie_file()
{
	release_video();
	now_movie_play = false;
	now_movie_pause = false;
}

void OSD_BASE::play_movie()
{
#if 0
	if(pMediaControl != NULL) {
		pMediaControl->Run();
		mute_video_dev(direct_show_mute[0], direct_show_mute[1]);
		now_movie_play = true;
		now_movie_pause = false;
	}
#else
	now_movie_play = true;
	now_movie_pause = false;
#endif	
}

void OSD_BASE::stop_movie()
{
#if 0
	if(pMediaControl != NULL) {
		pMediaControl->Stop();
	}
#endif	
	now_movie_play = false;
	now_movie_pause = false;
}

void OSD_BASE::pause_movie()
{
#if 0
	if(pMediaControl != NULL) {
		pMediaControl->Pause();
		now_movie_pause = true;
	}
#else
	now_movie_pause = true;
#endif
}

void OSD_BASE::set_cur_movie_frame(int frame, bool relative)
{
#if 0	
	if(pMediaSeeking != NULL) {
		LONGLONG now = bTimeFormatFrame ? frame : (LONGLONG)(frame / movie_frame_rate * 10000000.0 + 0.5);
		pMediaSeeking->SetPositions(&now, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	}
#endif	
}

uint32_t OSD_BASE::get_cur_movie_frame()
{
#if 0	
	if(pMediaSeeking != NULL) {
		LONGLONG now, stop;
		if(SUCCEEDED(pMediaSeeking->GetPositions(&now, &stop))) {
			if(bTimeFormatFrame) {
				return (uint32)(now & 0xffffffff);
			} else {
				return (uint32)(now * movie_frame_rate / 10000000.0 + 0.5);
			}
		}
	}
#endif	
	return 0;
}
//#endif

//#ifdef USE_VIDEO_CAPTURE
void OSD_BASE::enum_capture_devs()
{
#if 0	
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;
	
	num_capture_devs = 0;
	
	// create the system device enum
	if(SUCCEEDED(CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pDevEnum))) {
		// create the video input device enu,
		if(SUCCEEDED(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0)) && pClassEnum != NULL) {
			ULONG cFetched;
			IMoniker *pMoniker = NULL;
			
			while(num_capture_devs < MAX_CAPTURE_DEVS && SUCCEEDED(pClassEnum->Next(1, &pMoniker, &cFetched)) && pMoniker != NULL) {
				IPropertyBag *pBag = NULL;
				if(SUCCEEDED(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag))) {
					VARIANT var;
					var.vt = VT_BSTR;
					if(pBag->Read(L"FriendlyName", &var, NULL) == NOERROR) {
						LPCWSTR _lpw = NULL;
						int _convert = 0;
						_tcscpy_s(capture_dev_name[num_capture_devs++], 256, MyW2T(var.bstrVal));
						SysFreeString(var.bstrVal);
						pMoniker->AddRef();
					}
					SAFE_RELEASE(pBag);
				}
				SAFE_RELEASE(pMoniker);
			}
		}
	}
	SAFE_RELEASE(pClassEnum);
	SAFE_RELEASE(pDevEnum);
#endif	
}

bool OSD_BASE::connect_capture_dev(int index, bool pin)
{
	if(cur_capture_dev_index == index && !pin) {
		return true;
	}
	if(cur_capture_dev_index != -1) {
		release_video();
		cur_capture_dev_index = -1;
	}
#if 0	
	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
#if defined(_RGB555)
	mt.subtype = MEDIASUBTYPE_RGB555;
#elif defined(_RGB565)
	mt.subtype = MEDIASUBTYPE_RGB565;
#elif defined(_RGB888)
	mt.subtype = MEDIASUBTYPE_RGB32;
#endif
	mt.formattype = FORMAT_VideoInfo;
	
	if(FAILED(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&pGraphBuilder))) {
		return false;
	}
	
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;
	
	if(SUCCEEDED(CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void **)&pDevEnum))) {
		if(SUCCEEDED(pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0)) && pClassEnum != NULL) {
			for(int i = 0; i <= index; i++) {
				IMoniker *pMoniker = NULL;
				ULONG cFetched;
				
				if(SUCCEEDED(pClassEnum->Next(1, &pMoniker, &cFetched)) && pMoniker != NULL) {
					if(i == index) {
						pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pCaptureBaseFilter);
					}
					SAFE_RELEASE(pMoniker);
				} else {
					break;
				}
			}
		}
	}
	SAFE_RELEASE(pClassEnum);
	SAFE_RELEASE(pDevEnum);
	
	if(&pCaptureBaseFilter == NULL) {
		return false;
	}
	if(FAILED(pGraphBuilder->AddFilter(pCaptureBaseFilter, L"Video Capture"))) {
		return false;
	}
	if(FAILED(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&pCaptureGraphBuilder2))) {
		return false;
	}
	if(FAILED(pCaptureGraphBuilder2->SetFiltergraph(pGraphBuilder))) {
		return false;
	}
	
	IAMStreamConfig *pSC = NULL;
	ISpecifyPropertyPages *pSpec = NULL;
	
	if(FAILED(pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pCaptureBaseFilter, IID_IAMStreamConfig, (void **)&pSC))) {
		if(FAILED(pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCaptureBaseFilter, IID_IAMStreamConfig, (void **)&pSC))) {
			return false;
		}
	}
	if(SUCCEEDED(pSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec))) {
		CAUUID cauuid;
		pSpec->GetPages(&cauuid);
		if(pin) {
			OleCreatePropertyFrame(NULL, 30, 30, NULL, 1, (IUnknown **)&pSC, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
			CoTaskMemFree(cauuid.pElems);
		}
		SAFE_RELEASE(pSpec);
	}
	SAFE_RELEASE(pSC);
	
	if(FAILED(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void **)&pVideoBaseFilter))) {
		return false;
	}
	if(FAILED(pVideoBaseFilter->QueryInterface(IID_ISampleGrabber, (void **)&pVideoSampleGrabber))) {
		return false;
	}
	if(FAILED(pVideoSampleGrabber->SetMediaType(&mt))) {
		return false;
	}
	if(FAILED(pGraphBuilder->AddFilter(pVideoBaseFilter, L"Video Sample Grabber"))) {
		return false;
	}
	if(FAILED(pGraphBuilder->Connect(get_pin(pCaptureBaseFilter, PINDIR_OUTPUT), get_pin(pVideoBaseFilter, PINDIR_INPUT)))) {
		return false;
	}
	if(FAILED(pVideoSampleGrabber->SetBufferSamples(TRUE))) {
		return false;
	}
	if(FAILED(pVideoSampleGrabber->SetOneShot(FALSE))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IMediaControl, (void **)&pMediaControl))) {
		return false;
	}
	if(FAILED(pGraphBuilder->QueryInterface(IID_IBasicAudio, (void **)&pBasicAudio))) {
		//return false;
	}
	if(FAILED(pMediaControl->Run())) {
		return false;
	}
	
	// get the capture size
	pVideoSampleGrabber->GetConnectedMediaType(&mt);
	VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)mt.pbFormat;
	direct_show_width = pVideoHeader->bmiHeader.biWidth;
	direct_show_height = pVideoHeader->bmiHeader.biHeight;
	
	if(dshow_screen_buffer.width != direct_show_width || dshow_screen_buffer.height != direct_show_height) {
		initialize_screen_buffer(&dshow_screen_buffer, direct_show_width, direct_show_height, COLORONCOLOR);
	}
#endif	
	cur_capture_dev_index = index;
	return true;
}

void OSD_BASE::open_capture_dev(int index, bool pin)
{
	if(!connect_capture_dev(index, pin)) {
		release_video();
	}
}

void OSD_BASE::close_capture_dev()
{
	release_video();
	cur_capture_dev_index = -1;
}

void OSD_BASE::show_capture_dev_filter()
{
#if 0	
	if(pCaptureBaseFilter != NULL) {
		ISpecifyPropertyPages *pSpec = NULL;
		CAUUID cauuid;
		if(SUCCEEDED(pCaptureBaseFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec))) {
			pSpec->GetPages(&cauuid);
			OleCreatePropertyFrame(NULL, 30, 30, NULL, 1, (IUnknown **)&pCaptureBaseFilter, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
			CoTaskMemFree(cauuid.pElems);
			SAFE_RELEASE(pSpec);
		}
	}
#endif	
}

void OSD_BASE::show_capture_dev_pin()
{
#if 0	
	if(cur_capture_dev_index != -1) {
		if(!connect_capture_dev(cur_capture_dev_index, true)) {
			release_video();
		}
	}
#endif
}

void OSD_BASE::show_capture_dev_source()
{
#if 0
	if(pCaptureGraphBuilder2 != NULL) {
		IAMCrossbar *pCrs = NULL;
		ISpecifyPropertyPages *pSpec = NULL;
		CAUUID cauuid;
		
		if(FAILED(pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Interleaved, pCaptureBaseFilter, IID_IAMCrossbar, (void **)&pCrs))) {
			if(FAILED(pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCaptureBaseFilter, IID_IAMCrossbar, (void **)&pCrs))) {
				return;
			}
		}
		if(SUCCEEDED(pCrs->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec))) {
			pSpec->GetPages(&cauuid);
			OleCreatePropertyFrame(NULL, 30, 30, NULL, 1, (IUnknown **)&pCrs, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
			CoTaskMemFree(cauuid.pElems);
			SAFE_RELEASE(pSpec);
			
			AM_MEDIA_TYPE mt;
			pVideoSampleGrabber->GetConnectedMediaType(&mt);
			VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)mt.pbFormat;
			direct_show_width = pVideoHeader->bmiHeader.biWidth;
			direct_show_height = pVideoHeader->bmiHeader.biHeight;
			
			if(dshow_screen_buffer.width != direct_show_width || dshow_screen_buffer.height != direct_show_height) {
				initialize_screen_buffer(&dshow_screen_buffer, direct_show_width, direct_show_height, COLORONCOLOR);
			}
		}
		SAFE_RELEASE(pCrs);
	}
#endif	
}

void OSD_BASE::set_capture_dev_channel(int ch)
{
#if 0	
	IAMTVTuner *pTuner = NULL;
	
	if(pCaptureGraphBuilder2 != NULL) {
		if(SUCCEEDED(pCaptureGraphBuilder2->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, pCaptureBaseFilter, IID_IAMTVTuner, (void **)&pTuner))) {
			pTuner->put_Channel(ch, AMTUNER_SUBCHAN_DEFAULT, AMTUNER_SUBCHAN_DEFAULT);
			SAFE_RELEASE(pTuner);
		}
	}
#endif	
}

double OSD_BASE::get_movie_frame_rate()
{
	return movie_frame_rate;
}

int OSD_BASE::get_movie_sound_rate()
{
	return movie_sound_rate;
}

_TCHAR* OSD_BASE::get_capture_dev_name(int index)
{
	return capture_dev_name[index];
}
