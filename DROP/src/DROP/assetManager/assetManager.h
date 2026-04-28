#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>


struct AssetManagerContext 
{
	std::thread assetThread;
	
	// Queue of tasks
	std::queue<std::function<void()>> tasks;

	std::mutex queueMutex;
	std::condition_variable queueConditionVariable;
	bool bStop = false;
};

namespace AssetManager
{
	void InitAssetManager(AssetManagerContext* assetManager);
	void ShutDownAssetManager(AssetManagerContext* assetManager);
	void Enqueue(
		AssetManagerContext* assetManager
		, std::function<void()> task
	);
	void AssetLoop(AssetManagerContext* assetManager);
}