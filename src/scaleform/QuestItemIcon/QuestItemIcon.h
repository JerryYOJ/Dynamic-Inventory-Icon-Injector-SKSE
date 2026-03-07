class QuestItemIcon : public HookTemplate<QuestItemIcon> {
public:
	static void Install();
	static void InstallLate();

protected:
	static void PostCreateInv(RE::IMenu* thiz);
	static void PostCreateCont(RE::IMenu* thiz);

	inline static REL::Relocation<decltype(PostCreateInv)> _PostCreateInv;
	inline static REL::Relocation<decltype(PostCreateCont)> _PostCreateCont;
};