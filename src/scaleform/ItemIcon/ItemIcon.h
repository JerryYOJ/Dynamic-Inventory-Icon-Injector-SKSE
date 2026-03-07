class ItemIcon : public HookTemplate<ItemIcon> {
public:
	static void Install();
	static void InstallLate();

protected:
	static void PostCreateInv(RE::IMenu* thiz);
	static void PostCreateCont(RE::IMenu* thiz);
	static void PostCreateBart(RE::IMenu* thiz);
	static void PostCreateGift(RE::IMenu* thiz);

	inline static REL::Relocation<decltype(PostCreateInv)> _PostCreateInv;
	inline static REL::Relocation<decltype(PostCreateCont)> _PostCreateCont;
	inline static REL::Relocation<decltype(PostCreateBart)> _PostCreateBart;
	inline static REL::Relocation<decltype(PostCreateGift)> _PostCreateGift;
};