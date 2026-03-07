#pragma once

namespace REL {
	class RelocationOffsetID {
	public:
		constexpr RelocationOffsetID(std::uintptr_t a_seOffset, std::uint64_t a_aeID) noexcept :
			_seOffset(a_seOffset),
			_aeID(a_aeID)
		{
		}

		[[nodiscard]] std::uintptr_t address() const {
			switch (REL::Module::GetRuntime()) {
			case REL::Module::Runtime::AE:
				return _aeID.address();
			case REL::Module::Runtime::SE:
				return _seOffset.address();
			default:
				return 0x0;
			}
		}

		// Convenience template to cast the address to a pointer
		template <class T = void>
		[[nodiscard]] T* get() const {
			return reinterpret_cast<T*>(address());
		}

	private:
		REL::Offset _seOffset;
		REL::ID  _aeID;
	};
}

namespace Offset
{
	namespace GASActionBufferData
	{
		inline constexpr REL::RelocationOffsetID Vtbl(0x17BC3F0, 242366);
	}

	namespace GASDoAction
	{
		inline constexpr REL::RelocationID Vtbl(291613, 242413);
	}

	namespace GFxInitImportActions
	{
		inline constexpr REL::RelocationOffsetID Vtbl(0x17DC4C8, 244866);
	}

	namespace GFxMovieDefImpl
	{
		// SkyrimSE 1.6.318.0: 0x18D0260
		inline constexpr REL::RelocationOffsetID Vtbl(0x17DD860, 243274);
	}

	namespace GFxPlaceObject2
	{
		inline constexpr REL::RelocationOffsetID Vtbl(0x17BE0E0, 242592);
	}

	namespace GFxPlaceObject3
	{
		inline constexpr REL::RelocationOffsetID Vtbl(0x17BE138, 242593);
	}

	namespace GFxRemoveObject
	{
		inline constexpr REL::RelocationOffsetID Vtbl(0x17DC408, 244863);
	}

	namespace GFxRemoveObject2
	{
		inline constexpr REL::RelocationOffsetID Vtbl(0x17DC448, 244864);
	}

	//namespace HUDMenu
	//{
	//	// SkyrimSE 1.6.318.0: 0x8AC4F0
	//	inline constexpr REL::ID Ctor(51610);
	//}

	//namespace HUDNotifications
	//{
	//	// SkyrimSE 1.6.318.0: 0x8B12B0
	//	inline constexpr REL::ID ProcessMessage(51653);
	//}

	//namespace LocalMapMenu
	//{
	//	// SkyrimSE 1.6.323.0: 0x90B530
	//	inline constexpr REL::ID PopulateData(52971);
	//}

	//namespace MapMenu
	//{
	//	// SkyrimSE 1.6.318.0: 0x9128F0
	//	inline constexpr REL::ID Ctor(53093);
	//}
}