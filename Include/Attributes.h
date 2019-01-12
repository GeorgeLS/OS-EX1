#ifndef EXCERCISE_I_ATTRIBUTES_H
#define EXCERCISE_I_ATTRIBUTES_H

#ifdef __GNUC__

#define __NO_RETURN __attribute__((noreturn))
#define __INLINE __attribute__((always_inline))
#define __NON_NULL(...) __attribute__((nonnull(__VA_ARGS__)))
#define __FORMAT(formatType, index, firstToCheck) __attribute__((format(formatType, index, firstToCheck)))

#else

#define __NO_RETURN
#define __INLINE
#define __NON_NULL(...)
#define __FORMAT(formatType, index, firstToCheck)

#endif

#endif //EXCERCISE_I_ATTRIBUTES_H
