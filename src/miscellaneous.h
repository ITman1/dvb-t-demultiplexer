#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H

#if defined(_WIN32)
    #include <dirent.h>
#else
    #include <sys/stat.h>
#endif

int createDirectory(string name) {
#if defined(_WIN32)
    return mkdir(name.c_str());
#else
    return mkdir(name.c_str(), 0777);
#endif
}

#endif // MISCELLANEOUS_H
