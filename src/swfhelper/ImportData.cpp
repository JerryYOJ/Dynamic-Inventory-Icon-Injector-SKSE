#include "ImportData.h"

#include "SWF/TagFactory.h"

RE::GFxMovieDefImpl* ImportData::GetMovieDefImpl(RE::GFxMovieDef* movie)
{
	static REL::Relocation vtbl{ Offset::GFxMovieDefImpl::Vtbl.address() };
	if (*reinterpret_cast<std::uintptr_t*>(movie) == vtbl.address()) {
		return static_cast<RE::GFxMovieDefImpl*>(movie);
	}
	else {
		logger::critical("Failed to get GFxMovieDefImpl from GFxMovieDef, unexpected vtable {:X}"sv, *reinterpret_cast<std::uintptr_t*>(movie));
		return nullptr;
	}
}

RE::GFxMovieDefImpl* ImportData::LoadMovie(const std::string_view& a_sourcePath)
{
	const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	const auto loader = scaleformManager->loader;
	auto movieDef = loader->CreateMovie(a_sourcePath.data());
	if (!movieDef) {
		logger::error("Failed to create movie from {}"sv, a_sourcePath);
		return nullptr;
	}
	movieDef->WaitForLoadFinish();

	auto movieDefImpl = ImportData::GetMovieDefImpl(movieDef);

	return movieDefImpl;
}

bool ImportData::ImportResources(RE::GFxMovieDefImpl* targetMovie, std::vector<loadReq>& Requests)
{
	std::unordered_map<std::string_view, RE::GFxMovieDefImpl*> sourceMovies;

	for (auto&& [sourcePath, exports, resources] : Requests) {
		//if (sourceMovies.contains(sourcePath)) continue;

		auto&& movieDef = LoadMovie(sourcePath);
		if (!movieDef) continue;
		sourceMovies[sourcePath] = movieDef;

		for (auto&& it : exports) {
			resources.push_back(movieDef->GetResource(it.data()));
		}
	}

	if (sourceMovies.empty()) return false;

	auto&& bindTaskData = targetMovie->bindTaskData;
	auto&& movieData = bindTaskData->movieDataResource;
	auto&& loadTaskData = movieData->loadTaskData;

	std::unordered_map<std::string_view, uint32_t> movieIndexByPath;
	auto oldImportedMoviesCount = bindTaskData->importedMovies.GetSize();
	bindTaskData->importedMovies.Reserve(oldImportedMoviesCount + sourceMovies.size());
	
	for (auto&& [path, srcMovie] : sourceMovies) {
		if (!srcMovie) continue;

		movieIndexByPath[path] = bindTaskData->importedMovies.GetSize();
		bindTaskData->importedMovies.PushBack(srcMovie);

	}

	auto arr_size = sizeof(RE::GASExecuteTag*) * sourceMovies.size();
	auto tagList = static_cast<RE::GASExecuteTag**>(loadTaskData->allocator.Alloc(arr_size));
	
	for (std::size_t i = 0, size = sourceMovies.size(); i < size; i++) {
		auto movie = oldImportedMoviesCount + i;
		tagList[i] = SWF::TagFactory::MakeInitImportActions(movieData, static_cast<std::uint32_t>(movie));
	}

	loadTaskData->importFrames.PushBack({ 
		.data = tagList, 
		.size = static_cast<std::uint32_t>(sourceMovies.size())
	});

	loadTaskData->importFrameCount++;

	auto&& importData = bindTaskData->importData;
	auto&& resources = loadTaskData->resources;

	std::vector<RE::GFxMovieDefImpl::ImportedResource> newImportedEntries;
	std::uint16_t nextID = 0;

	for (auto&& [source, exports, res] : Requests) {
		auto&& movieDef = sourceMovies[source];
		if (!movieDef) continue;

		auto Idx = movieIndexByPath[source];

		auto importNode = new (loadTaskData->allocator.Alloc(sizeof(RE::GFxImportNode)))
			RE::GFxImportNode{
				.filename = source.data(),
				.frame = static_cast<std::uint32_t>(loadTaskData->importFrames.GetSize()),
				.movieIndex = Idx
			};

		for (int k = 0; k < exports.size(); k++) {
			RE::GFxResourceID resourceId;
			do {
				resourceId = RE::GFxResourceID(++nextID);
			} while (resources.Find(resourceId) != resources.end());

			RE::GFxResourceSource srcInfo{};
			srcInfo.type = RE::GFxResourceSource::kImported;
			srcInfo.data.importSource.index = loadTaskData->importedResourceCount;

			loadTaskData->importedResourceCount++;
			resources.Add(resourceId, srcInfo);

			RE::GFxImportNode::ImportAssetInfo assetInfo{
				.name = RE::GString{ exports[k].data() },
				.id = nextID,
				.importIndex = srcInfo.data.importSource.index
			};
			importNode->assets.PushBack(assetInfo);

			RE::GFxMovieDefImpl::ImportedResource imp{};
			imp.importData = std::addressof(movieDef->bindTaskData->importData);
			imp.resource = RE::GPtr(res[k]);

			newImportedEntries.push_back(imp);
		}

		if (loadTaskData->importInfoBegin) {
			auto& prev = loadTaskData->importInfoEnd;
			prev->nextInChain = importNode;
		}
		else {
			loadTaskData->importInfoBegin = importNode;
		}

		loadTaskData->importInfoEnd = importNode;
	}

	std::uint32_t newCount = (loadTaskData->importedResourceCount + 0xF) & -0x10;
	if (newCount > importData.importCount) {
		std::size_t newSize = sizeof(RE::GFxMovieDefImpl::ImportedResource) * newCount;
		auto newArray = new (importData.heap->Alloc(newSize, 0)) RE::GFxMovieDefImpl::ImportedResource[newCount];
	
		std::copy_n(importData.resourceArray, importData.importCount, newArray);

		for (std::uint32_t i = 0; i < importData.importCount; i++) {
			importData.resourceArray[i].~ImportedResource();
		}

		importData.heap->Free(importData.resourceArray);

		importData.resourceArray = newArray;
		importData.importCount = newCount;
	}

	std::uint32_t start = loadTaskData->importedResourceCount - static_cast<std::uint32_t>(newImportedEntries.size());

	std::copy(
		newImportedEntries.begin(),
		newImportedEntries.end(),
		std::addressof(importData.resourceArray[start])
	);

	return true;
}
