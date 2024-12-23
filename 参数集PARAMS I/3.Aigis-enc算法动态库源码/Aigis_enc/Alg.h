﻿/**************************************************************************
 * Alg.h (Version 1.1.1) created on May 12, 2019.
 *************************************************************************/

/**************************************************************************
 * 说明：
 *		1 应采用Microsoft Visual Studio 2013在Release-Win32配置下编译项目。
 *		2 所有函数所有指针类型的形式参数必须由函数调用者分配内存。
 *		3 对于所有名字带“_with_param_fixed”后缀的函数，都需根据要求提供对
 *		  应的测试向量文件。对于不在函数形式参数列表中的算法变量必须在函数
 *		  体中由函数提供者赋固定值（以作为测试向量的一部分）。所有这类函数
 *		  都对应一个不带有该后缀的函数，二者代码的唯一区别必须仅仅是前者的
 *		  算法变量必须根据测试向量赋值。
 *************************************************************************/

#ifndef ALG_H
#define ALG_H

#define PKC_ALG_API __declspec(dllexport)

typedef unsigned char* puchar_t;
typedef unsigned long long puchar_byts_t;

/**************************************************************************
 * 函数：kem_get_pk_byts
 * 功能：获取密钥封装算法声称公钥（字节）长度，必须与算法设计文档一致
 * 返回：密钥封装算法声称公钥（字节）长度
 *************************************************************************/
PKC_ALG_API puchar_byts_t kem_get_pk_byts();

/**************************************************************************
 * 函数：kem_get_sk_byts
 * 功能：获取密钥封装算法声称私钥（字节）长度，必须与算法设计文档一致
 * 返回：密钥封装算法声称私钥（字节）长度
 *************************************************************************/
PKC_ALG_API puchar_byts_t kem_get_sk_byts();

/**************************************************************************
 * 函数：kem_get_ss_byts
 * 功能：获取密钥封装算法声称共享秘密（字节）长度，必须与算法设计文档一致
 * 返回：密钥封装算法声称共享秘密（字节）长度
 *************************************************************************/
PKC_ALG_API puchar_byts_t kem_get_ss_byts();

/**************************************************************************
 * 函数：kem_get_ct_byts
 * 功能：获取密钥封装算法声称密文（字节）长度，必须与算法设计文档一致
 * 返回：密钥封装算法声称密文（字节）长度
 *************************************************************************/
PKC_ALG_API puchar_byts_t kem_get_ct_byts();

/**************************************************************************
 * 函数：kem_keygen
 * 功能：密钥封装——密钥生成算法
 * 输出：pk：公钥
 *		 pk_byts：公钥字节长度
 *		 sk：私钥
 *		 sk_byts：私钥字节长度
 * 返回：成功执行返回0，否则返回错误代码（负数）
 *************************************************************************/
PKC_ALG_API int kem_keygen(
	puchar_t pk, puchar_byts_t* pk_byts,
	puchar_t sk, puchar_byts_t* sk_byts);

/**************************************************************************
 * 函数：kem_enc
 * 功能：密钥封装——封装算法
 * 输入：pk：公钥
 *		 pk_byts：公钥字节长度
 * 输出：ss：共享秘密
 *		 ss_byts：共享秘密字节长度
 *		 ct：密文
 *		 ct_byts：密文字节长度
 * 返回：成功执行返回0，否则返回错误代码（负数）
 *************************************************************************/
PKC_ALG_API int kem_enc(
	puchar_t pk, puchar_byts_t pk_byts,
	puchar_t ss, puchar_byts_t* ss_byts,
	puchar_t ct, puchar_byts_t* ct_byts);

/**************************************************************************
 * 函数：kem_dec
 * 功能：密钥封装——解封装算法
 * 输入：sk：私钥
 *		 sk_byts：私钥字节长度
 *		 ct：密文
 *		 ct_byts：密文字节长度
 * 输出：ss：共享秘密
 *		 ss_byts：共享秘密字节长度
 * 返回：成功执行返回0，否则返回错误代码（负数）
 *************************************************************************/
PKC_ALG_API int kem_dec(
	puchar_t sk, puchar_byts_t sk_byts,
	puchar_t ct, puchar_byts_t ct_byts,
	puchar_t ss, puchar_byts_t* ss_byts);
#endif