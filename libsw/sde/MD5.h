// MD5.h: interface for the MD5 class.
//
//////////////////////////////////////////////////////////////////////
/*
	License to copy and use this software is granted provided that it
	is identified as the "RSA Data Security, Inc. MD5 Message-Digest
	Algorithm" in all material mentioning or referencing this software
	or this function.

	License is also granted to make and use derivative works provided
	that such works are identified as "derived from the RSA Data
	Security, Inc. MD5 Message-Digest Algorithm" in all material
	mentioning or referencing the derived work.

	RSA Data Security, Inc. makes no representations concerning either
	the merchantability of this software or the suitability of this
	software for any particular purpose. It is provided "as is"
	without express or implied warranty of any kind.

	These notices must be retained in any copies of any part of this
	documentation and/or software.
*/


#if !defined(AFX_MD5_H__22679C02_A7B5_4954_BF6F_ADA14280BE07__INCLUDED_)
#define AFX_MD5_H__22679C02_A7B5_4954_BF6F_ADA14280BE07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MD5  
{
public:
	MD5();
	virtual ~MD5();
	void MD5Update ( unsigned char *input, unsigned int inputLen);
	void MD5Final (unsigned char digest[16]);
	
private:
	unsigned long int state[4];     /* state (ABCD) */
	unsigned long int count[2];     /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];       /* input buffer */
	unsigned char PADDING[64];  /* What? */
	
private:
	void MD5Init ();
	void MD5Transform (unsigned long int state[4], unsigned char block[64]);
	void MD5_memcpy (unsigned char* output, unsigned char* input,unsigned int len);
	void Encode (unsigned char *output, unsigned long int *input,unsigned int len);
	void Decode (unsigned long int *output, unsigned char *input, unsigned int len);
	void MD5_memset (unsigned char* output,int value,unsigned int len);
	
	
};

#endif // !defined(AFX_MD5_H__22679C02_A7B5_4954_BF6F_ADA14280BE07__INCLUDED_)
