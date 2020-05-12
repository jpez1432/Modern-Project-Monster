
#include "Render.hpp"


using namespace Engine;

CRender::CRender(void)
{
}

CRender::~CRender(void)
{
    if (QuadTree) {
        delete QuadTree;
        QuadTree = NULL;
        Logger->Text("Destroyed QuadTree Object");
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}

bool CRender::Create(Game::CPodPool &PodPool, Game::CTrack &Track, Physics::CVehicle &Vehicle)
{
    this->Track = &Track;
    this->Vehicle = &Vehicle;
    this->PodPool = &PodPool;

    MainShader.Create("Main.frag", "Main.vert", "Lighting.frag", "Fog.frag", NULL);
    ColorizeShader.Create("Colorize.frag", "Colorize.vert", NULL);
    ProjectionShader.Create("Projection.frag", "Projection.vert", NULL);
    OrthoShader.Create("Ortho.frag", "Ortho.vert", NULL);

    MainShader.Bind();
    MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    MainShader.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
    MainShader.SetUniform("sunLight.fAmbient", float(1.0f));
    MainShader.SetUniform("sunLight.vDirection", -glm::normalize(Track.GetSunPosition()));
    MainShader.SetUniform("gSampler", 0);

    MainShader.SetUniform("fogParams.vFogColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    MainShader.SetUniform("fogParams.fStart", Config->Graphics.FarClip * 0.5f);
    MainShader.SetUniform("fogParams.fEnd",  Config->Graphics.FarClip);
    MainShader.SetUniform("fogParams.fDensity", 0.01f);
    MainShader.SetUniform("fogParams.iEquation", 2);

    MainShader.SetUniform("bFog", 1);
    MainShader.SetUniform("bLighted", 1);
    MainShader.SetUniform("bTextured", 1);

    Weather.Create(PodPool, Config->Game.Weather, MainShader);

    QuadTree = new CQuadTree();
    if (QuadTree->Create(&Track, Track.Dimensions() / 8)) {
        Logger->Text("Created Terrain QuadTree");
    } else {
        Logger->Error("Error Creating Terrain QuadTree");
    }

    glm::vec3 WaterVertices[] = {
        glm::vec3(-255.0f, 0, -255.0f),
        glm::vec3(-255.0f, 0, 255.0f),
        glm::vec3(255.0f, 0, 255.0f),
        glm::vec3(255.0f, 0, -255.0f)
    };

    glm::vec2 WaterTexCoords[] = {
        glm::vec2(-255.0f, -255.0f),
        glm::vec2(-255.0f, 255.0f),
        glm::vec2(255.0f, 255.0f),
        glm::vec2(255.0f, -255.0f)
    };

    glm::vec3 Normal(0, -1, 0);

    glGenVertexArrays(1, &WaterVAO);
    glBindVertexArray(WaterVAO);
    WaterData.Create();
    WaterData.Bind();

    for (int i = 0; i < 4; i++) {
        WaterData.AddData(&WaterVertices[i], sizeof(glm::vec3));
        WaterData.AddData(&WaterTexCoords[i], sizeof(glm::vec2));
    }

    WaterData.DataToGPU(GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*) sizeof(glm::vec3));

    LogoTexture.LoadImage("Logo.png", "Game\\");

    glm::vec3 CubeVertices[] = {
        glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, -0.5f, 0.5f),
    };

    glm::vec2 CubeTexCoords[] = {glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };

    glGenVertexArrays(1, &LogoVAO);
    glBindVertexArray(LogoVAO);
    LogoData.Create();
    LogoData.Bind();

    for (int i = 0; i < 36; i++) {
        LogoData.AddData(&CubeVertices[i], sizeof(glm::vec3));
        LogoData.AddData(&CubeTexCoords[i % 6], sizeof(glm::vec2));
    }

    LogoData.DataToGPU(GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*) sizeof(glm::vec3));


    Camera.Create(Track.Dimensions(), Track.GetStartPosition(0));


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = &InterfaceFile.c_str()[0];
    ImGui_ImplGlfw_InitForOpenGL(GLWindow, true);
    ImGui_ImplOpenGL3_Init(NULL);

    ImGuiStyle& Style = ImGui::GetStyle();

    Style.WindowRounding = 2.0f;
    Style.FrameRounding = 2.0f;
    Style.ScrollbarRounding = 2.0f;
    Style.WindowBorderSize = 2.0f;
    Style.FrameBorderSize = 2.0f;

    Style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    Style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.75f);
    Style.Colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    Style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    Style.Colors[ImGuiCol_Border]                 = ImVec4(0.61f, 0.61f, 0.61f, 0.50f);
    Style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
    Style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.29f, 0.30f, 0.31f, 0.40f);
    Style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.49f, 0.46f, 0.46f, 0.67f);
    Style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.66f, 0.19f, 0.19f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    Style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    Style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    Style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.37f, 0.38f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.90f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Button]                 = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    Style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.84f, 0.17f, 0.17f, 1.00f);
    Style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Header]                 = ImVec4(0.52f, 0.52f, 0.52f, 0.31f);
    Style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.11f, 0.11f, 0.80f);
    Style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    Style.Colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    Style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
    Style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    Style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    Style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.49f, 0.49f, 0.49f, 0.67f);
    Style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.50f, 0.10f, 0.10f, 0.95f);
    Style.Colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.98f, 0.26f, 0.26f, 0.48f);
    Style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.57f, 0.57f, 0.57f, 0.90f);
    Style.Colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.27f, 0.27f, 1.00f);
    Style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    Style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    Style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    glAlphaFunc(GL_GEQUAL, 0.75f);
    glEnable(GL_ALPHA_TEST);

    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_NORMALIZE);

    Resize();

//    Vehicle.Reset();
    Config->General.Paused = false;

    return true;

}

void CRender::Scene(float Delta)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (Config->Input.Joystick) {
        Input.Update();
    }

    glm::mat4 LookAt = Camera.Update(Delta, Vehicle);
//    glLoadMatrixf(glm::value_ptr(LookAt));

