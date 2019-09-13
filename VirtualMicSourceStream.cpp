/** @file

MODULE				: VirtualMic

FILE NAME			: VirtualMicSourceStream.cpp

DESCRIPTION			: VirtualMic source header

LICENSE: Software License Agreement (BSD License)

Copyright (c) 2014, CSIR
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the CSIR nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

===========================================================================
*/
#include "VirtualMicSourceStream.h"
#include "VirtualMic.h"

VirtualMicSourceStream::VirtualMicSourceStream(HRESULT *phr, VirtualMicSource *pParent, LPCWSTR pPinName) :
CSourceStream(NAME("CSIR VPP Virtual Mic"), phr, pParent, pPinName), m_pParent(pParent),
m_uiFillSize(0)
{
  // Set the default media type as 320x240x24@15
  GetMediaType(0, &m_mt);
}

VirtualMicSourceStream::~VirtualMicSourceStream()
{

}

HRESULT VirtualMicSourceStream::QueryInterface(REFIID riid, void **ppv)
{
  // Standard OLE stuff
  if (riid == _uuidof(IAMStreamConfig))
    *ppv = (IAMStreamConfig*)this;
  else if (riid == _uuidof(IKsPropertySet))
    *ppv = (IKsPropertySet*)this;
  else
    return CSourceStream::QueryInterface(riid, ppv);

  AddRef();
  return S_OK;
}

HRESULT VirtualMicSourceStream::FillBuffer(IMediaSample *pms)
{
  REFERENCE_TIME rtNow;
  unsigned uiTimePerFrame = UNITS;
  rtNow = m_rtLastTime;
  m_rtLastTime += uiTimePerFrame;
  pms->SetTime(&rtNow, &m_rtLastTime);
  pms->SetSyncPoint(TRUE);

  BYTE *pData;
  long lDataLen;
  pms->GetPointer(&pData);
  lDataLen = pms->GetSize();

#if 0
  // creates static noise
  for (int i = 0; i < lDataLen; ++i)
    pData[i] = rand();
#else
#if 0
  // creates beep
  int i = 0;
  static unsigned char val = 0;
  int repeat = 1;
  int inc = 10;
  while (i < lDataLen)
  {
    pData[i++] = val;
    if ((i % repeat) == 0)
    {
      val += inc;
      if (val == 0)
      {
        inc *= -1;
      }
    }
  }
#else
  int i = 0;
  static unsigned char value = 255;
  static unsigned char silence = 0;
  static unsigned value_count = 32000;
  static unsigned silence_count = 96000;
  static bool output_silence = true;
  static unsigned index = 0;
  static unsigned char val = 0;
  int repeat = 1;
  int inc = 1;
  static int currentBeep = 0;
  while (i < lDataLen)
  {
    if (output_silence)
    {
      if (index < silence_count)
      {
        pData[i++] = silence;
        index++;
      }
      else
      {
        index = 0;
        output_silence = false;
        val = 0;
      }
    }
    else
    {
      if (index < value_count)
      {
        pData[i++] = val;
        // if ((i % repeat) == 0)
        if ((index % repeat) == 0)
        {
          val += inc;
          if (val == 0)
          {
            inc *= -1;
          }
        }
        index++;
      }
      else
      {
        index = 0;
        output_silence = true;
      }
    }
  }

#endif
#endif
  return NOERROR;
} // FillBuffer

STDMETHODIMP VirtualMicSourceStream::Notify(IBaseFilter * pSender, Quality q)
{
  return E_NOTIMPL;
} // Notify

HRESULT VirtualMicSourceStream::SetMediaType(const CMediaType *pmt)
{
  DECLARE_PTR(WAVEFORMATEX, pWavForEx, pmt->Format());
  HRESULT hr = CSourceStream::SetMediaType(pmt);
  return hr;
}

