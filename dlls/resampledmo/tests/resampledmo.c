/*
 * Copyright 2019 Zebediah Figura
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#define COBJMACROS
#include "objbase.h"
#include "rpcproxy.h"
#include "wmcodecdsp.h"
#include "mftransform.h"
#include "wine/test.h"

#define check_interface(a, b, c) check_interface_(__LINE__, a, b, c)
static void check_interface_(unsigned int line, void *iface_ptr, REFIID iid, BOOL supported)
{
    IUnknown *iface = iface_ptr;
    HRESULT hr, expected_hr;
    IUnknown *unk;

    expected_hr = supported ? S_OK : E_NOINTERFACE;

    hr = IUnknown_QueryInterface(iface, iid, (void **)&unk);
    ok_(__FILE__, line)(hr == expected_hr, "Got hr %#x, expected %#x.\n", hr, expected_hr);
    if (SUCCEEDED(hr))
        IUnknown_Release(unk);
}

static void test_interfaces(void)
{
    IUnknown *dmo;
    HRESULT hr;
    ULONG ref;

    hr = CoCreateInstance(&CLSID_CResamplerMediaObject, NULL,
            CLSCTX_INPROC_SERVER, &IID_IUnknown, (void **)&dmo);
    ok(hr == S_OK, "Got hr %#x.\n", hr);

    check_interface(dmo, &IID_IMFTransform, TRUE);
    check_interface(dmo, &IID_IUnknown, TRUE);
    check_interface(dmo, &IID_IWMResamplerProps, TRUE);

    ref = IUnknown_Release(dmo);
    ok(!ref, "Got outstanding refcount %d.\n", ref);
}

static ULONG get_refcount(void *iface)
{
    IUnknown *unknown = iface;
    IUnknown_AddRef(unknown);
    return IUnknown_Release(unknown);
}

static const GUID test_iid = {0x33333333};
static LONG outer_ref = 1;

static HRESULT WINAPI outer_QueryInterface(IUnknown *iface, REFIID iid, void **out)
{
    if (IsEqualGUID(iid, &IID_IUnknown)
            || IsEqualGUID(iid, &IID_IWMResamplerProps)
            || IsEqualGUID(iid, &test_iid))
    {
        *out = (IUnknown *)0xdeadbeef;
        return S_OK;
    }
    ok(0, "unexpected call %s\n", wine_dbgstr_guid(iid));
    return E_NOINTERFACE;
}

static ULONG WINAPI outer_AddRef(IUnknown *iface)
{
    return InterlockedIncrement(&outer_ref);
}

static ULONG WINAPI outer_Release(IUnknown *iface)
{
    return InterlockedDecrement(&outer_ref);
}

static const IUnknownVtbl outer_vtbl =
{
    outer_QueryInterface,
    outer_AddRef,
    outer_Release,
};

static IUnknown test_outer = {&outer_vtbl};

static void test_aggregation(void)
{
    IWMResamplerProps *props, *props2;
    IUnknown *unk, *unk2;
    HRESULT hr;
    ULONG ref;

    props = (IWMResamplerProps *)0xdeadbeef;
    hr = CoCreateInstance(&CLSID_CResamplerMediaObject, &test_outer,
            CLSCTX_INPROC_SERVER, &IID_IWMResamplerProps, (void **)&props);
    ok(hr == E_NOINTERFACE, "Got hr %#x.\n", hr);
    ok(!props, "Got interface %p.\n", props);

    hr = CoCreateInstance(&CLSID_CResamplerMediaObject, &test_outer,
            CLSCTX_INPROC_SERVER, &IID_IUnknown, (void **)&unk);
    ok(hr == S_OK, "Got hr %#x.\n", hr);
    ok(outer_ref == 1, "Got unexpected refcount %d.\n", outer_ref);
    ok(unk != &test_outer, "Returned IUnknown should not be outer IUnknown.\n");
    ref = get_refcount(unk);
    ok(ref == 1, "Got unexpected refcount %d.\n", ref);

    ref = IUnknown_AddRef(unk);
    ok(ref == 2, "Got unexpected refcount %d.\n", ref);
    ok(outer_ref == 1, "Got unexpected refcount %d.\n", outer_ref);

    ref = IUnknown_Release(unk);
    ok(ref == 1, "Got unexpected refcount %d.\n", ref);
    ok(outer_ref == 1, "Got unexpected refcount %d.\n", outer_ref);

    hr = IUnknown_QueryInterface(unk, &IID_IUnknown, (void **)&unk2);
    ok(hr == S_OK, "Got hr %#x.\n", hr);
    ok(unk2 == unk, "Got unexpected IUnknown %p.\n", unk2);
    IUnknown_Release(unk2);

    hr = IUnknown_QueryInterface(unk, &IID_IWMResamplerProps, (void **)&props);
    ok(hr == S_OK, "Got hr %#x.\n", hr);

    hr = IWMResamplerProps_QueryInterface(props, &IID_IUnknown, (void **)&unk2);
    ok(hr == S_OK, "Got hr %#x.\n", hr);
    ok(unk2 == (IUnknown *)0xdeadbeef, "Got unexpected IUnknown %p.\n", unk2);

    hr = IWMResamplerProps_QueryInterface(props, &IID_IWMResamplerProps, (void **)&props2);
    ok(hr == S_OK, "Got hr %#x.\n", hr);
    ok(props2 == (IWMResamplerProps *)0xdeadbeef, "Got unexpected IWMResamplerProps %p.\n", props2);

    hr = IUnknown_QueryInterface(unk, &test_iid, (void **)&unk2);
    ok(hr == E_NOINTERFACE, "Got hr %#x.\n", hr);
    ok(!unk2, "Got unexpected IUnknown %p.\n", unk2);

    hr = IWMResamplerProps_QueryInterface(props, &test_iid, (void **)&unk2);
    ok(hr == S_OK, "Got hr %#x.\n", hr);
    ok(unk2 == (IUnknown *)0xdeadbeef, "Got unexpected IUnknown %p.\n", unk2);

    IWMResamplerProps_Release(props);
    ref = IUnknown_Release(unk);
    ok(!ref, "Got unexpected refcount %d.\n", ref);
    ok(outer_ref == 1, "Got unexpected refcount %d.\n", outer_ref);
}

START_TEST(resampledmo)
{
    IUnknown *dmo;
    HRESULT hr;

    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    hr = CoCreateInstance(&CLSID_CResamplerMediaObject, NULL,
            CLSCTX_INPROC_SERVER, &IID_IUnknown, (void **)&dmo);
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip("Failed to create audio resampler DMO.\n");
        return;
    }
    IUnknown_Release(dmo);

    test_interfaces();
    test_aggregation();

    CoUninitialize();
}
