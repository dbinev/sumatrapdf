/* Copyright 2006-2011 the SumatraPDF project authors (see ../AUTHORS file).
   License: FreeBSD (see ./COPYING) */

/* The most basic things, including string handling functions */
#include "BaseUtil.h"
#include "StrUtil.h"
#include "WStrUtil.h"

WCHAR * wstr_cat_s(WCHAR * dest, size_t dst_cch_size, const WCHAR * src)
{
    return wstr_catn_s(dest, dst_cch_size, src, Str::Len(src));
}

WCHAR * wstr_catn_s(WCHAR *dst, size_t dst_cch_size, const WCHAR *src, size_t src_cch_size)
{
    WCHAR *dstEnd = dst + Str::Len(dst);
    size_t len = min(src_cch_size + 1, dst_cch_size - (dstEnd - dst));
    if (dst_cch_size <= (size_t)(dstEnd - dst))
        return NULL;
    
    wcsncpy(dstEnd, src, len);
    dstEnd[len - 1] = L'\0';
    
    if (src_cch_size >= len)
        return NULL;
    return dst;
}

/* Concatenate 2 strings. Any string can be NULL.
   Caller needs to free() memory. */
WCHAR *wstr_cat(const WCHAR *str1, const WCHAR *str2)
{
    if (!str1)
        str1 = L"";
    if (!str2)
        str2 = L"";

    return wstr_printf(L"%s%s", str1, str2);
}

WCHAR *wstr_dupn(const WCHAR *str, size_t str_len_cch)
{
    WCHAR *copy;

    if (!str)
        return NULL;
    copy = (WCHAR *)memdup((void *)str, (str_len_cch + 1) * sizeof(WCHAR));
    if (copy)
        copy[str_len_cch] = 0;
    return copy;
}

int wstr_copyn(WCHAR *dst, size_t dst_cch_size, const WCHAR *src, size_t src_cch_size)
{
    size_t len = min(src_cch_size + 1, dst_cch_size);
    
    wcsncpy(dst, src, len);
    dst[len - 1] = L'\0';
    
    if (src_cch_size >= dst_cch_size)
        return FALSE;
    return TRUE;
}

int wstr_copy(WCHAR *dst, size_t dst_cch_size, const WCHAR *src)
{
    return wstr_copyn(dst, dst_cch_size, src, Str::Len(src));
}

int wstr_endswith(const WCHAR *txt, const WCHAR *end)
{
    size_t end_len;
    size_t txt_len;
    if (!txt || !end)
        return FALSE;
    txt_len = Str::Len(txt);
    end_len = Str::Len(end);
    if (end_len > txt_len)
        return FALSE;
    if (Str::Eq(txt+txt_len-end_len, end))
        return TRUE;
    return FALSE;
}

int wstr_endswithi(const WCHAR *txt, const WCHAR *end)
{
    size_t end_len;
    size_t txt_len;

    if (!txt || !end)
        return FALSE;

    txt_len = Str::Len(txt);
    end_len = Str::Len(end);
    if (end_len > txt_len)
        return FALSE;
    if (Str::EqI(txt+txt_len-end_len, end))
        return TRUE;
    return FALSE;
}

int wstr_empty(const WCHAR *str)
{
    if (!str)
        return TRUE;
    if (0 == *str)
        return TRUE;
    return FALSE;
}

WCHAR *wstr_printf(const WCHAR *format, ...)
{
    va_list args;
    WCHAR   message[256];
    size_t  bufCchSize = dimof(message);
    WCHAR * buf = message;

    va_start(args, format);
    for (;;)
    {
        int count = _vsnwprintf(buf, bufCchSize, format, args);
        if (0 <= count && (size_t)count < bufCchSize)
            break;
        /* we have to make the buffer bigger. The algorithm used to calculate
           the new size is arbitrary (aka. educated guess) */
        if (buf != message)
            free(buf);
        if (bufCchSize < 4*1024)
            bufCchSize += bufCchSize;
        else
            bufCchSize += 1024;
        buf = SAZA(WCHAR, bufCchSize);
        if (!buf)
            break;
    }
    va_end(args);

    if (buf == message)
        buf = Str::Dup(message);

    return buf;
}

int wstr_printf_s(WCHAR *out, size_t out_cch_size, const WCHAR *format, ...)
{
    va_list args;
    int count;

    va_start(args, format);
    count = _vsnwprintf(out, out_cch_size, format, args);
    if (count < 0 || (size_t)count >= out_cch_size)
        out[out_cch_size - 1] = '\0';
    va_end(args);

    return count;
}

/* Caller needs to free() the result */
char *wstr_to_multibyte(const WCHAR *txt,  UINT CodePage)
{
    assert(txt);
    if (!txt) return NULL;

    int requiredBufSize = WideCharToMultiByte(CodePage, 0, txt, -1, NULL, 0, NULL, NULL);
    char *res = SAZA(char, requiredBufSize);
    if (!res)
        return NULL;
    WideCharToMultiByte(CodePage, 0, txt, -1, res, requiredBufSize, NULL, NULL);
    return res;
}

/* Caller needs to free() the result */
WCHAR *multibyte_to_wstr(const char *src, UINT CodePage)
{
    assert(src);
    if (!src) return NULL;

    int requiredBufSize = MultiByteToWideChar(CodePage, 0, src, -1, NULL, 0);
    WCHAR *res = SAZA(WCHAR, requiredBufSize);
    if (!res)
        return NULL;
    MultiByteToWideChar(CodePage, 0, src, -1, res, requiredBufSize);
    return res;
}

void win32_dbg_outW(const WCHAR *format, ...)
{
    WCHAR   buf[4096];
    va_list args;

    va_start(args, format);
    _vsnwprintf(buf, dimof(buf), format, args);
    OutputDebugStringW(buf);
    va_end(args);
}
