include ../../common.mk

PRODUCT=png2ilbm

# png2ilbm is a host-only tool, override to always build for sdl2
override HOST=sdl2

# Add libpng flags (use homebrew paths directly)
CFLAGS+=-I/opt/homebrew/include
LDFLAGS+=-L/opt/homebrew/lib -lpng

# Disable macro redefinition warning for __pure conflict with system headers
CFLAGS+=-Wno-macro-redefined

# Add include directory
CFLAGS+=-I ./include

include ../../product.mk
