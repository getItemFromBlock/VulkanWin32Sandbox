#include "Resource/Mesh.hpp"

using namespace Resource;
using namespace Maths;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateDefaultCube()
{
	vertices.clear();

	const u32 faceIndices[] = { 0, 1, 3, 0, 3, 2};
	const u32 remapIndices[] = { 2, 1, 0, 0, 2, 1, 1, 0, 2 };

	for (u32 i = 0; i < 6; i++)
	{
		bool flip = i > 2;
		u32 globalIndex = (flip ? i-3 : i) * 3;
		for (u32 j = 0; j < 6; j++)
		{
			u32 id = faceIndices[j];

			Vec3 point = Vec3(id & 0x1 ? 1.0f : -1.0f, id & 0x2 ? 1.0f : -1.0f, flip ? -1.0f : 1.0f);
			Vec3 vert = Vec3(	point[remapIndices[globalIndex + 0]],
								point[remapIndices[globalIndex + 1]],
								point[remapIndices[globalIndex + 2]]);
			vert.y += 0.001f * id;
			Vec3 normal = Vec3();
			normal[remapIndices[globalIndex + 2]] = point.z;
			Vec2 uv = Vec2(-point.x, point.y) * 0.5f + 0.5f;
			u32 counter = i*6+j;
			Vec3 color = Vec3(counter & 0x1 ? 1.0f : 0.0f, counter & 0x2 ? 1.0f : 0.0f, counter & 0x4 ? 1.0f : 0.0f);
			if (i == 2 || i == 5)
				uv = Vec2(1) - uv;
			else if (i == 0 || i == 3)
				uv = Vec2(uv.y, 1-uv.x);
			if (i == 1)
				uv = uv / 2 + Vec2(0.5f, 0);
			else if (i == 4)
				uv = uv / 2;
			else
				uv = uv / 2 + Vec2(0.5f, 0.5f);
			vertices.push_back(Vertex(vert, uv, color, normal));
			if (flip && (j == 2 || j == 5))
				std::swap(vertices[vertices.size()-1],vertices[vertices.size()-2]);
		}
	}
}

const std::vector<Vertex>& Resource::Mesh::GetVertices() const
{
	return vertices;
}
