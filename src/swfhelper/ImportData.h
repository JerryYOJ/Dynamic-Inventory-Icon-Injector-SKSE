
namespace ImportData {
	struct loadReq {
		std::string_view sourcePath;
		std::vector<std::string_view> exports;
		std::vector<RE::GFxResource*> resources{}; //Caller leave it empty
	};

	RE::GFxMovieDefImpl* GetMovieDefImpl(RE::GFxMovieDef* movie);

	RE::GFxMovieDefImpl* LoadMovie(const std::string_view& a_sourcePath);
	bool ImportResources(RE::GFxMovieDefImpl* targetMovie, std::vector<loadReq>& Requests);
}