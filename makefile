GLFW_PATH = /usr/local/glfw/glfw-3.3.2
VULKAN_SDK_PATH = /usr/local/vulkan/1.2.154.0/x86_64
VULKAN_HEADERS_PATH = /usr/local/vulkan/Vulkan-Headers

EXEC = vulkan-c.out

SRCS := $(wildcard src/*.cpp)
OBJS := $(notdir $(SRCS:%.cpp=%.o))

SHADER_SRCS := $(wildcard src/shaders/*)
SHADER_OBJS := $(notdir $(SHADER_SRCS:%=%.spv))

CFLAGS =-I$(VULKAN_HEADERS_PATH)/include -I$(GLFW_PATH)/include
LDFLAGS =-L$(GLFW_PATH)/build/src -lGL -lglfw3 -lX11 -lXrandr -lpthread -ldl -lm -lvulkan

GLSLC := $(VULKAN_SDK_PATH)/bin/glslc

$EXEC: $(OBJS) $(SHADER_OBJS)
	g++ $(CFLAGS) -o bin/$(EXEC) build/*.o $(LDFLAGS)

shaders: $(SHADER_OBJS)

%.o: src/%.cpp
	g++ $(CFLAGS) -c $^ -o build/$@

%.spv: src/shaders/%
	$(GLSLC) --target-env=vulkan1.2 $^ -o bin/$@