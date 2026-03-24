#ifndef SCC_SUPPORT_ATTRIBUTES_H
#define SCC_SUPPORT_ATTRIBUTES_H

#if defined(__has_attribute)
#    if __has_attribute(preferred_type)
#        define SCC_PREFERRED_TYPE(T) __attribute__((preferred_type(T)))
#    else
#        define SCC_PREFERRED_TYPE(T)
#    endif
#else
#    define SCC_PREFERRED_TYPE(T)
#endif

#endif // SCC_SUPPORT_ATTRIBUTES_H
