#include "patches.hpp"
#include "main.hpp"
#include "save.hpp"

namespace CTRPluginFramework {

	void Patches::applyCodeBinPatch()
	{
		std::string filepath = TOP_DIR "/" << std::string(OnionSave::settings.entries[OnionSave::settings.header.lastLoadedPack].name) << "/code.bin";
		DEBUG("Trying to apply code.bin patch...");
		File binFile(filepath, File::READ);
		if (!binFile.IsOpen()) {
			DEBUG(" not found, skipping.\n");
			return;
		}
		u32 bufsize = 0x100000;
		u8 *buff = (u8 *)::operator new (bufsize, std::nothrow);
		u32 filesize = binFile.GetSize();
		if (!buff) {
			DEBUG(" failed, not enough memory.\n");
			return;
		}
		int amTimes = (filesize / bufsize);
		for (int i = 0; i < amTimes; i++) {
			binFile.Read(buff, bufsize);
			u8* src = (u8*)0x100000 + i * bufsize;
			if (!Process::CopyMemory(src, buff, bufsize)) {
				DEBUG(" failed, invalid code size.\n");
				::operator delete(buff);
				return;
			}
		}
		u32 lastBit = filesize - (amTimes * bufsize);
		u32 lastAddr = 0x100000 + (amTimes * bufsize);
		binFile.Read(buff, lastBit);
		if (!Process::CopyMemory((void*)lastAddr, buff, lastBit)) {
			DEBUG(" failed, invalid code size.\n");
			::operator delete(buff);
			return;
		}
		DEBUG(" succeded.");
		::operator delete(buff);
	}

	Result  PatchRead(File& file, u8 *dst, u32 size)
	{
		Result          res;
		std::vector<u8> buf;

		buf.resize(size);
		if ((res = file.Read(buf.data(), size)))
			return res;

		std::copy(buf.begin(), buf.end(), dst);
		return res;
	}

	void Patches::applyCodeIpsPatch()
	{
		std::string filepath = TOP_DIR "/" << std::string(OnionSave::settings.entries[OnionSave::settings.header.lastLoadedPack].name) << "/code.ips";
		DEBUG("Trying to apply code.ips patch...");
		File ipsFile(filepath, File::READ);
		if (!ipsFile.IsOpen()) {
			DEBUG(" not found, skipping.\n");
			return;
		}

		u8 buffer[5];
		u64 ips_size = ipsFile.GetSize();
		u8* code = (u8*)0x00100000;

		if (ipsFile.Read(buffer, sizeof(buffer)) || memcmp(buffer, "PATCH", sizeof(buffer))) {
			DEBUG(" failed, not an ips file.\n");
			return;
		};

		while (!ipsFile.Read(buffer, 3))
		{
			if (memcmp(buffer, "EOF", 3) == 0) break;

			u32 offset = (buffer[0] << 16) | (buffer[1] << 8) | buffer[2];

			if (ipsFile.Read(buffer, 2)) { DEBUG(" failed, ips file corrupted at address: 0x%08X\n", ipsFile.Tell()); return; }

			u32 patchSize = (buffer[0] << 8) | buffer[1];

			if (!patchSize)
			{
				if (ipsFile.Read(buffer, 2)) { DEBUG(" failed, ips file corrupted at address: 0x%08X\n", ipsFile.Tell()); return; };

				u32 rleSize = (buffer[0] << 8) | buffer[1];

				if (ipsFile.Read(buffer, 1)) { DEBUG(" failed, ips file corrupted at address: 0x%08X\n", ipsFile.Tell()); return; };

				if (!Process::CheckRegion((u32)code + offset, rleSize)) { DEBUG(" failed, ips file corrupted at address: 0x%08X\n", ipsFile.Tell()); return; };
				memset(code + offset, buffer[0], rleSize);

				continue;
			}

			if (PatchRead(ipsFile, (u8*)((u32)code + offset), patchSize)) { DEBUG(" failed, ips file corrupted at address: 0x%08X\n", ipsFile.Tell()); return; };
		}
		DEBUG(" succeded.\n")
	}
}