HRESULT VirtualMicSourceStream::GetMediaType(int iPosition, CMediaType *pmt)
{
  if (iPosition < 0) return E_INVALIDARG;
  if (iPosition > 0) return VFW_S_NO_MORE_ITEMS;

  DECLARE_PTR(WAVEFORMATEX, pWavForEx, pmt->AllocFormatBuffer(sizeof(WAVEFORMATEX)));
  ZeroMemory(pWavForEx, sizeof(WAVEFORMATEX));

#if 0
  unsigned uiBitsPerSample(8);
  unsigned uiChannels(1);
  unsigned uiSamplePerSecond(8000);
#endif

  unsigned uiBitsPerSample(16);
  unsigned uiChannels(2);
  unsigned uiSamplePerSecond(44100);

  pWavForEx->wFormatTag = WAVE_FORMAT_PCM;
  pWavForEx->wBitsPerSample = uiBitsPerSample;
  pWavForEx->nChannels = uiChannels;
  pWavForEx->nSamplesPerSec = uiSamplePerSecond;
  pWavForEx->wBitsPerSample = uiBitsPerSample;
  pWavForEx->cbSize = 0; // no extra info at end of header

  // From MSDN: http://msdn.microsoft.com/en-us/library/ms713497(VS.85).aspx
  // Block alignment, in bytes. The block alignment is the minimum atomic unit of data for the wFormatTag format type. If wFormatTag is WAVE_FORMAT_PCM or WAVE_FORMAT_EXTENSIBLE, nBlockAlign must be equal to the product of nChannels and wBitsPerSample divided by 8 (bits per byte).
  pWavForEx->nBlockAlign = uiChannels * (uiBitsPerSample >> 3);
  pWavForEx->nAvgBytesPerSec = pWavForEx->nSamplesPerSec * pWavForEx->nBlockAlign;

  pmt->SetType(&MEDIATYPE_Audio);
  pmt->SetSubtype(&MEDIASUBTYPE_PCM);
  pmt->SetFormatType(&FORMAT_WaveFormatEx);
  pmt->SetTemporalCompression(FALSE);

  pmt->SetSampleSize(4);

  return NOERROR;

} // GetMediaType

HRESULT VirtualMicSourceStream::CheckMediaType(const CMediaType *pMediaType)
{
  WAVEFORMATEX *pWavForEx = (WAVEFORMATEX*)(pMediaType->Format());
  if (*pMediaType != m_mt)
    return E_INVALIDARG;
  return S_OK;
} // CheckMediaType

HRESULT VirtualMicSourceStream::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
  CAutoLock cAutoLock(m_pFilter->pStateLock());
  HRESULT hr = NOERROR;

  // Audio buffer size
  //Buffer size calculation for PCM
  WAVEFORMATEX* pWavForEx = (WAVEFORMATEX*)m_mt.Format();
  pProperties->cBuffers = 1;
  m_uiFillSize = pWavForEx->nSamplesPerSec * pWavForEx->nChannels * (pWavForEx->wBitsPerSample >> 3);
  pProperties->cbBuffer = m_uiFillSize;

  ALLOCATOR_PROPERTIES Actual;
  hr = pAlloc->SetProperties(pProperties, &Actual);

  if (FAILED(hr)) return hr;
  if (Actual.cbBuffer < pProperties->cbBuffer) return E_FAIL;

  return NOERROR;
} // DecideBufferSize

// Called when graph is run
HRESULT VirtualMicSourceStream::OnThreadCreate()
{
  m_rtLastTime = 0;
  return NOERROR;
} // OnThreadCreate

