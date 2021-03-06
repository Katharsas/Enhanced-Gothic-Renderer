namespace MemoryOffsets
{
	namespace Gothic
	{
		namespace zCWorld
		{
			static const unsigned int GlobalVobTree = 0x24;
			static const unsigned int BSPTree = 0x1AC;
		};

		namespace zCTexture
		{
			static const unsigned int D3D7Surface = 0xD4;
		};
	};
};


namespace MemoryStaticObjects
{
	namespace Gothic
	{
		namespace DInput
		{
			// These aren't namely in the executable, and thus
			// ida doesn't spit them out in the .map-file used to generate
			// the memory-header
			static const unsigned int DInput7DeviceMouse = 0x008D1D70;
			static const unsigned int DInput7DeviceKeyboard = 0x008D1D64;
		}
	};
};