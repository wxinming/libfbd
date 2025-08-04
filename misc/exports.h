#pragma once

#define LIBFBD_NOT_EXPORTS
#ifdef LIBFBD_NOT_EXPORTS
#define LIBFBD_DLL_EXPORT
#else
//±àÒëÎª¶¯Ì¬Á´½Ó¿â
#ifdef LIBFBD_EXPORTS
#define LIBFBD_DLL_EXPORT __declspec(dllexport)
#pragma warning(disable:4251)
#else
#define LIBFBD_DLL_EXPORT __declspec(dllimport)
#endif // LIBFBD_EXPORTS
#endif // LIBFBD_NOT_EXPORTS
