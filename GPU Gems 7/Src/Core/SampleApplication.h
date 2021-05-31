#ifndef SAMPLE_APPLICATION_H_
#define SAMPLE_APPLICATION_H_

#include "Application.h"

#include "Renderer/Renderable.h"

class SampleApplication : public Application
{
private:
	MetricsGuiMetric frameTimeMetric;
	MetricsGuiPlot frameTimePlot;

	Renderable* renderable;

	Camera cam;

	glm::vec2 prevMousePos;

public:
	SampleApplication() : Application()
	{
		frameTimeMetric = MetricsGuiMetric("Frame time", "s", MetricsGuiMetric::USE_SI_UNIT_PREFIX);
		frameTimeMetric.mSelected = true;
		frameTimePlot.mShowAverage = true;
		frameTimePlot.mShowLegendAverage = true;
		frameTimePlot.mShowLegendColor = false;
		frameTimePlot.AddMetric(&frameTimeMetric);


		auto vArr = renderer->CreateVertexArray();
		const float pos[] =
		{
			-1000, 0, -1000,
			 1000, 0, -1000,
			 1000, 0,  1000,
			-1000, 0,  1000,
		};
		const float col[] =
		{
			0, 1, 1,
			1, 0, 0,
			1, 0, 1,
			1, 1, 0,

		};
		vArr->AddVertexBuffer(pos, sizeof(float) * 12, 3);
		vArr->AddVertexBuffer(col, sizeof(float) * 12, 3);

		auto iBuf = renderer->CreateIndexBuffer({ 0, 1, 2, 3, 0, 2 });
		auto shader = renderer->CreateShader("Assets/Shaders/VertexShader.glsl", "Assets/Shaders/FragmentShader.glsl");

		renderable = renderer->CreateRenderable(shader, vArr, iBuf);
	}

	~SampleApplication()
	{

	}

protected:

	virtual void Update(float dt) override
	{
		glm::vec2 mousePos = input->GetMousePos();
		glm::vec2 mouseDelta = mousePos - prevMousePos;
		prevMousePos = mousePos;
        if (input->IsButtonDown(SDL_BUTTON_LEFT))
        {
            cam.ProcessMouseMovement(mouseDelta.x, mouseDelta.y);
        }

        if (input->IsKeyDown(SDLK_a)) cam.ProcessKeyboard(Camera_Movement::LEFT, dt);
        if (input->IsKeyDown(SDLK_d)) cam.ProcessKeyboard(Camera_Movement::RIGHT, dt);

        if (input->IsKeyDown(SDLK_w)) cam.ProcessKeyboard(Camera_Movement::FORWARD, dt);
        if (input->IsKeyDown(SDLK_s)) cam.ProcessKeyboard(Camera_Movement::BACKWARD, dt);

        if (input->IsKeyDown(SDLK_LSHIFT)) cam.ProcessKeyboard(Camera_Movement::DOWN, dt);
		if (input->IsKeyDown(SDLK_SPACE)) cam.ProcessKeyboard(Camera_Movement::UP, dt);

		frameTimeMetric.AddNewValue(dt);
		frameTimePlot.UpdateAxes();
	}

	virtual void Render() override
	{
		glm::mat4 skyboxView = glm::mat4(glm::mat3(cam.GetViewMatrix()));
		glm::mat4 proj = glm::perspective(glm::pi<float>() / 4.0f, (float)screen_width / (float)screen_height, 0.1f, 10000.f);


		renderer->RenderSkybox(skyboxView, proj);

		renderable->SetRenderBackface(true);
		renderable->SetModelMatrix(glm::mat4(1.f));
		renderable->SetViewMatrix(cam.GetViewMatrix());
		renderable->SetProjectionMatrix(proj);
		renderer->Render(*renderable);

		ImGui::Begin("Debug");
		{

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Text("Frame Info");
			ImGui::Separator();
			frameTimePlot.DrawHistory();
		}
		ImGui::End();
	}
};

#endif