//    Frustum.Update(Camera.Position, Camera.Center, Camera.Up);
    Frustum.UpdateFast(ProjectionMatrix, LookAt);

    MainShader.Bind();

    if (Config->General.EditMode) {

        MainShader.SetUniform("gSampler", 0);

        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        MainShader.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
        MainShader.SetUniform("sunLight.fAmbient", float(1.0f));
        MainShader.SetUniform("sunLight.vDirection", -glm::normalize(Track->GetSunPosition()));

        MainShader.SetUniform("bFog", 1);
        MainShader.SetUniform("fogParams.vFogColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        MainShader.SetUniform("fogParams.fStart", Config->Graphics.FarClip * 0.5f);
        MainShader.SetUniform("fogParams.fEnd",  Config->Graphics.FarClip);
        MainShader.SetUniform("fogParams.fDensity", 0.01f);
        MainShader.SetUniform("fogParams.iEquation", 2);

        if (Config->Game.Weather == 2) {
            MainShader.SetUniform("fogParams.fDensity", 0.1f);
        } else if (Config->Game.Weather == 3) {
            MainShader.SetUniform("fogParams.fDensity", 0.5f);
        } else {
            MainShader.SetUniform("fogParams.fDensity", 0.0f);
            MainShader.SetUniform("bFog", 0);
        }

        MainShader.SetUniform("bLighted", Config->Render.Lighted);
        MainShader.SetUniform("bTextured", Config->Render.Textured);

        Config->Render.Wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT, GL_FILL);
        Config->Render.Culled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

    } else {

        MainShader.SetUniform("gSampler", 0);

        MainShader.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        MainShader.SetUniform("sunLight.vColor", glm::vec3(0.85f, 0.85f, 0.85f));
        MainShader.SetUniform("sunLight.fAmbient", float(0.85f));
        MainShader.SetUniform("sunLight.vDirection", -glm::normalize(Track->GetSunPosition()));

        MainShader.SetUniform("bFog", 1);
        MainShader.SetUniform("fogParams.vFogColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        MainShader.SetUniform("fogParams.fStart", Config->Graphics.FarClip * 0.5f);
        MainShader.SetUniform("fogParams.fEnd",  Config->Graphics.FarClip);
        MainShader.SetUniform("fogParams.fDensity", 0.01f);
        MainShader.SetUniform("fogParams.iEquation", 2);

        if (Config->Game.Weather == 2) {
            MainShader.SetUniform("fogParams.fDensity", 0.1f);
        } else if (Config->Game.Weather == 3) {
            MainShader.SetUniform("fogParams.fDensity", 0.5f);
        } else {
            MainShader.SetUniform("fogParams.fDensity", 0.0f);
            MainShader.SetUniform("bFog", 0);
        }

        MainShader.SetUniform("bLighted", 1);
        MainShader.SetUniform("bTextured", 1);

        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_CULL_FACE);

    }


    if (Config->Render.Reflections && Config->Render.Water && Camera.Position.y > Track->GetWaterHeight()) {
        Reflections();
    }

    if (Config->Render.Backdrop) {
        Backdrop(false);
    }

    if (Config->Render.Water) {
        Water(Delta);
    }

    if (Config->Render.Terrain) {

        Weather.ApplyTint();

        for (int x = -Config->Render.Wrap; x <= Config->Render.Wrap; x++) {
            for (int z = -Config->Render.Wrap; z <= Config->Render.Wrap; z++) {

                if (glm::distance(glm::vec3(x * Track->Dimensions() + Track->Dimensions() / 2.0f, 0, z * Track->Dimensions() + Track->Dimensions() / 2.0f), Camera.Position) <  Track->Dimensions()) {

                    MainShader.SetUniform("modelViewMatrix", glm::translate(LookAt, glm::vec3(x * Track->Dimensions(), 0.0f, z * Track->Dimensions())));
                    Tree(QuadTree->GetRootNode(), x, z, false);
                }
            }
        }
    }

    if (Config->Render.Models) {
        Models(false);
    }

    Vehicle->Render(MainShader, LookAt, glm::vec3(0));//-glm::normalize(Track->GetSunPosition()));
    glCallList(PerspectiveView);
    Vehicle->RenderParts(LookAt);

    Interface();

    if (Config->General.Paused) {
        Logo(Delta);
    }

    glFlush();
    glfwSwapBuffers(GLWindow);

}

void CRender::Reflections(void)
{

//    glDisable(GL_DEPTH_TEST);
//    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//
//    glEnable(GL_STENCIL_TEST);
//    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
//    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
//
//    Water(0.0f);
//
//    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//    glEnable(GL_DEPTH_TEST);
//
//    glStencilFunc(GL_EQUAL, 1, 0xffffffff);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
//
//    if (Config->Render.Backdrop) {
//        Backdrop(true);
//    }
//
//    if (Config->Render.Models) {
//        Models(true);
//    }
//
////    Weather.ApplyTint();
//
//    glPushMatrix();
//
//    glTranslatef(0.0f, Camera.Position.y + Track->GetWaterHeight() * 2.0f, 0.0f);
//    Tree(QuadTree->GetRootNode(), 0, 0, true);
//
//    glPopMatrix();
//
//    glDisable(GL_STENCIL_TEST);

}

void CRender::Editor(float Delta)
{

}

void CRender::Tree(CNode *Node, int XOffset, int ZOffset, bool Invert)
{
    if (Node->Leaf) {

//        glm::vec3 Min = glm::vec3(Node->Bounds[0].x + float(XOffset * Track->Dimensions()), Vehicle->GetPosition().y, Node->Bounds[0].z + float(ZOffset * Track->Dimensions()));
//        glm::vec3 Max = glm::vec3(Node->Bounds[3].x + float(XOffset * Track->Dimensions()), Vehicle->GetPosition().y, Node->Bounds[3].z + float(ZOffset * Track->Dimensions()));
//
//        if (Frustum.BoxInFrustum(Min, Max) > CFrustum::Outside) {
        if (Frustum.SphereInFrustum(glm::vec3(Node->Center.x + XOffset * Track->Dimensions(), Vehicle->GetPosition().y, Node->Center.z + ZOffset * Track->Dimensions()), QuadTree->GetLeafSize()) > CFrustum::Outside) {
//        if (glm::distance(glm::vec3(Node->Center.x + XOffset * Track->Dimensions(), Node->Center.y, Node->Center.z + ZOffset * Track->Dimensions()), Camera.Position) <  Config->Graphics.FarClip) {

            Node->Render(Invert);

        }

    } else {

        for (int i = 0; i < 4; i++) {
            Tree(Node->Child[i], XOffset, ZOffset, Invert);
        }
    }
}

