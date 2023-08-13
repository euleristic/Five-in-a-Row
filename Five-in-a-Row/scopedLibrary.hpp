// RAII wrapper of glfw
#pragma once

class ScopedLibrary {
public:
	ScopedLibrary();
	~ScopedLibrary() noexcept;
};

