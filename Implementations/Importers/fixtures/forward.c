/* SPDX-License-Identifier: Apache-2.0 */
__declspec(dllimport) unsigned int __cdecl sample(unsigned int a, unsigned int b);

unsigned int forward(unsigned int a, unsigned int b)
{
    return sample(a, b);
}
