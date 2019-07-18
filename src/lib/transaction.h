/*******************************************************************************
*  (c) 2019 ZondaX GmbH
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

#include "os.h"

#define TX_NO_ERROR 0
#define TX_NO_MORE_DATA 1

void transaction_initialize();

/// Clears the transaction buffer
void transaction_reset();

/// Appends buffer to the end of the current transaction buffer
/// Transaction buffer will grow until it reaches the maximum allowed size
/// \param buffer
/// \param length
/// \return It returns an error message if the buffer is too small.
uint32_t transaction_append(unsigned char *buffer, uint32_t length);

/// Returns size of the raw json transaction buffer
/// \return
uint32_t transaction_get_buffer_length();

/// Returns the raw json transaction buffer
/// \return
uint8_t *transaction_get_buffer();

/// Parse json message stored in transaction buffer
/// This function should be called as soon as full buffer data is loaded.
/// \return It returns NULL if json is valid or error message otherwise.
const char *transaction_parse();

uint8_t transaction_getNumItems();

int8_t transaction_getItem(int8_t displayIdx,
                           char *outKey, uint16_t outKeyLen,
                           char *outValue, uint16_t outValueLen,
                           uint8_t pageIdx, uint8_t *pageCount);