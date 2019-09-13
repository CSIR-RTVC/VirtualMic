/** @file

MODULE				: VirtualMic

FILE NAME			: VirtualMic.h

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

// {D180F1D5-7E2E-4f45-A7C8-CACFD17178CD}
static const GUID CLSID_VPP_VirtualMic =
{ 0xd180f1d5, 0x7e2e, 0x4f45, { 0xa7, 0xc8, 0xca, 0xcf, 0xd1, 0x71, 0x78, 0xcd } };

#define DECLARE_PTR(type, ptr, expr) type* ptr = (type*)(expr);

/// fwd
class VirtualMicSourceStream;

/**
 * @brief VirtualMicSource is a virtual audio capture filter.
 * The current implementation just emits a constant beep.
 * @TODO: add metronome-like audio
 */
class VirtualMicSource : public CSource
{
public:

  /**
   * @brief
   */
  static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
  /**
   * @brief
   */
  STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
  /**
   * @brief
   */
  IFilterGraph *GetGraph() { return m_pGraph; }

private:
  /**
   * @brief
   */
  VirtualMicSource(LPUNKNOWN lpunk, HRESULT *phr);
};

