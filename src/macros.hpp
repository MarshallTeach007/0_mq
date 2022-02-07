
/******************************************************************************/
/*  0MQ Internal Use                                                          */
/******************************************************************************/

#define LIBSZMQ_UNUSED(object) (void) object
#define LIBSZMQ_DELETE(p_object)                                                \
    {                                                                          \
        delete p_object;                                                       \
        p_object = 0;                                                          \
    }

/******************************************************************************/

#if !defined SZMQ_NOEXCEPT
#if defined SZMQ_HAVE_NOEXCEPT
#define SZMQ_NOEXCEPT noexcept
#else
#define SZMQ_NOEXCEPT
#endif
#endif

#if !defined SZMQ_OVERRIDE
#if defined SZMQ_HAVE_NOEXCEPT
#define SZMQ_OVERRIDE override
#else
#define SZMQ_OVERRIDE
#endif
#endif

#if !defined SZMQ_FINAL
#if defined SZMQ_HAVE_NOEXCEPT
#define SZMQ_FINAL final
#else
#define SZMQ_FINAL
#endif
#endif

#if !defined SZMQ_DEFAULT
#if defined SZMQ_HAVE_NOEXCEPT
#define SZMQ_DEFAULT = default;
#else
#define SZMQ_DEFAULT                                                            \
    {                                                                          \
    }
#endif
#endif

#if !defined SZMQ_NON_COPYABLE_NOR_MOVABLE
#if defined SZMQ_HAVE_NOEXCEPT
#define SZMQ_NON_COPYABLE_NOR_MOVABLE(classname)                                \
  public:                                                                      \
    classname (const classname &) = delete;                                    \
    classname &operator= (const classname &) = delete;                         \
    classname (classname &&) = delete;                                         \
    classname &operator= (classname &&) = delete;
#else
#define SZMQ_NON_COPYABLE_NOR_MOVABLE(classname)                                \
  private:                                                                     \
    classname (const classname &);                                             \
    classname &operator= (const classname &);
#endif
#endif
