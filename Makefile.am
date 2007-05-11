#
# Build libhashcash
#

lib_LIBRARIES           = libhashcash
libhashcash_a_CPPFLAGS  = -DREGEXP_POSIX
libhashcash_a_SOURCES   = \
  sdb.c lock.c sstring.c random.c libsha1.c libfastmint.c       \
  fastmint_mmx_standard_1.c fastmint_mmx_compact_1.c            \
  fastmint_ansi_compact_1.c fastmint_ansi_standard_1.c          \
  fastmint_ansi_compact_2.c fastmint_ansi_standard_2.c          \
  fastmint_altivec_standard_1.c fastmint_altivec_standard_2.c   \
  fastmint_altivec_compact_2.c fastmint_ansi_ultracompact_1.c   \
  fastmint_library.c
