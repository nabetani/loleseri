#pragma once

#if __clang__ || __GNUC__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LOLESERI_LITTLE_ENDIAN 1
#else
#define LOLESERI_BIG_ENDIAN 1
#endif
#endif

#if defined _MSC_VER &&  (_M_IX86 || _M_AMD64 )
#define LOLESERI_LITTLE_ENDIAN 1
#endif

#if (! defined LOLESERI_LITTLE_ENDIAN) && (! defined LOLESERI_BIG_ENDIAN)
#error "you should define LOLESERI_LITTLE_ENDIAN or LOLESERI_BIG_ENDIAN"
#endif
