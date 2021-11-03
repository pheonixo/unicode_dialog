CFLAGS := -std=gnu99 -pipe -fno-builtin -fmessage-length=0 -O2
WARNINGS := -Wall -Wextra -Wnormalized -Wuninitialized -Winit-self -Wno-unknown-pragmas
XTRA_WARN := -Wno-misleading-indentation -Wno-implicit-fallthrough -Wno-maybe-uninitialized -Wno-missing-braces
LFLAGS := -std=gnu99 -pipe -fno-builtin -fmessage-length=0 -O2

uni_table: uni_table.c ctype_compress_no_locale.c
	mkdir -p ./build
	gcc $^ $(CFLAGS) $(WARNINGS) $(XTRA_WARN) `pkg-config --cflags --libs gtk+-3.0` -L./build -lpthread -o ./build/$@