HRESULT STDMETHODCALLTYPE VirtualMicSourceStream::SetFormat(AM_MEDIA_TYPE *pmt)
{
  DECLARE_PTR(WAVEFORMATEX, pWavForEx, m_mt.pbFormat);
  m_mt = *pmt;
  IPin* pin;
  ConnectedTo(&pin);
  if (pin)
  {
    IFilterGraph *pGraph = m_pParent->GetGraph();
    pGraph->Reconnect(this);
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE VirtualMicSourceStream::GetFormat(AM_MEDIA_TYPE **ppmt)
{
  *ppmt = CreateMediaType(&m_mt);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE VirtualMicSourceStream::GetNumberOfCapabilities(int *piCount, int *piSize)
{
  *piCount = 1;
  *piSize = sizeof(AUDIO_STREAM_CONFIG_CAPS);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE VirtualMicSourceStream::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC)
{
  *pmt = CreateMediaType(&m_mt);
#if 0
  DECLARE_PTR(VIDEOINFOHEADER, pvi, (*pmt)->pbFormat);

  if (iIndex == 0) iIndex = 4;

  pvi->bmiHeader.biCompression = BI_RGB;
  pvi->bmiHeader.biBitCount = 24;
  pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pvi->bmiHeader.biWidth = 80 * iIndex;
  pvi->bmiHeader.biHeight = 60 * iIndex;
  pvi->bmiHeader.biPlanes = 1;
  pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
  pvi->bmiHeader.biClrImportant = 0;

  SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
  SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

  (*pmt)->majortype = MEDIATYPE_Video;
  (*pmt)->subtype = MEDIASUBTYPE_RGB24;
  (*pmt)->formattype = FORMAT_VideoInfo;
  (*pmt)->bTemporalCompression = FALSE;
  (*pmt)->bFixedSizeSamples = FALSE;
  (*pmt)->lSampleSize = pvi->bmiHeader.biSizeImage;
  (*pmt)->cbFormat = sizeof(VIDEOINFOHEADER);

  DECLARE_PTR(VIDEO_STREAM_CONFIG_CAPS, pvscc, pSCC);

  pvscc->guid = FORMAT_VideoInfo;
  pvscc->VideoStandard = AnalogVideo_None;
  pvscc->InputSize.cx = 640;
  pvscc->InputSize.cy = 480;
  pvscc->MinCroppingSize.cx = 80;
  pvscc->MinCroppingSize.cy = 60;
  pvscc->MaxCroppingSize.cx = 640;
  pvscc->MaxCroppingSize.cy = 480;
  pvscc->CropGranularityX = 80;
  pvscc->CropGranularityY = 60;
  pvscc->CropAlignX = 0;
  pvscc->CropAlignY = 0;

  pvscc->MinOutputSize.cx = 80;
  pvscc->MinOutputSize.cy = 60;
  pvscc->MaxOutputSize.cx = 640;
  pvscc->MaxOutputSize.cy = 480;
  pvscc->OutputGranularityX = 0;
  pvscc->OutputGranularityY = 0;
  pvscc->StretchTapsX = 0;
  pvscc->StretchTapsY = 0;
  pvscc->ShrinkTapsX = 0;
  pvscc->ShrinkTapsY = 0;
  pvscc->MinFrameInterval = 200000;   //50 fps
  pvscc->MaxFrameInterval = 50000000; // 0.2 fps
  pvscc->MinBitsPerSecond = (80 * 60 * 3 * 8) / 5;
  pvscc->MaxBitsPerSecond = 640 * 480 * 3 * 8 * 50;
#endif

  return S_OK;
}

HRESULT VirtualMicSourceStream::Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData,
  DWORD cbInstanceData, void *pPropData, DWORD cbPropData)
{// Set: Cannot set any properties.
  return E_NOTIMPL;
}

HRESULT VirtualMicSourceStream::Get(
  REFGUID guidPropSet,   // Which property set.
  DWORD dwPropID,        // Which property in that set.
  void *pInstanceData,   // Instance data (ignore).
  DWORD cbInstanceData,  // Size of the instance data (ignore).
  void *pPropData,       // Buffer to receive the property data.
  DWORD cbPropData,      // Size of the buffer.
  DWORD *pcbReturned     // Return the size of the property.
  )
{
  if (guidPropSet != AMPROPSETID_Pin)             return E_PROP_SET_UNSUPPORTED;
  if (dwPropID != AMPROPERTY_PIN_CATEGORY)        return E_PROP_ID_UNSUPPORTED;
  if (pPropData == NULL && pcbReturned == NULL)   return E_POINTER;

  if (pcbReturned) *pcbReturned = sizeof(GUID);
  if (pPropData == NULL)          return S_OK; // Caller just wants to know the size. 
  if (cbPropData < sizeof(GUID))  return E_UNEXPECTED;// The buffer is too small.

  *(GUID *)pPropData = PIN_CATEGORY_CAPTURE;
  return S_OK;
}

HRESULT VirtualMicSourceStream::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
  if (guidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
  if (dwPropID != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;
  // We support getting this property, but not setting it.
  if (pTypeSupport) *pTypeSupport = KSPROPERTY_SUPPORT_GET;
  return S_OK;
}
