CC       := g++
CFLAGS   := -Wall -Wextra -Werror -std=c++11
LIBS     := -lraylib -lopengl32 -lgdi32 -lwinmm
CXXFLAGS := -I"C:\raylib\raylib\src" -L"C:\raylib\raylib\src"
TARGET   := game
SRCS     := $(wildcard *.cpp)
OBJS     := $(SRCS:.cpp=.o)

all: $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(CXXFLAGS) $(LIBS)


clean:
	rm -f *.o *.exe