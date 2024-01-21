#include "MenuCache.h"
#include "Settings.h"
#include <future>

namespace SkyrimSoulsRE::MenuCache
{
	std::unordered_map<std::string, std::future<RE::GFxMovieView*>> viewsCache;

	// Experimental caching of new menu instances.
	// Can significantly reduce stuttering after a menu has been opened at least once.
	// This is disabled by default, see settings->enableMenuCaching option.
	RE::GFxMovieView* CreateInstance_Hook(RE::GFxMovieDefImpl* a_this, const RE::GFxMovieDef::MemoryParams& a_memParams, bool a_initFirstFrame)
	{
		if (a_this)
		{
			std::string url = a_this->GetFileURL();
			for (char& c : url)
			{
				c = std::tolower(c);
			}

			auto it = viewsCache.find(url);

			if (it != viewsCache.end())
			{
				it->second.wait();

				if (it->second.valid())
				{
					// Return the async instance
					RE::GFxMovieView* asyncView = it->second.get();
					viewsCache.erase(it);
					SKSE::log::info("Returning async instance for: '{}'", url);

					a_this->AddRef();

					// Start creating the next one
					std::future<RE::GFxMovieView*> futureInstance = std::async(std::launch::async, [a_this, a_memParams, a_initFirstFrame]() {
						auto result = a_this->CreateInstance(a_memParams, a_initFirstFrame);
						a_this->Release();
						return result;
					});

					viewsCache[url] = std::move(futureInstance);

					return asyncView;
				}
			}
			else
			{
				a_this->AddRef();

				// Start async creation of the instance
				std::future<RE::GFxMovieView*> futureInstance = std::async(std::launch::async, [a_this, a_memParams, a_initFirstFrame]() {
					auto result = a_this->CreateInstance(a_memParams, a_initFirstFrame);
					a_this->Release();
					return result;
				});

				viewsCache[url] = std::move(futureInstance);
			}
		}

		// Return the initial synchronous instance
		RE::GFxMovieView* synchronousInstance = a_this->CreateInstance(a_memParams, a_initFirstFrame);
		SKSE::log::info("Returning the initial synchronous instance from URL '{}'.", a_this->GetFileURL());
		return synchronousInstance;
	}

	RE::GFxMovieView* CreateInstance_Hook(RE::GFxMovieDefImpl* a_this, const RE::GFxMovieDef::MemoryParams& a_memParams, bool a_initFirstFrame)
	{
		if (Settings::GetSingleton()->enableMenuCaching)
		{
			SKSE::log::warn("Menu caching has been enabled. This feature is experimental.");
			SKSE::GetTrampoline().write_call<6>(REL::ID{ 82325 }.address() + 0x1B8, (uintptr_t)CreateInstance_Hook);
		}
	}
}
