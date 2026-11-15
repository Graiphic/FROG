/* SPDX-License-Identifier: Apache-2.0 */
unsigned int missing_helper(unsigned int value);
unsigned int missing_entry(unsigned int value) { return missing_helper(value); }
#ifdef _WIN32
__declspec(dllimport) unsigned int vendor_helper(unsigned int value);
unsigned int external_entry(unsigned int value) { return vendor_helper(value); }
#endif
