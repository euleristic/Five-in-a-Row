#include "decisionComputer.hpp"
#include "goalFunction.hpp"

DecisionComputer::DecisionComputer() {

	// Run the thread
	thread = std::thread([&]() {
		constexpr Minimax<Constants::PLY_LOOK_AHEAD, true, GoalFunction, true> redComputer;
		constexpr Minimax<Constants::PLY_LOOK_AHEAD, false, GoalFunction, true> blueComputer;
		while (true) {
			
			// Await the begin signal
			begin.acquire();
			
			// Is the signal from the destructor?
			if (dead)
				return;

			// Call minimax
			result = blue ? blueComputer(*board) : redComputer(*board);

			// Signal completion
			complete.release();
		}
	});
}

DecisionComputer::~DecisionComputer() {
	dead = true;
	
	// Awake the thread
	begin.release();

	// Await it
	thread.join();
}

void DecisionComputer::operator()(const Board& board, const bool blue) {
	// No data races
	if (running) {
		Await();
	}
	this->board = &board;
	this->blue = blue;
	running = true;

	// Awake the thread
	begin.release();
}

bool DecisionComputer::Running() const {
	return running;
}

std::optional<size_t> DecisionComputer::TryResult() noexcept {
	// Non-blocking acquire attempt
	if (complete.try_acquire()) {
		// It's done! Return the result
		running = false;
		return result;
	}
	// It's still running, return the null-opt
	return {};
}

void DecisionComputer::Await() noexcept {
	// Block until the thread signals completion
	complete.acquire();
	running = false;
}