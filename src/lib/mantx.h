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

#pragma once

#include "rlp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MANTX_NO_ERROR 0
#define MANTX_ERROR_UNEXPECTED_ROOT -1
#define MANTX_ERROR_UNEXPECTED_FIELD_COUNT -2
#define MANTX_ERROR_UNEXPECTED_FIELD -3
#define MANTX_ERROR_UNEXPECTED_DISPLAY_IDX -4
#define MANTX_ERROR_INVALID_TIME -5

#define MANTX_FIELD_NONCE        0
#define MANTX_FIELD_GASPRICE     1
#define MANTX_FIELD_GASLIMIT     2
#define MANTX_FIELD_TO           3
#define MANTX_FIELD_VALUE        4
#define MANTX_FIELD_DATA         5
///
#define MANTX_FIELD_V            6
#define MANTX_FIELD_R            7
#define MANTX_FIELD_S            8
///
#define MANTX_FIELD_ENTERTYPE    9
#define MANTX_FIELD_ISENTRUSTTX  10
#define MANTX_FIELD_COMMITTIME   11
#define MANTX_FIELD_EXTRATO      12

#define MANTX_ROOTFIELD_COUNT 13
#define MANTX_EXTRATOFIELD_COUNT 1
#define MANTX_EXTRATO2FIELD_COUNT 3
#define MANTX_DISPLAY_COUNT 11

typedef struct {
    rlp_field_t root;

    uint8_t *rootData;
    rlp_field_t rootFields[MANTX_ROOTFIELD_COUNT];

    uint8_t *extraToData;
    rlp_field_t extraToFields[MANTX_EXTRATOFIELD_COUNT];

    uint8_t *extraToData2;
    rlp_field_t extraToFields2[MANTX_EXTRATOFIELD_COUNT];

} mantx_context_t;

// parse and check a tx buffer
int8_t mantx_parse(mantx_context_t *ctx, uint8_t *data, uint16_t dataLen);

const char *maxtx_getFieldName(uint8_t fieldIdx);

// get a readable output for each field
int8_t mantx_print(mantx_context_t *ctx, uint8_t *data, uint8_t fieldIdx, char *out, uint16_t outLen);

int8_t mantx_getItem(mantx_context_t *ctx, uint8_t *data, uint8_t displayIdx,
                     char *outKey, uint16_t outKeyLen,
                     char *outValue, uint16_t outValueLen);
#ifdef __cplusplus
}
#endif