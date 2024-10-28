// export.h
#ifndef EXPORT_H
#define EXPORT_H

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define EXPORT __attribute__((dllexport))
    #else
      #define EXPORT __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define EXPORT __attribute__((dllimport))
    #else
      #define EXPORT __declspec(dllimport)
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define EXPORT __attribute__((visibility("default")))
    #define DLL_LOCAL __attribute__((visibility("hidden")))
  #else
    #define EXPORT
    #define DLL_LOCAL
  #endif
#endif

#endif // EXPORT_H
