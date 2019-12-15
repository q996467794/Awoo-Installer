/*
Copyright (c) 2017-2018 Adubbz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "nx/ipc/es.h"

#include <string.h>

#include <switch.h>
#include "service_guard.h"

static Service g_esSrv;

NX_GENERATE_SERVICE_GUARD(es);

Result _esInitialize() {
    return smGetService(&g_esSrv, "es");
}

void _esCleanup() {
    serviceClose(&g_esSrv);
}

Service* esGetServiceSession() {
    return &g_esSrv;
}

Result esImportTicket(void const *tikBuf, size_t tikSize, void const *certBuf, size_t certSize) {
    return serviceDispatch(&g_esSrv, 1,
        .buffer_attrs = {
            SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
        },
        .buffers = {
            { tikBuf,   tikSize },
            { certBuf,  certSize },
        },
    );
}

Result esDeleteTicket(const RightsId *rightsIdBuf, size_t bufSize) {
    return serviceDispatch(&g_esSrv, 3,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
        .buffers = { { rightsIdBuf, bufSize }, },
    );
}

Result esGetTitleKey(const RightsId *rightsId, u8 *outBuf, size_t bufSize) {
    struct {
        RightsId rights_Id;
        u32 key_generation;
    } in = { rightsId, 0 };

    return serviceDispatchIn(&g_esSrv, 8, in,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { outBuf, bufSize } },
    );
}

Result esCountCommonTicket(u32 *numTickets) {
    return serviceDispatchOut(&g_esSrv, 9, *numTickets);
}

Result esCountPersonalizedTicket(u32 *numTickets) {
    return serviceDispatchOut(&g_esSrv, 10, *numTickets);
}

Result esListCommonTicket(u32 *numRightsIdsWritten, RightsId *outBuf, size_t bufSize) {
    return serviceDispatchOut(&g_esSrv, 11, *numRightsIdsWritten,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { outBuf, bufSize } },
    );
}

Result esListPersonalizedTicket(u32 *numRightsIdsWritten, RightsId *outBuf, size_t bufSize) {
    return serviceDispatchOut(&g_esSrv, 12, *numRightsIdsWritten,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { outBuf, bufSize } },
    );
}

Result esGetCommonTicketData(u64 *unkOut, void *outBuf1, size_t bufSize1, const RightsId* rightsId) {
    Result rc = serviceDispatchInOut(&g_esSrv, 16, *rightsId, *unkOut,
        .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
        .buffers = { { outBuf1, bufSize1 } },
    );
    return rc;
}