#ifndef FLAG
#    ifdef OPT_ENUM
#        define FLAG(Enum, FlagType, Kind, VType, Desc, Hidden) Opt_##Enum,
#    else
#        define FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)                                    \
            {Opt_##Enum, FlagType, OptKind::Kind, ValType::VType, Desc, Hidden},
#    endif
#endif


#ifndef BASIC_FLAG
#    define BASIC_FLAG(Enum, FlagType, Desc, Hidden) FLAG(Enum, FlagType, Flag, None, Desc, Hidden)
#endif

#ifndef BASIC_FLAG_VALUE
#    define BASIC_FLAG_VALUE(Enum, FlagType, Desc, Hidden)                                         \
        FLAG(Enum, FlagType, JoinedOrSeparate, Str, Desc, Hidden)
#endif

#ifndef BASIC_FLAG_VALUES
#    define BASIC_FLAG_VALUES(Enum, FlagType, Desc, Hidden)                                        \
        FLAG(Enum, FlagType, JoinedOrSeparate, StrList, Desc, Hidden)
#endif

#ifndef EQUAL_FLAG
#    define EQUAL_FLAG(Enum, FlagType, Desc, Hidden)                                               \
        FLAG(Enum, "--" #FlagType "=", Equal, Str, Desc, Hidden)
#endif

#ifndef WARN_FLAG
#    define WARN_FLAG(Enum, FlagType, Desc, Hidden)                                                \
        BASIC_FLAG(W##Enum, "-W" #FlagType, "Enable " Desc, Hidden)                                \
        BASIC_FLAG(Wno##Enum, "-Wno-" #FlagType, "Disable " Desc, Hidden)
#endif


#ifndef ALIAS_FLAG
#    ifdef OPT_ENUM
#        define ALIAS_FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)
#    else
#        define ALIAS_FLAG(Enum, FlagType, Kind, VType, Desc, Hidden)                              \
            {Opt_##Enum, FlagType, OptKind::Kind, ValType::VType, Desc, Hidden},
#    endif
#endif

#ifndef ALIAS_BASIC_FLAG
#    define ALIAS_BASIC_FLAG(Enum, FlagType, Desc, Hidden)                                          \
        ALIAS_FLAG(Enum, FlagType, Flag, None, Desc, Hidden)
#endif

#ifndef ALIAS_BASIC_FLAG_VALUE
#    define ALIAS_BASIC_FLAG_VALUE(Enum, FlagType, Desc, Hidden)                                    \
        ALIAS_FLAG(Enum, FlagType, JoinedOrSeparate, Str, Desc, Hidden)
#endif

#ifndef ALIAS_BASIC_FLAG_VALUES
#    define ALIAS_BASIC_FLAG_VALUES(Enum, FlagType, Desc, Hidden)                                   \
        ALIAS_FLAG(Enum, FlagType, JoinedOrSeparate, StrList, Desc, Hidden)
#endif

#ifndef ALIAS_EQUAL_FLAG
#    define ALIAS_EQUAL_FLAG(Enum, FlagType, Desc, Hidden)                                          \
        ALIAS_FLAG(Enum, "--" #FlagType "=", Equal, Str, Desc, Hidden)
#endif
