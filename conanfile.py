from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class hanoip83Recipe(ConanFile):
    name = "hanoip83"
    version = "1.0"
    package_type = "application"

    # Optional metadata
    license = "MIT"
    author = "Cyoutous"
    url = "https://github.com/Cyoutous/HanoiP83"
    description = "A classic Tower of Hanoi puzzle game built with C++17, EnTT ECS, and raylib."
    topics = ("game", "hanoi-tower", "entt", "raylib", "ecs")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    # exports_sources = "CMakeLists.txt", "src/*"

    def requirements(self):
        self.requires("raylib/5.5")
        self.requires("entt/3.16.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    # def package(self):
    #     cmake = CMake(self)
    #     cmake.install()

    

    
