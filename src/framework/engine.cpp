
#include <algorithm>
#include <random>
#include "engine.h"

using namespace std;

// Color objects
const color WHITE(1, 1, 1);
const color BLACK(0, 0, 0);
const color RED(1, 0, 0);
const color GREEN(1, 0, 0);
const color BLUE(1, 0, 0);

//
enum state {start, instructions, selectCards, play, over};
state screen = start;
static bool flagPlayer1 = false;
static bool flagPlayer2 = false;

Engine::Engine() {
    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() = default;

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
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert",
                                                  "../res/shaders/shape.frag",
                                                  nullptr, "shape");
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", FONT_SIZE);

    // Set uniforms
    textShader.use().setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    // Shape object for the cursor
    cursor = make_unique<Rect>(shapeShader, vec2(10, 10), vec2(0, 0), WHITE);

    vector<vector<int>> coordinateMatrix;
    for (int ii = 1; ii <= 4; ii++) {
        for (int jj = 1; jj <= 3; jj++) {
            coordinateMatrix.push_back({ii * 400 - 160, jj * 240 + 100});
        }
    }
    for (int ii = 0; ii < 12; ii++) {
        vector<int> coordVect = coordinateMatrix[ii];
        cardShapes.push_back(make_unique<Rect>(shapeShader, vec2{coordVect[0], coordVect[1]}, vec2{360, 200},
                                           color{WHITE.red, WHITE.green, WHITE.blue, WHITE.alpha}));
        outlineShapes.push_back(make_unique<Rect>(shapeShader, vec2{coordVect[0], coordVect[1]}, vec2{380, 220},
                                               color{RED.red, RED.green, RED.blue, RED.alpha}));
    }

    // Populate the deck
    for (int ii = 0; ii < 3; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            for (int kk = 0; kk < 3; kk++) {
                for (int ll = 0; ll < 3; ll++) {
                    deck.push_back(make_unique<card>(card(ii, jj, kk, ll)));
                }
            }
        }
    }
    // Shuffle the card vector
    std::shuffle(std::begin(deck), std::end(deck), std::mt19937(std::random_device()()));
    // Grab the first 12 cards to begin the game with
    for (int ii = 0; ii < 12; ii++) {
        cardsInPlay.push_back(std::move(deck[ii]));
    }
}

void Engine::processInput() {
    glfwPollEvents();

    // Variable to keep track of the last frame's mouse status
    static bool mousePressedLastFrame = false;

    // Close window if escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = HEIGHT - mouseY; // make sure mouse y-axis isn't flipped

    cursor->setPosX(mouseX);
    cursor->setPosY(mouseY);

    switch (screen) {
        case start: {
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { screen = play; }
            else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) { screen = instructions; }
            break;
        }
        case instructions: {
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { screen = play; }
            break;
        }
        case play: {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                flagPlayer1 = true;
                countSelected = 0;
            }
            else if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
                flagPlayer2 = true;
            }
        }
        case selectCards: {
            // Variable to hold mouse press status
            bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            for (int ii = 0; ii < cardShapes.size(); ii++) {
                if (cardShapes[ii]->isOverlapping(*cursor)) {
                    hoverIndices.push_back(ii);
                    if (!mousePressed && mousePressedLastFrame) {
                        if (flagPlayer1) {player1.push_back(std::move(cardsInPlay[ii]));}
                        else if (flagPlayer2) {player2.push_back(std::move(cardsInPlay[ii]));}
                        cardsInPlay.push_back(std::move(deck.back()));
                    }
                }
            }
            mousePressedLastFrame = mousePressed;
        }
    }
}


void Engine::update() {
    // Change values of objects
    // Calculate delta time
    auto currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (flagPlayer1 || flagPlayer2) {screen = selectCards;}

    // TODO: Check to see if there are still sets that can be made
}

void Engine::render() {
    // Draw objects
    glClearColor(BLACK.red, BLACK.green, BLACK.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    switch (screen) {
        case start: {
            // TODO: Make start screen
            break;
        }
        case instructions: {
            // TODO: Make instructions screen
            break;
        }
        case play: {
            // TODO: Show current scores of each player
            for (const unique_ptr<Rect> &cardShape : cardShapes) {
                cardShape->setUniforms();
                cardShape->draw();
            }
        }
        case selectCards: {
            // TODO: Show which player is selecting, show current scores of each player
            for(int ii = 0; ii < hoverIndices.size(); ii++) {
                outlineShapes[hoverIndices[ii]]->setUniforms();
                outlineShapes[hoverIndices[ii]]->draw();
                hoverIndices.pop_back();
            }
            for (const unique_ptr<Rect> &cardShape : cardShapes) {
                cardShape->setUniforms();
                cardShape->draw();
            }
        }
        case over: {
            // TODO: Show which player one and both scores
            break;
        }
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}