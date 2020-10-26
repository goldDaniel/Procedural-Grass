#ifndef FRUSTUM_CULL_H_
#define FRUSTUM_CULL_H_

#include <glm/matrix.hpp>

class Frustum
{
public:
	Frustum() {}

	// m = ProjectionMatrix * ViewMatrix 
	Frustum(glm::mat4 m);

	// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
	bool IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const;

private:
	enum Planes
	{
		Left = 0,
		Right,
		Bottom,
		Top,
		Near,
		Far,
		Count,
		Combinations = Count * (Count - 1) / 2
	};

	template<Planes i, Planes j>
	struct ij2k
	{
		enum { k = i * (9 - i) / 2 + j - 1 };
	};

	template<Planes a, Planes b, Planes c>
	glm::vec3 intersection(const glm::vec3* crosses) const;

	glm::vec4   planes[Count];
	glm::vec3   points[8];
};

inline Frustum::Frustum(glm::mat4 m)
{
	m = glm::transpose(m);
	planes[Left] = m[3] + m[0];
	planes[Right] = m[3] - m[0];
	planes[Bottom] = m[3] + m[1];
	planes[Top] = m[3] - m[1];
	planes[Near] = m[3] + m[2];
	planes[Far] = m[3] - m[2];

	glm::vec3 crosses[Combinations] = 
	{
		glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Right])),
		glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Bottom])),
		glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Top])),
		glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Near])),
		glm::cross(glm::vec3(planes[Left]),   glm::vec3(planes[Far])),
		glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Bottom])),
		glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Top])),
		glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Near])),
		glm::cross(glm::vec3(planes[Right]),  glm::vec3(planes[Far])),
		glm::cross(glm::vec3(planes[Bottom]), glm::vec3(planes[Top])),
		glm::cross(glm::vec3(planes[Bottom]), glm::vec3(planes[Near])),
		glm::cross(glm::vec3(planes[Bottom]), glm::vec3(planes[Far])),
		glm::cross(glm::vec3(planes[Top]),    glm::vec3(planes[Near])),
		glm::cross(glm::vec3(planes[Top]),    glm::vec3(planes[Far])),
		glm::cross(glm::vec3(planes[Near]),   glm::vec3(planes[Far]))
	};

	points[0] = intersection<Left, Bottom, Near>(crosses);
	points[1] = intersection<Left, Top, Near>(crosses);
	points[2] = intersection<Right, Bottom, Near>(crosses);
	points[3] = intersection<Right, Top, Near>(crosses);
	points[4] = intersection<Left, Bottom, Far>(crosses);
	points[5] = intersection<Left, Top, Far>(crosses);
	points[6] = intersection<Right, Bottom, Far>(crosses);
	points[7] = intersection<Right, Top, Far>(crosses);

}

// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
inline bool Frustum::IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const
{
	// check box outside/inside of frustum
	for (int i = 0; i < Count; i++)
	{
		if ((glm::dot(planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
			(glm::dot(planes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
		{
			return false;
		}
	}

	// check frustum outside/inside box
	int out;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

	return true;
}

template<Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
inline glm::vec3 Frustum::intersection(const glm::vec3* crosses) const
{
	float D = glm::dot(glm::vec3(planes[a]), crosses[ij2k<b, c>::k]);
	glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
		glm::vec3(planes[a].w, planes[b].w, planes[c].w);
	return res * (-1.0f / D);
}

#endif