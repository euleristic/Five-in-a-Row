// RAII wrapper of GLFW
// Responsibility can be moved and not copied, but nothing stops the program from creating
// multiple instances of the object, which may lead to a surprising life-time of GLFW
#pragma once

class ScopedLibrary {
	bool owner;
public:
	ScopedLibrary();
	~ScopedLibrary() noexcept;

	// The program can move the responsibility of destruction
	ScopedLibrary(ScopedLibrary&& other) noexcept;
	ScopedLibrary& operator=(ScopedLibrary&& other) noexcept;

	// ScopedLibrary cannot be copied
	ScopedLibrary(const ScopedLibrary&) = delete;
	ScopedLibrary& operator=(const ScopedLibrary&) = delete;
};

