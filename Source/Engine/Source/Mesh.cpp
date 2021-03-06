//	VQEngine | DirectX11 Renderer
//	Copyright(C) 2018  - Volkan Ilbeyli
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//	Contact: volkanilbeyli@gmail.com

#include "Mesh.h"
#include "Utilities/Log.h"

#define VERBOSE_LOGGING 0


// Renderer* is used to create Vertex/Index buffer data from
// Mesh constructor as the constructor receives raw vertex/index
// buffer data.
Renderer* Mesh::spRenderer = nullptr;

std::pair<BufferID, BufferID> Mesh::GetIABuffers(int lod /*= 0*/) const
{
	assert(mLODs.size() > 0); // maybe no assert and return <-1, -1> ?

	if (lod < mLODs.size())
	{
		return mLODs[lod].GetIABufferPair();
	}

#if VERBOSE_LOGGING
	Log::Warning("Requested LOD level (%d) doesn't exist (LOD levels = %d). Returning LOD=0", lod, static_cast<int>(mLODs.size()));
#endif

	return mLODs.back().GetIABufferPair();
}

