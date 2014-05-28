#include <functional>
#include <cstdarg>
#include <thread>

template<typename ReturnType, typename ...ParamTypes>
class AsyncTaskWrapper {
	enum class State { IDLE, STARTING, RUNNING, DONE, FAILED };
public:
	AsyncTaskWrapper(std::function<ReturnType(ParamTypes...)> t, ParamTypes...prm) : state(State::IDLE) {
		task = std::bind(t, prm...);
	};

	void Start() {
		// Only try to start the task if we're in idle
		if (state != State::IDLE) {
			return;
		}

		state = State::STARTING;
		thread = std::thread(&AsyncTaskWrapper::_ExecuteTask, this);
	}

	ReturnType GetResultBlocking() {
		if (state == State::IDLE) {
			Start();
		}
		thread.join();

		return result;
	}

	ReturnType GetResult() {
		if (state != State::DONE) {
			throw "Task is not done, result is not ready";
		}
		thread.join();

		return result;
	}

	bool IsRunning() { return state == State::RUNNING; }
	bool IsDone() { return state == State::DONE; }
private:
	State state;
	std::function<ReturnType()> task;
	ReturnType result;
	std::thread thread;

	void _ExecuteTask() {
		state = State::RUNNING;
		result = task();
		state = State::DONE;
	}

	AsyncTaskWrapper() = delete;
	AsyncTaskWrapper(const AsyncTaskWrapper&) = delete;
};