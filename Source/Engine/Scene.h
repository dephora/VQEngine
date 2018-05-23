//	VQEngine | DirectX11 Renderer
//	Copyright(C) 2016  - Volkan Ilbeyli
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
#pragma once

#include "Settings.h"	// todo: is this needed?
#include "Light.h"
#include "Mesh.h"
#include "Skybox.h"
#include "GameObject.h"
#include "GameObjectFactory.h"

#include "Utilities/Camera.h"

#include <memory>

struct SerializedScene;
class SceneManager;
class Renderer;
class TextRenderer;
struct SceneView;
struct ShadowView;
class MaterialBuffer;

#define DO_NOT_LOAD_SCENES 1

class Scene	// Base class for scenes
{
	friend class Engine;
public:
	Scene();
	~Scene() = default;

	//----------------------------------------------------------------------------------------------------------------
	// CORE INTERFACE
	//----------------------------------------------------------------------------------------------------------------

	// Initializes scene-independent variables such as renderers.
	//
	void Initialize(Renderer* pRenderer, TextRenderer* pTextRenderer);
	// void Exit(); ?

	// Moves objects from serializedScene into objects vector and sets the scene pointer in objects
	//
	void LoadScene(SerializedScene& scene, const Settings::Window& windowSettings, const std::vector<Mesh>& builtinMeshes);

	// clears object/light containers and camera settings
	//
	void UnloadScene();

	// updates selected camera and calls overridden Update from derived scene class
	//
	void UpdateScene(float dt);

	// calls .Render() on objects and calls derived class RenderSceneSpecific()
	//
	int Render(const SceneView& sceneView) const;

	// each scene has to implement scene-specific RenderUI() function. RenderUI() is called
	// after post processing is finished and is the last rendering workload before presenting the frame.
	//
	virtual void RenderUI() const = 0;

	// calls Render() on skybox.
	//
	inline void RenderSkybox(const XMMATRIX& viewProj) const { mSkybox.Render(viewProj); }


	void GatherLightData(SceneLightingData& outLightingData, ShadowView& outShadowView) const;

	inline const EnvironmentMap& GetEnvironmentMap() const { return mSkybox.GetEnvironmentMap(); }
	inline const Camera& GetActiveCamera() const	{ return mCameras[mSelectedCamera]; }
	inline const Settings::SceneRender& GetSceneRenderSettings() const { return mSceneRenderSettings; }
	inline bool  HasSkybox() const { return mSkybox.GetSkyboxTexture() != -1; }
	void ResetActiveCamera();

	EEnvironmentMapPresets GetActiveEnvironmentMapPreset() const { return mActiveSkyboxPreset; }
	void SetEnvironmentMap(EEnvironmentMapPresets preset);

protected:
	//----------------------------------------------------------------------------------------------------------------
	// VIRTUAL FUNCTIONS FOR SCENE-SPECIFIC LOGIC
	//----------------------------------------------------------------------------------------------------------------
	// https://en.wikipedia.org/wiki/Template_method_pattern
	// https://stackoverflow.com/questions/9724371/force-calling-base-class-virtual-function
	// https://isocpp.org/wiki/faq/strange-inheritance#two-strategies-for-virtuals
	// template method seems like a good idea here.
	// the idea is that base class takes care of the common tasks among all scenes and calls the 
	// customized functions of the derived classes through pure virtual functions
	// note:	int Render() is not a good idea for the derived classes: to leave the responsiblity to the implementor to count the 
	//			number of objects rendered in a scene. this definitely needs refactoring... A game object factory might be a good idea.
	virtual void Update(float dt) = 0;
	virtual void Load(SerializedScene& scene) = 0;
	virtual void Unload() = 0;
	
	GameObject*		CreateNewGameObject();

protected:
	friend class SceneResourceView; // using attorney method, alternatively can use friend function

	//----------------------------------------------------------------------------------------------------------------
	// SCENE DATA
	//----------------------------------------------------------------------------------------------------------------
	std::vector<Mesh>		mGeometry;
	std::vector<Camera>		mCameras;
	std::vector<Light>		mLights;
	MaterialBuffer			mMaterials;

	// GameObjects hold indices to the Scene Data described above
	ObjectPool mObjectPool;
	//std::vector<GameObject> mObjects;

	//----------------------------------------------------------------------------------------------------------------
	// RENDERING
	//----------------------------------------------------------------------------------------------------------------
	Renderer*				mpRenderer;		 // static?
	TextRenderer*			mpTextRenderer;	 // static?

	Skybox					mSkybox;
	EEnvironmentMapPresets	mActiveSkyboxPreset;
	
	int mSelectedCamera;

	Settings::SceneRender	mSceneRenderSettings;
};

struct SerializedScene
{
	GameObject*		CreateNewGameObject();

	std::vector<Settings::Camera>	cameras;
	std::vector<Light>				lights;
	MaterialBuffer					materials;
	std::vector<GameObject>			objects;
	Settings::SceneRender			settings;
	char loadSuccess = '0';
};

constexpr size_t SIZE_BRDF = sizeof(BRDF_Material);			// 64 Bytes
constexpr size_t SIZE_PHNG = sizeof(BlinnPhong_Material);	// 60 Bytes
// if we use 64 bits for the MaterialID, we can use 32-bit to index each array
