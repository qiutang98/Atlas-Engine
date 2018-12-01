#ifndef TERRAIN_H
#define TERRAIN_H

#include "../System.h"
#include "TerrainNode.h"
#include "TerrainStorage.h"

#include "../Camera.h"
#include "../Texture.h"
#include "../VertexArray.h"

#include <vector>

/**
 * Represents a terrain. This class uses different LoDs and tessellation to render the terrain
 * efficiently with high detail without sacrificing too much on performance.
 */
class Terrain {

public:
	/**
	 * Constructs a terrain
	 * @param rootNodeCount The number of root nodes. Root nodes are the lowest level of detail
	 * @param LoDCount The number of level of detail levels. Results in LoDCount - 1 subdivisions of the root nodes.
	 * @param patchSizeFactor Changes the size of a patch and the number of vertices in a patch.
	 * @param resolution Like a scale in x and z direction of the terrain. Lower means more vertices per square unit.
	 * @param height The maximum height of the terrain
	 * @note The maximum number of nodes in a terrain is 2^16.
	 * @example Let's assume we want 3 root nodes on each side of the terrain which leads to rootNodeCount = 3^2 = 9.
	 * We now decide that we want an LoDCount of 7. To check if we don't exceed the maximum number of nodes we calculate
	 * rootNodeCount * (pow(4,LoDCount) - 1) / 3 = 3 * (pow(4, 7) - 1) = 49149 < 2^16 = 65536. This means that at the
	 * maximum lod which is Lod6 we have a total of pow(4, 6) * 9 = 36864 nodes. If we have a patchSizeFactor of 4
	 * we have 16 * 4 * 4 vertices per patch. Each node has 8 * 8 = 64 patches. This results in a maximum vertex count of
	 * 603979776. If we take the square root we get the number of vertices per side: sqrt(603979776) = 24576. If we set
	 * the resolution to 0.5 we get a total of 12288 units per terrain side. The size of map is therefore roughly
	 * 150 kilounits^2.
	 */
	Terrain(int32_t rootNodeCount, int32_t LoDCount, int32_t patchSizeFactor, float resolution, float height);

	/**
	 * Updates the terrain and the storage queues.
	 * @param camera
	 * @note After calling this method the storage->requestedCells list contains all the
	 * StorageCells which are needed by the terrain. If these StorageCells aren't loaded the terrain
	 * can't increase the level of detail of the specific nodes. The storage->unusedCells list contains
	 * all the cells which aren't needed any more at the moment.
	 */
	void Update(Camera* camera);

	/**
	 * Sets the distance of a specific level of detail.
	 * @param LoD The level of detail to be set in range of (0,LoDCount-1)
	 * @param distance The distance where the level of details should begin
	 * @note Only the highest level of detail will render splatmaps and tessellation.
	 */
	void SetLoDDistance(int32_t LoD, float distance);

	/**
	 * Sets the tessellation function t(distance) = factor / pow(distance, slope) + shift
	 * @param factor
	 * @param slope
	 * @param shift
	 * @param maxLevel Determines the maximum level of subdivisions the tessellation uses.
	 * @note The tessellation for a point p is calculated by:
	 * distance = length(camera->location, p)
	 * tessLevel = mix(1, maxLevel, clamp(t(distance), 0, 1))
	 */
	void SetTessellationFunction(float factor, float slope, float shift, int32_t maxLevel = 64);

	/**
	 * Sets the distance where the displacement should start
	 * @param distance The distance
	 * @note Displacement mapping is only available if tessellation is enabled in the specified distance.
	 * The maximum distance recommended is t(distance) = 1, where t is the tessellation function.
	 * Otherwise there would be too much flickering present on screen.
	 */
	void SetDisplacementDistance(float distance);

	/**
	 * Binds the vertex array of the terrain
	 */
	void Bind();

	/**
	 * Unbinds the vertex array of the terrain
	 */
	void Unbind();

	TerrainStorage* storage;

	vec3 translation;
	float resolution;

	int32_t patchVertexCount;
	int32_t patchSize;
	vector<vec2> patchOffsets;

	float tessellationFactor;
	float tessellationSlope;
	float tessellationShift;
	int32_t maxTessellationLevel;

	float heightScale;
	float displacementDistance;

	vector<TerrainNode*> renderList;

private:
	void GeneratePatchVertexBuffer(int32_t patchSizeFactor);

	void GeneratePatchOffsets(int32_t patchSizeFactor);

	vector<vec2> vertices;

	VertexArray* vertexArray;

	int32_t rootNodeCount;
	int32_t LoDCount;

	vector<float> LoDDistances;
	vector<TerrainNode*> rootNodes;

};


#endif