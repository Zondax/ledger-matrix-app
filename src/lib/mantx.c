/*******************************************************************************
*   (c) 2019 ZondaX GmbH
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "mantx.h"
#include "rlp.h"
#include "uint256.h"

#include <stdio.h>
#include <time.h>

const char *fieldNames[] = {
    "Nonce",
    "Gas Price",
    "Gas Limit",
    "To",
    "Value",
    "Data",
    "ChainID",  // According to EIP155
    "R",
    "S",
    "EnterType",
    "IsEntrustTx",
    "CommitTime",
    "ExtraTo"
};

const uint8_t displayIdxs[] = {
    MANTX_FIELD_NONCE,
    MANTX_FIELD_GASPRICE,
    MANTX_FIELD_GASLIMIT,
    MANTX_FIELD_TO,
    MANTX_FIELD_VALUE,
    MANTX_FIELD_DATA,
    MANTX_FIELD_V,
    //MANTX_FIELD_R,        // Do not show according to EIP155
    //MANTX_FIELD_S,
    MANTX_FIELD_ENTERTYPE,
    MANTX_FIELD_ISENTRUSTTX,
    MANTX_FIELD_COMMITTIME,
    MANTX_FIELD_EXTRATO
};

int8_t mantx_parse(mantx_context_t *ctx, uint8_t *data, uint16_t dataLen) {
    uint16_t fieldCount;

    // we expect a single root list
    int8_t err = rlp_parseStream(data, dataLen, &ctx->root, 1, &fieldCount);
    if (err != MANTX_NO_ERROR)
        return err;
    if (ctx->root.kind != RLP_KIND_LIST)
        return MANTX_ERROR_UNEXPECTED_ROOT;

    // now we can extract all rootFields in that list
    err = rlp_readList(data, &ctx->root, ctx->rootFields, MANTX_ROOTFIELD_COUNT, &fieldCount);
    if (err != MANTX_NO_ERROR)
        return err;
    if (fieldCount != MANTX_ROOTFIELD_COUNT)
        return MANTX_ERROR_UNEXPECTED_FIELD_COUNT;
    ctx->rootData = data + ctx->root.fieldOffset + ctx->root.valueOffset;

    // Now parse the extraTo field
    const rlp_field_t *extraToField = &ctx->rootFields[MANTX_FIELD_EXTRATO];
    err = rlp_readList(ctx->rootData, extraToField, ctx->extraToFields, MANTX_EXTRATOFIELD_COUNT, &fieldCount);
    if (err != MANTX_NO_ERROR)
        return err;
    if (fieldCount != MANTX_EXTRATOFIELD_COUNT)
        return MANTX_ERROR_UNEXPECTED_FIELD_COUNT;
    ctx->extraToData = ctx->rootData + ctx->extraToFields[0].fieldOffset + ctx->extraToFields[0].valueOffset;

    // Now parse the extraTo2 field
    const rlp_field_t *extraToField2 = &ctx->extraToFields[0];
    err = rlp_readList(ctx->extraToData, extraToField2, ctx->extraToFields2, MANTX_EXTRATO2FIELD_COUNT, &fieldCount);
    if (err != MANTX_NO_ERROR)
        return err;
    if (fieldCount != MANTX_EXTRATO2FIELD_COUNT)
        return MANTX_ERROR_UNEXPECTED_FIELD_COUNT;

    return MANTX_NO_ERROR;
}

const char *maxtx_getFieldName(uint8_t fieldIdx) {
    if (fieldIdx > MANTX_ROOTFIELD_COUNT)
        return "? Unknown";
    return fieldNames[fieldIdx];
}

int8_t mantx_print(mantx_context_t *ctx, uint8_t *data, uint8_t fieldIdx, char *out, uint16_t outLen) {
    const rlp_field_t *f = ctx->rootFields + fieldIdx;
    const uint8_t *d = ctx->rootData;

    MEMSET(out, 0, outLen);

    uint256_t tmp;

    switch (fieldIdx) {
        case MANTX_FIELD_NONCE: {
            rlp_readUInt256(d, f, &tmp);
            tostring256(&tmp, 10, out, outLen);
            break;
        }
        case MANTX_FIELD_GASPRICE: {
            rlp_readUInt256(d, f, &tmp);
            tostring256(&tmp, 10, out, outLen);
            break;
        }
        case MANTX_FIELD_GASLIMIT: {
            rlp_readUInt256(d, f, &tmp);
            tostring256(&tmp, 10, out, outLen);
            break;
        }
        case MANTX_FIELD_TO: {
            rlp_readString(d, f, out, outLen);
            break;
        }
        case MANTX_FIELD_VALUE: {
            rlp_readUInt256(d, f, &tmp);
            tostring256(&tmp, 10, out, outLen);
            break;
        }
        case MANTX_FIELD_DATA: {
            rlp_readString(d, f, out, outLen);
            break;
        }
        case MANTX_FIELD_V: {
            // TODO fix chain id
            uint8_t tmpByte;
            rlp_readByte(d, f, &tmpByte);
            snprintf(out, outLen, "%d", tmpByte);
            break;
        }
        case MANTX_FIELD_R:
            // empty response
            out[0] = 0;
            break;
        case MANTX_FIELD_S:
            // empty response
            out[0] = 0;
            break;
        case MANTX_FIELD_ENTERTYPE:
            rlp_readUInt256(d, f, &tmp);
            tostring256(&tmp, 10, out, outLen);
            break;
        case MANTX_FIELD_ISENTRUSTTX:
            rlp_readUInt256(d, f, &tmp);
            tostring256(&tmp, 10, out, outLen);
            break;
        case MANTX_FIELD_COMMITTIME: {
            rlp_readUInt256(d, f, &tmp);

            // this should be limited to uint64_t
            if (tmp.elements[0].elements[0] != 0 ||
                tmp.elements[0].elements[1] != 0 ||
                tmp.elements[1].elements[0] != 0) {
                return MANTX_ERROR_INVALID_TIME;
            }

            time_t t = tmp.elements[1].elements[1] / 1000;
            struct tm *ptm = gmtime(&t);

            strftime(out, outLen, "%d%b%Y %T", ptm);
            break;
        }
        case MANTX_FIELD_EXTRATO:
            snprintf(out, outLen, "???");
            // TODO: This will be a list
            break;
        default:
            return MANTX_ERROR_UNEXPECTED_FIELD;
    }
    return MANTX_NO_ERROR;
}

int8_t mantx_getItem(
    mantx_context_t *ctx, uint8_t *data, uint8_t displayIdx,
    char *outKey, uint16_t outKeyLen,
    char *outValue, uint16_t outValueLen) {

    if (displayIdx >= MANTX_DISPLAY_COUNT)
        return MANTX_ERROR_UNEXPECTED_DISPLAY_IDX;

    // TODO: Check displayIdx ranges
    uint8_t fieldIdx = displayIdxs[displayIdx];

    snprintf(outKey, outKeyLen, "%s", maxtx_getFieldName(fieldIdx));

    if (mantx_print(ctx, data, fieldIdx, outValue, outValueLen) != MANTX_NO_ERROR) {

    }

    return MANTX_NO_ERROR;
}