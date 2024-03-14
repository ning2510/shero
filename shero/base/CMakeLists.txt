set(HEADERS
    Config.h
    Endian.h
    Log.h
    Macro.h
    Mutex.h
    Noncopyable.h
    Singleton.h
    Thread.h
    Timer.h
    Util.h
)

install(FILES ${HEADERS} DESTINATION include/shero/base)