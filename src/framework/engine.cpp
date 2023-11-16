#include "engine.h"

const color WHITE(1, 1, 1);
const color BLACK(0, 0, 0);
const color BLUE(0, 0, 1);
const color RED(1, 0, 0);
const color GREEN(0, 1, 1);
static bool flagPlayer1 = false;
static bool flagPlayer2 = false;

Engine::Engine() {
    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(WIDTH, HEIGHT, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    shaderManager = make_unique<ShaderManager>();
    shapeShader = this->shaderManager->loadShader("../res/shaders/circle.vert",
                                                  "../res/shaders/circle.frag",
                                                  nullptr, "circle");
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    vector<vector<int>> coordinateMatrix;
    for (int column = 0; column < 4; column++) {
        for (int row = 0; row < 3; row++) {
            coordinateMatrix.push_back({(int) (0.25 * column * WIDTH), (int) (0.33 * row * HEIGHT)});
        }
    }

    for (int ii = 0; ii < 12; ii++) {
        vector<int> coordVect = coordinateMatrix[ii];
        cardShapes.push_back(make_unique<Rect>(shapeShader, vec2{100, 100}, vec2{WIDTH / 4, HEIGHT / 2},
                                           color{WHITE.red, WHITE.green, WHITE.blue, WHITE.alpha}));
    }

    cardItem = make_unique<Rect>(shapeShader, vec2{10, 10}, vec2{100, 100},
                                 color{1.0f, 1.0f, 1.0f, 1.0f});
}

void Engine::processInput() {
    glfwPollEvents();

    // Close window if escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        flagPlayer1 = true;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        flagPlayer2 = true;
    }

    for(unique_ptr<Rect> &shape : cardShapes) {
        // Add stuff
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = HEIGHT - mouseY; // make sure mouse y-axis isn't flipped
}


void Engine::update() {
    // Change values of objects
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (flagPlayer1 || flagPlayer2) {
        // add stuff here
    }

}

void Engine::render() {
    // Draw objects
    glClearColor(BLACK.red, BLACK.green, BLACK.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    for (const unique_ptr<Rect> & cardShape : cardShapes) {
        cardShape->setUniforms();
        cardShape->draw();
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}