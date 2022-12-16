#include "Scene.h"

Scene::Scene() {}
Scene::~Scene() {}

void Scene::Update(float elapsedTime, XMFLOAT4 position, ID3D12CommandQueue* const commandQueue) {
	Lights::OnUpdate(elapsedTime * 0.05f, position);
	Terrain::Update(position, commandQueue);
}