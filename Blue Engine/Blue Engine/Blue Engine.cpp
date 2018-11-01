// Blue Engine.cpp: Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "Blue Engine\src\engine.h"

int main(int argc, char* argv[])
{

	Window* window = Engine::Init("Blue Engine", WINDOWPOSITION_UNDEFINED, WINDOWPOSITION_UNDEFINED,
		1280, 720, WINDOW_RESIZABLE);

	Camera* camera = new Camera(45.0f, 2.0f, 1.0f, 200.0f);
	camera->location = vec3(0.0f, 0.0f, 0.0f);
	camera->thirdPerson = true;
	camera->thirdPersonDistance = 15.0f;

	Texture* texture = new Texture("image.png");

	window->SetIcon(texture);

	RenderTarget* target = new RenderTarget(1920, 1080);

	Scene* scene = new Scene();

	Mesh* mesh = new Mesh("cube.dae");

	Actor* actor = new Actor(mesh);

	PostProcessRenderer* postProcessRenderer = new PostProcessRenderer("shader/postprocessing.vsh", "shader/postprocessing.fsh");
	target->texture = new Texture("nice.jpg");
	target->texture->FlipHorizontally();

	GeometryRenderer* geoRenderer = new GeometryRenderer();

	SceneNode* node = new SceneNode();

	scene->rootNode->Add(node);
	node->Add(actor);

	node->transformationMatrix = glm::translate(vec3(0.0f, 0.0f, 5.0f));

	// For now we will leave the main loop here until we implement a more advanced event system
	// Our event structure
	SDL_Event event;
	bool quit = false;

	// We need the time passed per frame in the rendering loop
	unsigned int time = 0;

	// Handle events and rendering here
	while (!quit) {

		unsigned int deltatime = SDL_GetTicks() - time;
		time = SDL_GetTicks();

		// Poll all the events
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {

				// If the SDL event is telling us to quit we should do it
				quit = true;

			}
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {

				// If the user presses escape we also want to quit
				quit = true;

			}

		}

		camera->UpdateView();
		camera->UpdateProjection();

		scene->rootNode->transformationMatrix = glm::rotate((float)time / 1000.0f, vec3(0.0f, 1.0f, 0.0f));
		actor->modelMatrix = glm::rotate((float)time / 500.0f, vec3(0.0f, 1.0f, 0.0f));

		scene->Update();

		geoRenderer->Render(window, target, camera, scene);
		postProcessRenderer->Render(window, target, camera, scene);

		window->Update();

	}

	delete camera;
	delete window;

    return 0;
}

