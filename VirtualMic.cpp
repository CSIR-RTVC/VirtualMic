/** @file

MODULE				: VirtualMic

FILE NAME			: VirtualMic.cpp

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
//#pragma warning(disable:4244)
//#pragma warning(disable:4711)
#include "VirtualMic.h"
#include "VirtualMicSourceStream.h"
#include <stdio.h>
#include <olectl.h>
#include <dvdmedia.h>

CUnknown * WINAPI VirtualMicSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
  ASSERT(phr);
  CUnknown *punk = new VirtualMicSource(lpunk, phr);
  return punk;
}

VirtualMicSource::VirtualMicSource(LPUNKNOWN lpunk, HRESULT *phr) :
CSource(NAME("CSIR VPP Virtual Mic"), lpunk, CLSID_VPP_VirtualMic)
{
  ASSERT(phr);
  CAutoLock cAutoLock(&m_cStateLock);
  // Create the one and only output pin
  m_paStreams = (CSourceStream **) new VirtualMicSourceStream*[1];
  m_paStreams[0] = new VirtualMicSourceStream(phr, this, L"CSIR VPP Virtual Mic");
}

HRESULT VirtualMicSource::QueryInterface(REFIID riid, void **ppv)
{
  //Forward request for IAMStreamConfig & IKsPropertySet to the pin
  if (riid == _uuidof(IAMStreamConfig) || riid == _uuidof(IKsPropertySet))
    return m_paStreams[0]->QueryInterface(riid, ppv);
  else
    return CSource::QueryInterface(riid, ppv);
}
