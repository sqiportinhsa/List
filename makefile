TARGET = build\example.exe
CPPFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -Werror=vla

.PHONY: all
all: $(TARGET)

$(TARGET): Libs\bin\logging.o bin\list.o bin\example.o
	g++ Libs\bin\logging.o bin\list.o bin\example.o -o $(TARGET) $(CPPFLAGS)

Libs\bin\logging.o: Libs\logging.cpp Libs\logging.h
	g++ -c Libs\logging.cpp -o Libs\bin\logging.o $(CPPFLAGS)

bin\list.o: list.cpp list.h
	g++ -c list.cpp -o bin\list.o $(CPPFLAGS)

bin\example.o: example.cpp list.cpp list.h
	g++ -c example.cpp -o bin\example.o