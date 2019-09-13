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
#pragma once

#pragma warning(push)     // disable for this header only
#pragma warning(disable:4312)
// DirectShow
#include <streams.h>
#pragma warning(pop)      // restore original warning level

/// fwd
class VirtualMicSource;

class VirtualMicSourceStream : public CSourceStream, public IAMStreamConfig, public IKsPropertySet
{
public:
  /**
   * @brief IUnknown
   */
  STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
  /**
   * @brief IUnknown
   */
  STDMETHODIMP_(ULONG) AddRef() { return GetOwner()->AddRef(); }                                                          \
  /**
   * @brief IUnknown
   */
    STDMETHODIMP_(ULONG) Release() { return GetOwner()->Release(); }
  /**
   * @brief IQualityControl
   */
  STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
  /**
   * @brief IAMStreamConfig
   */
  HRESULT STDMETHODCALLTYPE SetFormat(AM_MEDIA_TYPE *pmt);
  /**
   * @brief IAMStreamConfig
   */
  HRESULT STDMETHODCALLTYPE GetFormat(AM_MEDIA_TYPE **ppmt);
  /**
   * @brief IAMStreamConfig
   */
  HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(int *piCount, int *piSize);
  /**
   * @brief IAMStreamConfig
   */
  HRESULT STDMETHODCALLTYPE GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC);
  /**
   * @brief IKsPropertySet
   */
  HRESULT STDMETHODCALLTYPE Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData);
  /**
   * @brief IKsPropertySet
   */
  HRESULT STDMETHODCALLTYPE Get(REFGUID guidPropSet, DWORD dwPropID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData, DWORD *pcbReturned);
  /**
   * @brief IKsPropertySet
   */
  HRESULT STDMETHODCALLTYPE QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport);
  /**
   * @brief CSourceStream
   */
  VirtualMicSourceStream(HRESULT *phr, VirtualMicSource *pParent, LPCWSTR pPinName);
  /**
   * @brief Destructor
   */
  ~VirtualMicSourceStream();
  /**
   * @brief CSourceStream
   */
  HRESULT FillBuffer(IMediaSample *pms);
  /**
   * @brief CSourceStream
   */
  HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties);
  /**
   * @brief CSourceStream
   */
  HRESULT CheckMediaType(const CMediaType *pMediaType);
  /**
   * @brief CSourceStream
   */
  HRESULT GetMediaType(int iPosition, CMediaType *pmt);
  /**
   * @brief CSourceStream
   */
  HRESULT SetMediaType(const CMediaType *pmt);
  /**
   * @brief CSourceStream
   */
  HRESULT OnThreadCreate(void);

private:
  VirtualMicSource *m_pParent;
  REFERENCE_TIME m_rtLastTime;
  CCritSec m_cSharedState;
  IReferenceClock *m_pClock;
  unsigned m_uiFillSize;
};
