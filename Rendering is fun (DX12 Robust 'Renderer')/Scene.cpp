#include "Scene.h"

Scene::Scene() {}
Scene::~Scene() {}

void Scene::Update(float elapsedTime, XMFLOAT4 position) {
	Lights::OnUpdate(elapsedTime * 0.05f, position);
	Terrain::Update(position);
}