void CRender::Interface(void)
{

    glAlphaFunc(GL_GEQUAL, 0.5f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int Width, Height;
    glfwGetFramebufferSize(GLWindow, &Width, &Height);

    if (Config->General.Paused) {

        ImGui::Begin("Project Monster", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::SetWindowSize(ImVec2(300, 320), 0);
        ImGui::SetWindowPos(ImVec2(float ((Width / 2) - 150), (float (Height / 2) - Height / 6)));

        ImGui::PushItemWidth(230);

        if (ImGui::CollapsingHeader("General")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            if (ImGui::Button("Load Defaults... ")) {

                if (MessageBox(NULL, "Defaults Will Be Loaded And Restart Required, Would You Like To Continue?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                    Config->Defaults();
                    Loop = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Restart...")) {
                Loop = false;
            }

            ImGui::Checkbox("Edit Mode", &Config->General.EditMode);
            ImGui::SameLine();
            ImGui::Checkbox("Physics Mode", &Config->General.PhysicsMode);

            ImGui::Checkbox("Debug Mode", &Config->General.DebugMode);
            ImGui::SameLine();
            ImGui::Checkbox("Fps Counter", &Config->General.FpsCounter);

        }

        if (ImGui::CollapsingHeader("Game")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            ImGui::Checkbox("Use Pod Files", &Config->Game.Pods);
            ImGui::SameLine();
            ImGui::Checkbox("Use Loose Files", &Config->Game.Files);

            ImGui::Text("Pod INI File - %s", Config->Game.PodIni);
            ImGui::SameLine();

            if (ImGui::Button("...")) {
                std::string File = FileOpen(NULL, (char*)("Pod INI Files\0*.ini\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".ini")[0]);

                if (File.size() != 0) {
                    strcpy(Config->Game.PodIni, File.substr(File.find_last_of("\\") + 1).c_str());

                    if (MessageBox(NULL, "Changes Require A Restart, Would You Like To Continue?", "Restart", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Loop = false;
                    }
                }
            }

            static int Index = 0;
            ImGui::SliderInt("Weather", &Config->Game.Weather, 0, 7);

            if (Config->Game.Weather != Index) {
                Weather.Create(*PodPool, Config->Game.Weather, MainShader);
                Index = Config->Game.Weather;
            }

            ImGui::SliderInt("Scenery", &Config->Game.Scenery, 0, 3);

            static int LastTruck = 0;
            static int ComboBox_TruckItem = 0;

            for (unsigned int i = 0; i < PodPool->NumTrucks(); i++) {
                if (StrCompare(&PodPool->TruckFilename(i), Config->Game.Vehicle)) {
                    ComboBox_TruckItem = i;
                    LastTruck = ComboBox_TruckItem;
                    continue;
                }
            }

            static char **TruckItems = new char*[PodPool->NumTrucks()];

            for (unsigned int i = 0; i < PodPool->NumTrucks(); i++) {
//                TruckItems[i] = &PodPool->TruckFilename(i);
                TruckItems[i] = &PodPool->TruckName(i);
            }


            ImGui::Combo("Truck", &ComboBox_TruckItem, TruckItems, PodPool->NumTrucks(), 6);


            if (ComboBox_TruckItem != LastTruck) {

                LastTruck = ComboBox_TruckItem;

                strcpy(Config->Game.Vehicle, &PodPool->TruckFilename(ComboBox_TruckItem));

//                Loop = false;

                if (Vehicle) {

                    Physics::CServer::GetInstance()->UnregisterClient(Vehicle);

                    delete Vehicle;
                    Vehicle = new Physics::CVehicle();
                    Physics::CServer::GetInstance()->RegisterClient(Vehicle);

                    if (Vehicle->LoadTruck(*PodPool, Config->Game.Vehicle)) {
                        Logger->Text("Loaded Truck - " + std::string(Config->Game.Vehicle));
                    } else {
                        Logger->Error("Error Loading Truck - " + std::string(Config->Game.Vehicle));
                    }

                    if (Vehicle->LoadPhysics(Config->Physics.VehicleFile)) {
                        Logger->Text("Loaded Vehicle Physics");
                    } else {
                        Logger->Error("Error Loading Vehicle Physics");
                    }

                    if (Vehicle->Create(Track->GetStartPosition(0), Track->GetStartRotation(0), Track->Dimensions())) {
                        Logger->Text("Created ODE Physics Vehicle");
                    } else {
                        Logger->Error("Error Creating ODE Physics Vehicle");
                    }

                    Config->General.Paused = false;
                }

            }

            static int LastTrack = 0;
            static int ComboBox_TrackItem = 0;

            for (unsigned int i = 0; i < PodPool->NumTracks(); i++) {
                if (StrCompare(&PodPool->TrackFilename(i), Config->Game.Track)) {
                    ComboBox_TrackItem = i;
                    LastTrack = ComboBox_TrackItem;
                    continue;
                }
            }

            static char **TrackItems = new char*[PodPool->NumTracks()];

            for (unsigned int i = 0; i < PodPool->NumTracks(); i++) {
//                TrackItems[i] = &PodPool->TrackFilename(i);
                TrackItems[i] = &PodPool->TrackName(i);
            }

            ImGui::Combo("Track", &ComboBox_TrackItem, TrackItems, PodPool->NumTracks(), 6);

//            ImGui::SameLine();
//            ImGui::Text("(Restart)");

            if (ComboBox_TrackItem != LastTrack) {

                LastTrack = ComboBox_TrackItem;

//                strcpy(Config->Game.Track, TrackItems[ComboBox_TrackItem]);
                strcpy(Config->Game.Track, &PodPool->TrackFilename(ComboBox_TrackItem));

                Loop = false;

//                if (Track) {
//
//                    delete Track;
//                    Track = new Game::CTrack();
//                    if (Track->Load(*PodPool, Config->Game.Track)) {
//                        Logger->Text("Loaded Track - " + std::string(Config->Game.Track));
//                    } else {
//                        Logger->Error("Error Loading Track - " + std::string(Config->Game.Track));
//                    }
//
//                    // Physics::CServer::GetInstance()->Destroy();
//                    if (Physics::CServer::GetInstance()->SetTerrain(Track->TerrainObject())) {
//                        Logger->Text("Created ODE Physics Server");
//                    } else {
//                        Logger->Error("Error Creating ODE Physics Server");
//                    }
//
////                    // FIX /////////////
////                    QuadTree->Destroy();
////                    ////////////////////
////
//                    delete QuadTree;
//                    QuadTree = new CQuadTree();
//                    if (QuadTree->Create(Track, Track->Dimensions() / 8 )) {//, true) {
//                        Logger->Text("Created Terrain QuadTree");
//                    } else {
//                        Logger->Error("Error Creating Terrain QuadTree");
//                    }
//
//                    Vehicle->SetPosition(Track->GetStartPosition(0));
////                    Vehicle->SetOrientation(Track->GetStartPosition(0), Track->GetStartRotation(0));
//
//                    Config->General.Paused = false;
//                }

            }

        }

        if (ImGui::CollapsingHeader("Help")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            ImGui::Text("Escape - Pause");
            ImGui::Text("R - Reset Truck Start");
            ImGui::Text("F - Flip Truck Upright");
            ImGui::Text("C - Cycle Camera Modes");
            ImGui::Text("Scroll Wheel - Zoom");
            ImGui::Text("Alt + Enter - Fullscreen");
            ImGui::Text("Ctrl + Q - Exit");
            ImGui::Text("Ctrl + V - Toggle VSync");
            ImGui::Text("Ctrl + P - Physics Mode");
            ImGui::Text("Ctrl + E - Editor Mode");
            ImGui::Text("Ctrl + D - Debug Info");
            ImGui::Text("Ctrl + F - Fps Counter");
            ImGui::Text("Ctrl + W - Wrap Track");
            ImGui::Text("F1 - Culling");
            ImGui::Text("F2 - Lighting");
            ImGui::Text("F3 - Texturing");
            ImGui::Text("F4 - Wireframe");
            ImGui::Text("F5 - Terrain");
            ImGui::Text("F6 - Boxes");
            ImGui::Text("F7 - Models");
            ImGui::Text("F8 - Water");
            ImGui::Text("F9 - Backdrop");
            ImGui::Text("F10 - Reflections");
            ImGui::Text("W - Gas");
            ImGui::Text("S - Brake");
            ImGui::Text("A - Steer Left");
            ImGui::Text("D - Steer Right");
            ImGui::Text("Q - Down Shift");
            ImGui::Text("E - Up Shift");

        }

        if (ImGui::CollapsingHeader("Render")) {

//            ImGui::Text("Edit Mode");
//            ImGui::Separator();

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            ImGui::Checkbox("Wrap Track", &Config->Render.Wrap);
            ImGui::SameLine();
            ImGui::Checkbox("Wireframe", &Config->Render.Wireframe);

            ImGui::Checkbox("Textured  ", &Config->Render.Textured);
            ImGui::SameLine();
            ImGui::Checkbox("Lighting", &Config->Render.Lighted);

            ImGui::Checkbox("Culling   ", &Config->Render.Culled);
            ImGui::SameLine();
            ImGui::Checkbox("Reflections", &Config->Render.Reflections);

            ImGui::Checkbox("Terrain   ", &Config->Render.Terrain);
            ImGui::SameLine();
            ImGui::Checkbox("Boxes", &Config->Render.Boxes);

            ImGui::Checkbox("Models    ", &Config->Render.Models);
            ImGui::SameLine();
            ImGui::Checkbox("Water", &Config->Render.Water);

            ImGui::Checkbox("Backdrop", &Config->Render.Backdrop);

        }

        if (ImGui::CollapsingHeader("Graphics")) {

            static bool Restart = false;

            float DMin = 0.0f, DMax = 0.0f;
            unsigned int IMin = 0, IMax = 0;

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 1);

            ImGui::PushItemWidth(160);

            ImGui::Checkbox("Fullscreen      ", &Config->Graphics.Fullscreen);
            ImGui::SameLine();
            ImGui::Checkbox("Vertical Sync", &Config->Graphics.VSync);

            static int ComboBox_ResItem = 0;
            static bool LoadRes = false;

            if (LoadRes == false) {
                LoadRes = true;

                if (Config->Graphics.Width == 640 && Config->Graphics.Height == 480) {
                    ComboBox_ResItem = 0;
                }

                if (Config->Graphics.Width == 800 && Config->Graphics.Height == 600) {
                    ComboBox_ResItem = 1;
                }

                if (Config->Graphics.Width == 900 && Config->Graphics.Height == 600) {
                    ComboBox_ResItem = 2;
                }

                if (Config->Graphics.Width == 1024 && Config->Graphics.Height == 768) {
                    ComboBox_ResItem = 3;
                }

                if (Config->Graphics.Width == 1440 && Config->Graphics.Height == 900) {
                    ComboBox_ResItem = 4;
                }

                if (Config->Graphics.Width == 1600 && Config->Graphics.Height == 900) {
                    ComboBox_ResItem = 5;
                }

                if (Config->Graphics.Width == 1600 && Config->Graphics.Height == 1200) {
                    ComboBox_ResItem = 6;
                }

                if (Config->Graphics.Width == 1680 && Config->Graphics.Height == 1050) {
                    ComboBox_ResItem = 7;
                }

                if (Config->Graphics.Width == 1920 && Config->Graphics.Height == 1080) {
                    ComboBox_ResItem = 8;
                }

            }

            const char* ResItems[] = { "640x480", "800x600", "900x600", "1024x768", "1440x900", "1600x900", "1600x1200", "1680x1050", "1920x1080" };

            ImGui::Combo("Resolution", &ComboBox_ResItem, ResItems, IM_ARRAYSIZE(ResItems), 8);

            static int Count, PosX, PosY;
            static GLFWmonitor** Monitors = glfwGetMonitors(&Count);

            IMin = 0;
            IMax = Count - 1;
            ImGui::DragScalar("Monitor", ImGuiDataType_U32, &Config->Graphics.Monitor, 1, &IMin, &IMax, "%i");

            DMin = 16;
            DMax = 256;
            static int LastClip = Config->Graphics.FarClip;
            ImGui::DragScalar("Draw Distance", ImGuiDataType_Float, &Config->Graphics.FarClip, 1, &DMin, &DMax, "%.1f");

            if (Config->Graphics.FarClip != LastClip) {
                LastClip = Config->Graphics.FarClip;
                Resize();
            }

            DMin = 0.0f;
            DMax = 120.0f;
            static float LastFOV = Config->Graphics.FieldOfVision;
            ImGui::DragScalar("Field Of Vision", ImGuiDataType_Float, &Config->Graphics.FieldOfVision, 1.0f, &DMin, &DMax, "%.1f");

            if (LastFOV != Config->Graphics.FieldOfVision) {
                LastFOV = Config->Graphics.FieldOfVision;
                Resize();
            }

            IMin = 0;
            IMax = 2;
            static int LastTexture = Config->Graphics.TextureQuality;
            ImGui::DragScalar("Texture Quality", ImGuiDataType_U32, &Config->Graphics.TextureQuality, 1, &IMin, &IMax, "%i");

            if (LastTexture != Config->Graphics.TextureQuality) {
                LastTexture = Config->Graphics.TextureQuality;
                Restart = true;
            }

            IMin = 2;
            IMax = 16;
            int Next = pow(2, ceil(log(Config->Graphics.Antialiasing) / log(2)));
            static int LastAA = Config->Graphics.Antialiasing;
            ImGui::DragScalar("Antialiasing", ImGuiDataType_U32, &Config->Graphics.Antialiasing, Next, &IMin, &IMax, "%i");

            if (LastAA != Config->Graphics.Antialiasing) {
                LastAA = Config->Graphics.Antialiasing;
                Restart = true;
            }


            static bool LastMultiSample = Config->Graphics.Multisample;
            ImGui::Checkbox("Multisampling   ", &Config->Graphics.Multisample);

            if (LastMultiSample != Config->Graphics.Multisample) {
                LastMultiSample = Config->Graphics.Multisample;

                if (Config->Graphics.Multisample) {
                    glEnable(GL_MULTISAMPLE);
                } else {
                    glDisable(GL_MULTISAMPLE);
                }
            }

            ImGui::SameLine();

            static bool LastDoubleBuffer = Config->Graphics.DoubleBuffer;
            ImGui::Checkbox("Double Buffer", &Config->Graphics.DoubleBuffer);

            if (LastDoubleBuffer != Config->Graphics.DoubleBuffer) {
                LastDoubleBuffer = Config->Graphics.DoubleBuffer;
                Restart = true;
            }

            if (ImGui::Button("Apply Changes... ")) {

                if (Restart) {
                    if (MessageBox(NULL, "Changes Require A Restart, Would You Like To Continue?", "Restart", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Loop = false;
                    }
                } else {

                    if (Config->Graphics.Multisample) {
                        glEnable(GL_MULTISAMPLE);
                    } else {
                        glDisable(GL_MULTISAMPLE);
                    }

                    std::string Resolution = std::string(ResItems[ComboBox_ResItem]);
                    Config->Graphics.Width = atoi(Resolution.substr(0, Resolution.find_first_of("x")).c_str());
                    Config->Graphics.Height = atoi(Resolution.substr(Resolution.find_last_of("x") + 1, Resolution.length()).c_str());

                    if (Config->Graphics.Fullscreen) {

                        glfwSetWindowMonitor(GLWindow, Monitors[Config->Graphics.Monitor], 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);

                    } else {

                        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
                        glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);

                        const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[Config->Graphics.Monitor]);
                        glfwGetMonitorPos(Monitors[Config->Graphics.Monitor], &PosX, &PosY);
                        glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));

                    }

                    glfwSwapInterval(Config->Graphics.VSync);

                }

            }

//            ImGui::SameLine();
//            if (ImGui::Button("Load Defaults... ")) {
//
//                if (MessageBox(NULL, "Defaults Will Be Loaded And Restart Required, Would You Like To Continue?", "Defaults", MB_ICONEXCLAMATION | MB_YESNO) == IDYES) {
//                    Config->Defaults(ConfigFile);
//                    Loop = false;
//                }
//            }

        }

        ImGui::End();

    }

    if (Config->General.DebugMode && !Config->General.Paused) {

        ImGui::Begin("Debug Info", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

        ImGui::SetWindowSize(ImVec2(246, 236));
        ImGui::SetWindowPos(ImVec2(Width - 250, 4));

        ImGui::Text("Gear - %i", Vehicle->GetGear());
        ImGui::Text("Speed - %0.2f", Vehicle->GetSpeed());
        ImGui::Text("Rpms - %0.2f", Vehicle->GetRpms() + 1000.0f);
        ImGui::Text("Vehicle - %0.2f, %0.2f, %0.2f", Vehicle->GetPosition()[0], Vehicle->GetPosition()[1], Vehicle->GetPosition()[2]);

        ImGui::Separator();

        switch (Config->Camera.Mode) {

        case 0:
            ImGui::Text("Camera - Cockpit");
            break;

        case 1:
            ImGui::Text("Camera - Chase");
            break;

        case 2:
            ImGui::Text("Camera - Race");
            break;

        case 3:
            ImGui::Text("Camera - Free");
            break;

        case 4:
            ImGui::Text("Camera - Orbit");
            break;

        }

        ImGui::Separator();

        ImGui::Text("Position - %0.2f, %0.2f, %0.2f", Camera.Position.x, Camera.Position.y, Camera.Position.z);
        ImGui::Text("View - %0.2f, %0.2f, %0.2f", Camera.Center.x, Camera.Center.y, Camera.Center.z);
        ImGui::Text("Up - %0.2f, %0.2f, %0.2f", Camera.Up.x, Camera.Up.y, Camera.Up.z);
        ImGui::Text("Pitch - %0.2f, Yaw - %0.2f", Camera.Pitch, Camera.Yaw);
        ImGui::Text("Angle - %0.2f", Camera.Angle);

        if (Config->General.FpsCounter) {
            ImGui::Separator();
            ImGui::Text("Fps - %i", Fps);
        }

        ImGui::End();
    }



    if (Config->General.PhysicsMode && !Config->General.Paused) {

        ImGui::Begin("Physics Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SetWindowSize(ImVec2(230, Height - 8), 0);
        ImGui::SetWindowPos(ImVec2(4, 4));

        ImGui::PushItemWidth(150);

        double DMin = 0.0f, DMax = 0.0f;
        unsigned int IMin = 0, IMax = 0;

        if (ImGui::CollapsingHeader("World        ")) {

            if (ImGui::Button("  Load  ")) {
                std::string File = FileOpen(NULL, (char*)("Physics World Files\0*.world\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".world")[0]);

                if (File.size() != 0) {
                    Physics::CServer::GetInstance()->LoadPhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("  Save  ")) {
                std::string File = FileSave(NULL, (char*)("Physics World Files\0*.world\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".world")[0]);

                if (File.size() != 0) {
                    Physics::CServer::GetInstance()->SavePhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Defaults ")) {
                Physics::CServer::GetInstance()->Defaults("Default.world");
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("General")) {

                ImGui::Checkbox("Quick Step", &Physics::CServer::GetInstance()->World.QuickStep);
                ImGui::Checkbox("Variable Step", &Physics::CServer::GetInstance()->World.VariableStep);

                DMin = 0.0001f;
                DMax = 100.0f;
                ImGui::DragScalar("Mass Scale", ImGuiDataType_Double, &Physics::CServer::GetInstance()->World.MassScale, 0.100f, &DMin, &DMax, "%.3f", 1.0f);

                DMin = 0.0001f;
                DMax = 5.0f;
                ImGui::DragScalar("Step Size", ImGuiDataType_Double, &Physics::CServer::GetInstance()->World.StepSize, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                IMin = 1;
                IMax = 120;
                ImGui::DragScalar("Iterations", ImGuiDataType_U32, &Physics::CServer::GetInstance()->World.NumIterations, 1, &IMin, &IMax, "%i", 1.0f);

                DMin = -25.0f;
                DMax = 25.0f;
                ImGui::DragScalar("Gravity", ImGuiDataType_Double, &Physics::CServer::GetInstance()->World.Gravity, 0.01f, &DMin, &DMax, "%.2f", 1.0f);

                DMin = 0.0f;
                DMax = 1.0f;
                ImGui::DragScalar("ERP", ImGuiDataType_Double, &Physics::CServer::GetInstance()->World.ERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("CFM", ImGuiDataType_Double, &Physics::CServer::GetInstance()->World.CFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("Slip", ImGuiDataType_Double, &Physics::CServer::GetInstance()->World.Slip, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

            }

            if (ImGui::CollapsingHeader("Collisions")) {

                ImGui::PushItemWidth(85);

                ImGui::Text("Auto Disable Parameters");

                ImGui::Checkbox("Automatic Disable", &Physics::CServer::GetInstance()->Collisions.AutoDisableFlag);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Linear Threshold", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.AutoDisableLinearThreshold, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Angular Threshold", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.AutoDisableAngularThreshold, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                IMin = 0;
                IMax = 60;
                ImGui::DragScalar("Steps", ImGuiDataType_U32, &Physics::CServer::GetInstance()->Collisions.AutoDisableSteps, 1, &IMin, &IMax, "%i", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Time", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.AutoDisableTime, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                ImGui::Text("General Parameters");

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Bounce", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.Bounce, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Bounce Velocity", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.BounceVelocity, 0.001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0f;
                DMax = 1.0f;
                ImGui::DragScalar("Soft ERP", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.SoftERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("Soft CFM", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Collisions.SoftCFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);


            }

            ImGui::PushItemWidth(70);

            if (ImGui::CollapsingHeader("Damping")) {

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Linear", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Damping.LinearDamping, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Linear Threshold", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Damping.LinearDampingThreshold, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Angular", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Damping.AngularDamping, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Angular Threshold", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Damping.AngularDampingThreshold, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

            }

            if (ImGui::CollapsingHeader("Contacts")) {

                IMin = 0;
                IMax = 40;
                ImGui::DragScalar("Contacts", ImGuiDataType_U32, &Physics::CServer::GetInstance()->Contacts.NumContacts, 1, &IMin, &IMax, "%i", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Correcting Velocity", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Contacts.MaxCorrectingVelocity, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Surface Layer", ImGuiDataType_Double, &Physics::CServer::GetInstance()->Contacts.SurfaceLayer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

            }

            Physics::CServer::GetInstance()->SetPhysics();
        }

        ImGui::PushItemWidth(80);

        ImGui::Separator();

        if (ImGui::CollapsingHeader("Vehicle      ")) {

            static bool AxleCommons = false;
            static bool WheelCommons = false;

            if (ImGui::Button("  Load ")) {
                std::string File = FileOpen(NULL, (char*)("Physics Vehicle Files\0*.vehicle\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".vehicle")[0]);

                if (File.size() != 0) {
                    Vehicle->LoadPhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("  Save  ")) {
                std::string File = FileSave(NULL, (char*)("Physics Vehicle Files\0*.vehicle\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".vehicle")[0]);

                if (File.size() != 0) {
                    Vehicle->SavePhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if (ImGui::Button(" Defaults")) {
                Vehicle->Defaults("Default.vehicle");
            }

            ImGui::Separator();

            if (ImGui::CollapsingHeader("General ")) {


                DMin = 0.0001f;
                DMax = 100.0f;
                ImGui::DragScalar("Mass Scale ", ImGuiDataType_Double, &Vehicle->Vehicle.MassScale, 0.100f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 1.0f;
                DMax = 20000.0f;
                ImGui::DragScalar("Chassis/Body Mass", ImGuiDataType_Double, &Vehicle->Vehicle.Mass, 10.0f, &DMin, &DMax, "%.00f", 1.0f);

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("Slip ", ImGuiDataType_Double, &Vehicle->Vehicle.Data.Slip, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);
            }

            ImGui::Checkbox("Link Axle Common Attributes", &AxleCommons);

            if (AxleCommons) {

                DMin = 1.0f;
                DMax = 20000.0f;
                ImGui::DragScalar("Axles Mass", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);
                Vehicle->Vehicle.Axles[1].Mass = Vehicle->Vehicle.Axles[0].Mass;

                DMin = -1.0f;
                DMax = 1.0f;
                ImGui::DragScalar("Minimum Steering", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].MinSteer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);
                Vehicle->Vehicle.Axles[1].MinSteer = Vehicle->Vehicle.Axles[0].MinSteer;

                DMin = -1.0f;
                DMax = 1.0f;
                ImGui::DragScalar("Maximum Steering", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].MaxSteer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);
                Vehicle->Vehicle.Axles[1].MaxSteer = Vehicle->Vehicle.Axles[0].MaxSteer;

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Steering Speed", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].SteerSpeed, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);
                Vehicle->Vehicle.Axles[1].SteerSpeed = Vehicle->Vehicle.Axles[0].SteerSpeed;

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("Slip ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Data.Slip, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);
                Vehicle->Vehicle.Axles[1].Data.Slip = Vehicle->Vehicle.Axles[0].Data.Slip;

            } else {

                if (ImGui::CollapsingHeader("Front Axle")) {

                    DMin = 1.0f;
                    DMax = 20000.0f;
                    ImGui::DragScalar("Axle Mass", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Minimum Steering ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].MinSteer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Maximum Steering ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].MaxSteer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Steering Speed ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].SteerSpeed, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Slip   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Data.Slip, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                }

                if (ImGui::CollapsingHeader("Rear Axle")) {

                    DMin = 1.0f;
                    DMax = 20000.0f;
                    ImGui::DragScalar("Axle Mass ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Minimum Steering  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].MinSteer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Maximum Steering  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].MaxSteer, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Steering Speed  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].SteerSpeed, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Slip    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Data.Slip, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);


                }
            }

            ImGui::Checkbox("Link Wheel Common Attributes", &WheelCommons);

            if (WheelCommons) {

                DMin = 1.0f;
                DMax = 20000.0f;
                ImGui::DragScalar("Wheels Mass", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                DMin = 0.000000028f;
                DMax = 1.0f;
                ImGui::DragScalar("Damping ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Damping, 0.00000000001f, &DMin, &DMax, "%.9f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Error", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Error, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Stop ERP", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].StopERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("Stop CFM", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("Bounce", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Bounce, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.00000001f;
                DMax = 1.0f;
                ImGui::DragScalar("CFM  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].CFM, 0.0001f, &DMin, &DMax, "%.8f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("FMax", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].FMax, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                DMin = 0.0001f;
                DMax = 1.0f;
                ImGui::DragScalar("FudgeFactor", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                for (unsigned int i = 0; i < 2; i++) {
                    for (unsigned int j = 0; j < 2; j++) {

                        Vehicle->Vehicle.Axles[i].Wheels[j].Mass = Vehicle->Vehicle.Axles[0].Wheels[0].Mass;
                        Vehicle->Vehicle.Axles[i].Wheels[j].Damping = Vehicle->Vehicle.Axles[0].Wheels[0].Damping;
                        Vehicle->Vehicle.Axles[i].Wheels[j].Error = Vehicle->Vehicle.Axles[0].Wheels[0].Error;
                        Vehicle->Vehicle.Axles[i].Wheels[j].StopERP = Vehicle->Vehicle.Axles[0].Wheels[0].StopERP;
                        Vehicle->Vehicle.Axles[i].Wheels[j].StopCFM = Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM;
                        Vehicle->Vehicle.Axles[i].Wheels[j].Bounce = Vehicle->Vehicle.Axles[0].Wheels[0].Bounce;
                        Vehicle->Vehicle.Axles[i].Wheels[j].CFM = Vehicle->Vehicle.Axles[0].Wheels[0].CFM;
                        Vehicle->Vehicle.Axles[i].Wheels[j].FMax = Vehicle->Vehicle.Axles[0].Wheels[0].FMax;
                        Vehicle->Vehicle.Axles[i].Wheels[j].FudgeFactor = Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor;

                    }
                }

            } else {

                if (ImGui::CollapsingHeader("Front Left Wheel")) {

                    DMin = 1.0f;
                    DMax = 20000.0f;
                    ImGui::DragScalar("Wheel Mass", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                    DMin = 0.000000028f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Damping   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Damping, 0.00000000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Error ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Error, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop ERP ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].StopERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop CFM ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Bounce ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].Bounce, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("CFM   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].CFM, 0.0001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FMax  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].FMax, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FudgeFactor  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Steer Multiplier", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].SteerMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Drive Multiplier", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[0].DriveMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                }

                if (ImGui::CollapsingHeader("Front Right Wheel")) {

                    DMin = 1.0f;
                    DMax = 20000.0f;
                    ImGui::DragScalar("Wheel Mass ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                    DMin = 0.000000028f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Damping  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].Damping, 0.00000000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Error  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].Error, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop ERP  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].StopERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop CFM  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].StopCFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Bounce  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].Bounce, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("CFM   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].CFM, 0.0001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FMax   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].FMax, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FudgeFactor   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].FudgeFactor, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Steer Multiplier ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].SteerMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Drive Multiplier ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[0].Wheels[1].DriveMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                }

                if (ImGui::CollapsingHeader("Rear Left Wheel")) {

                    DMin = 1.0f;
                    DMax = 20000.0f;
                    ImGui::DragScalar("Wheel Mass  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                    DMin = 0.000000028f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Damping   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].Damping, 0.00000000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Error   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].Error, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop ERP   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].StopERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop CFM   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].StopCFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Bounce   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].Bounce, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("CFM    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].CFM, 0.0001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FMax   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].FMax, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FudgeFactor   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].FudgeFactor, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Steer Multiplier  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].SteerMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Drive Multiplier  ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[0].DriveMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                }

                if (ImGui::CollapsingHeader("Rear Right Wheel")) {

                    DMin = 1.0f;
                    DMax = 20000.0f;
                    ImGui::DragScalar("Wheel Mass   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].Mass, 10.0f, &DMin, &DMax, "%.2f", 1.0f);

                    DMin = 0.000000028f;
                    DMax = 1.0f;
                    ImGui::DragScalar(" Damping    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].Damping, 0.00000000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Error    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].Error, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop ERP    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].StopERP, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Stop CFM    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].StopCFM, 0.00000001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Bounce    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].Bounce, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.00000001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("CFM     ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].CFM, 0.0001f, &DMin, &DMax, "%.8f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FMax    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].FMax, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = 0.0001f;
                    DMax = 1.0f;
                    ImGui::DragScalar("FudgeFactor    ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].FudgeFactor, 0.0001f, &DMin, &DMax, "%.4f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Steer Multiplier   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].SteerMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);

                    DMin = -1.0f;
                    DMax = 1.0f;
                    ImGui::DragScalar("Drive Multiplier   ", ImGuiDataType_Double, &Vehicle->Vehicle.Axles[1].Wheels[1].DriveMultiplier, 1.0f, &DMin, &DMax, "%.0f", 1.0f);


                }
            }

            if (ImGui::CollapsingHeader("Transmission")) {



            }

            Vehicle->SetPhysics();
        }

        ImGui::End();

    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glAlphaFunc(GL_GEQUAL, 0.75f);

}

void CRender::Logo(float Delta)
{
    glPolygonMode(GL_FRONT, GL_FILL);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    static float Rotation = 0.0f;
//    Rotation += 1.0f * Delta;

    glm::mat4 ModelView = glm::lookAt(glm::vec3(0.0f, -0.5f, 2.0f), glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ModelView = glm::translate(ModelView, glm::vec3(0.0f, 0.0f, 0.0f));
    ModelView = glm::scale(ModelView, glm::vec3(1.0f, 1.0f, 1.0f));
    ModelView = glm::rotate(ModelView, Rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    MainShader.Bind();
    MainShader.SetUniform("modelViewMatrix", ModelView);
    MainShader.SetUniform("bLighted", 0);

    LogoTexture.Bind(0);

    glBindVertexArray(LogoVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glCullFace(GL_BACK);
}

void CRender::Water(float Delta)
{

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    MainShader.Bind();
    MainShader.SetUniform("modelViewMatrix", glm::translate(Camera.LookAt(), glm::vec3(Track->Dimensions() / 2.0f, Track->GetWaterHeight(), Track->Dimensions() / 2.0f)));
    MainShader.SetUniform("bLighted", 0);

    Weather.ApplyWaterTint();

    Track->GetWaterTexture()->Bind(0);

    glBindVertexArray(WaterVAO);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);

    glCullFace(GL_BACK);

}

void CRender::Backdrop(bool Invert)
{

    if (Config->Game.Weather == 2 || Config->Game.Weather == 3) {
        return;
    }

    Weather.ApplyBackdropTint();

    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_CLIP_PLANE0);

    glm::mat4 Matrix(0);
    Matrix = glm::translate(Camera.LookAt(), glm::vec3(Camera.Position.x, Camera.Position.y - (16.0f / 256.0f), Camera.Position.z));

    if (Invert) {

        glCullFace(GL_FRONT);

        Matrix = glm::scale(Matrix, glm::vec3(1.0, -1.0, 1.0));
        MainShader.SetUniform("modelViewMatrix", Matrix);

//        GLdouble PlaneEqn[] = { 0.0f, -1.0f, 0.0f, 0.0f };
//        glClipPlane(GL_CLIP_PLANE0, PlaneEqn);

        Weather.DrawSkyDome();
        Track->DrawBackground(0);
        Track->DrawBackground(1);

    } else {

        MainShader.SetUniform("modelViewMatrix", Matrix);

//        glCullFace(GL_BACK);

//        GLdouble PlaneEqn[] = { 0.0f, 1.0f, 0.0f, 0.0f };
//        glClipPlane(GL_CLIP_PLANE0, PlaneEqn);

        Weather.DrawSkyDome();
        Track->DrawBackground(0);
        Track->DrawBackground(1);

    }

//    glDisable(GL_CLIP_PLANE0);
    glEnable(GL_DEPTH_TEST);

}


void CRender::Models(bool Invert)
{
    glm::mat4 Orientation;
    glm::mat4 LookAt = Camera.LookAt();

    Weather.ApplyTint();

    for (int i = 0; i < Track->NumModels(); i++) {

        if (Track->GetModelPriority(i) >= (int)Config->Game.Scenery) {
            continue;
        }

        if (Track->GetModelTypeFlags(i) == "6,0") {
            continue;
        }

        glm::vec3 Position = Track->GetModelPosition(i);
        glm::vec3 Rotation = Track->GetModelRotation(i);

        if (Frustum.SphereInFrustum(Position, Track->GetModelBounds(i)) > CFrustum::Outside) {
//        if (glm::distance(Position, Camera.Position) <  Config->Graphics.FarClip) {

            if (Invert) {
                Orientation = glm::translate(LookAt, glm::vec3(Position.x, Track->GetWaterHeight(), Position.z));
            } else {
                Orientation = glm::translate(LookAt, Track->GetModelPosition(i));
            }

            if (Track->GetModelTypeFlags(i) != "8,0" && Track->GetModelTypeFlags(i) != "9,0") {

                Orientation = glm::rotate(Orientation, glm::radians(270.0f) - Rotation.x, glm::vec3(0, 1, 0));

            } else {

                double Theta = 0.0f;

                if (Position.z - Camera.Position.z == 0.0f) {

                    if (Position.x < Camera.Position.x) {
                        Theta = (glm::pi<float>());
                    }

                } else {

                    Theta = atan((Position.x - Camera.Position.x) / (Position.z - Camera.Position.z));

                    if (Position.z > Camera.Position.z) {
                        Theta = (glm::pi<float>()) / 2.0f - Theta;
                    } else {
                        Theta = (glm::pi<float>()) * 1.5f - Theta;
                    }

                }

                Orientation = glm::rotate(Orientation, glm::radians(270.0f) - float(Theta), glm::vec3(0, 1, 0));

            }

            Orientation = glm::rotate(Orientation, -Rotation.z, glm::vec3(1, 0, 0));
            Orientation = glm::rotate(Orientation, Rotation.y, glm::vec3(0, 0, 1));

            if (Invert) {

                glCullFace(GL_FRONT);

                GLdouble PlaneEqn[] = { 0.0f, -1.0f, 0.0f, 0.0f };
                glClipPlane(GL_CLIP_PLANE0, PlaneEqn);
                glEnable(GL_CLIP_PLANE0);

                glScalef(1.0f, -2.0f, 1.0f);

                Track->DrawModel(i);

                glDisable(GL_CLIP_PLANE0);

            } else {

                glCullFace(GL_BACK);

                MainShader.SetUniform("modelViewMatrix", Orientation);
                Track->DrawModel(i);
            }

//            glPopMatrix();
        }
    }

}

void CRender::Resize(void)
{

    if (glfwGetWindowAttrib(GLWindow, GLFW_ICONIFIED)) {
        return;
    }

    int Width, Height;
    glfwGetFramebufferSize(GLWindow, &Width, &Height);



    PerspectiveView = glGenLists(1);
    glNewList(PerspectiveView, GL_COMPILE);

    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    ProjectionMatrix = glm::perspective(glm::radians(Config->Graphics.FieldOfVision), Config->Graphics.AspectRatio, Config->Graphics.NearClip, Config->Graphics.FarClip);
    glLoadMatrixf(glm::value_ptr(ProjectionMatrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEndList();

    OrthographicalView = glGenLists(1);
    glNewList(OrthographicalView, GL_COMPILE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    OrthoMatrix = glm::ortho(0.0f, float(Config->Graphics.Width), float(Config->Graphics.Height), 0.0f);
    glLoadMatrixf(glm::value_ptr(OrthoMatrix));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEndList();

    MainShader.Bind();
    MainShader.SetUniform("projectionMatrix", ProjectionMatrix);

    ProjectionShader.Bind();
    ProjectionShader.SetUniform("projectionMatrix", ProjectionMatrix);

    ColorizeShader.Bind();
    ColorizeShader.SetUniform("projectionMatrix", ProjectionMatrix);

//    Frustum.Resize();

}


