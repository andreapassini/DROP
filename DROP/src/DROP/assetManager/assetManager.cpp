#include "assetManager.h"
#include <iostream>

void AssetManager::InitAssetManager(
	AssetManagerContext* assetManager
) {
	if (!assetManager) return;

	assetManager->assetThread = std::thread(AssetLoop, assetManager);
}

void AssetManager::Enqueue(
	AssetManagerContext* assetManager
	, std::function<void()> task
) {
	if (!assetManager) return;

	{
		std::unique_lock<std::mutex> lock(assetManager->queueMutex);
		assetManager->tasks.emplace(move(task));
	}
	assetManager->queueConditionVariable.notify_one();
}

// https://www.geeksforgeeks.org/cpp/thread-pool-in-cpp/
void AssetManager::AssetLoop(
	AssetManagerContext* assetManager
) {
	if (!assetManager) return;

	int32_t i = 0;
	// we cannot use stop without locking
	while (true) {
		std::function<void()> task;

		// The reason for putting the below code
		// here is to unlock the queue before
		// executing the task so that other
		// threads can perform enqueue tasks
		{
			// Locking the queue so that data
			// can be shared safely
			std::unique_lock<std::mutex> lock(assetManager->queueMutex);

			// Waiting until there is a task to
			// execute or the pool is stopped
			assetManager->queueConditionVariable.wait(lock, [assetManager] {
				return !assetManager->tasks.empty() || assetManager->bStop;
				});

			// exit the thread in case the pool
			// is stopped and there are no tasks
			if (assetManager->bStop && assetManager->tasks.empty()) {
				return;
			}

			// Get the next task from the queue
			task = std::move(assetManager->tasks.front());
			assetManager->tasks.pop();
			i++;
			std::cout << "task num: " << i << ", queue size: " << assetManager->tasks.size() << std::endl;
		}

		task();
	}
}

void AssetManager::ShutDownAssetManager(
	AssetManagerContext* assetManager
) {
	if (!assetManager) return;

	{
		// Lock the queue to update the stop flag safely
		std::unique_lock<std::mutex> lock(assetManager->queueMutex);
		assetManager->bStop = true;
	}

	// Notify all threads
	assetManager->queueConditionVariable.notify_all();

	// Joining all worker threads to ensure they have
	// completed their tasks
	assetManager->assetThread.join();
}
