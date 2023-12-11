
#include <algorithm>
#include <random>
#include "engine.h"
#include "stb_image.h"

using namespace std;

// Color objects
const color WHITE(1, 1, 1);
const color BLACK(0, 0, 0);
const color RED(1, 0, 0);
const color GREEN(0, 1, 0);

// Color vectors
vec3 WHITE_VECT = {WHITE.red, WHITE.green, WHITE.blue};

// game state objects
enum state {start, instructions, play, selectCards, over};
state screen = start;

// flags to denote which player is acting
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

    cardShader = this->shaderManager->loadShader("../res/shaders/card.vert",
                                                 "../res/shaders/card.frag",
                                                 nullptr, "card");
    cardShader.use();
    cardShader.setMatrix4("tex", this->PROJECTION);

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", FONT_SIZE);

    // Set uniforms
    textShader.use().setVector2f("vertex", vec4(100, 100, .5, .5));
    cardShader.use().setMatrix4("projection", this->PROJECTION);
    shapeShader.use().setMatrix4("projection", this->PROJECTION);

    // Texturing magic
    const char *filename = "card_map.png";
    imgObject = stbi_load(filename, &imgX, &imgY, &imgN, 4);

    glGenTextures(1, &textureObject);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureObject);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgX, imgY, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgObject);
    free(imgObject);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Engine::initShapes() {
    // Shape object for the cursor
    cursor = make_unique<Rect>(shapeShader, vec2(10, 10), vec2(0, 0), WHITE);

    vector<vector<int>> coordinateMatrix;
    for (int ii = 1; ii <= 4; ii++) {
        for (int jj = 1; jj <= 3; jj++) {
            coordinateMatrix.push_back({ii * 390 - 135, jj * 265 + 20});
        }
    }
    for (int ii = 0; ii < 12; ii++) {
        vector<int> coordVect = coordinateMatrix[ii];
        cardShapes.push_back(make_unique<Tex>(cardShader, vec2{coordVect[0], coordVect[1]}, vec2{350, 225},
                                           color{WHITE.red, WHITE.green, WHITE.blue, WHITE.alpha}));
        outlineShapes.push_back(make_unique<Rect>(shapeShader, vec2{coordVect[0], coordVect[1]}, vec2{370, 245},
                                                  color{RED.red, RED.green, RED.blue, RED.alpha}));
    }

    int mapPos = 1;
    // Populate the deck
    for (int ii = 0; ii < 3; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            for (int kk = 0; kk < 3; kk++) {
                for (int ll = 0; ll < 3; ll++) {
                    deck.emplace_back(ii, jj, kk, ll, mapPos);
                }
            }
        }
    }
    // Shuffle the card vector
    std::shuffle(std::begin(deck), std::end(deck), std::mt19937(std::random_device()()));
    // Grab the first 12 cards to begin the game with
    for (int ii = 0; ii < 12; ii++) {
        cardsInPlay.push_back(deck[ii]);
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

    if (screen == start) {
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { screen = play; }
        else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) { screen = instructions; }
    }
    else if (screen == instructions) {
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { screen = play; }
    }
    else if (screen == play) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            flagPlayer1 = true;
            screen = selectCards;
            selectedCards.clear();
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            flagPlayer2 = true;
            screen = selectCards;
            selectedCards.clear();
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            extraCards = true;
        }
    }
    else if (screen == selectCards) {
        // Variable to hold mouse press status
        bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        for (int ii = 0; ii < cardShapes.size(); ii++) {
            if (cardShapes[ii]->isOverlapping(*cursor)) {
                hoverIndices.push_back(ii);
                if (!mousePressed && mousePressedLastFrame && selectedCards.size() < 3) {
                    selectedCards.push_back(cardsInPlay[ii]);
                    // Error line?
                    cardsInPlay.push_back(deck.back());
                    selectedIndices.push_back(ii);
                }
            }
        }
        mousePressedLastFrame = mousePressed;
    }
}


void Engine::update() {
    // Change values of objects
    // Calculate delta time
    auto currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Logic to deal with selected cards in the selection screen
    if (!selectedIndices.empty()) {
        // Check to see if there are 3 cards selected, if there are, see if it's a set
        if (selectedCards.size() == 3) { validSet = selectedCards[0].isSetWith(&selectedCards[1], &selectedCards[2]);}
        // Change the outline of selected cards to green
        for (int selectedIndex: selectedIndices) {outlineShapes[selectedIndex]->setColor(GREEN);}
    }
    // Reset the red outlines after the selections are cleared
    else {
        for (unique_ptr<Rect> &shape: outlineShapes) {shape->setColor(RED);}
    }

    // Check to see if there are still sets that can be made
    // This seems like an inefficient way of doing it, but I don't know how else to do it
    for (card &card1 : cardsInPlay) {
        for (card &card2: cardsInPlay) {
            for (card &card3 : cardsInPlay) {
                if (&card1 != &card2 && &card2 != &card3 && &card1 != &card3) {
                    setCanBeMade = card1.isSetWith(&card2, &card3);
                }
            }
        }
    }

    // This is the only way the game ends
    if (!setCanBeMade && deck.empty()) {
        screen = over;
    }

}

void Engine::render() {
    // Draw objects
    glClearColor(BLACK.red, BLACK.green, BLACK.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    switch (screen) {
        case start: {
            string message = "Let's Play Set!";
            string message1 = "Press [S] to start game, [I] for instructions";
            fontRenderer->renderText(message, 192, 300, 1.2, WHITE_VECT);
            fontRenderer->renderText(message1, 50, 200, .65, WHITE_VECT);
            break;
        }
        case instructions: {
            string message = "Instructions";
            // newline isn't working????
            string instructions_1a = "1. The object of the game is to identify a SET of 3 cards from 12 cards ";
            string instructions_1b = "placed face-up on the table. Each card has four distinct features: shape,";
            string instructions_1c = "color, number, and shading. ";

            string instructions_2a = "2. A SET consists of 3 cards in which each of the cards' features, when";
            string instructions_2b = "looked at one-by-one, are the same on each card, or, are different on ";
            string instructions_2c = "each card. All of the features must SEPARATELY satisfy this rule. In";
            string instructions_2d = "other words: shape must be either the same on all 3 cards, or different";
            string instructions_2e = "on each of the 3 cards; color must be either the same on all 3 cards,";
            string instructions_2f = "or different on each of the 3, etc.";

            fontRenderer->renderText(message, 260, 560, 1.2, WHITE_VECT);

            fontRenderer->renderText(instructions_1a, 25, 510, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_1b, 25, 490, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_1c, 25, 470, .45, WHITE_VECT);

            fontRenderer->renderText(instructions_2a, 25, 440, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2b, 25, 420, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2c, 25, 400, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2d, 25, 380, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2e, 25, 360, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2f, 25, 340, .45, WHITE_VECT);

            break;
        }
        case play: {
            // TODO: Show current scores of each player
            // Render the card textures
            for (int ii = 0; ii < cardShapes.size(); ii++) {
                mapPos = cardsInPlay[ii].getMapPos();
                mapU = int(mapPos / 9) + 1;
                mapV = mapPos % 9;
                cardShapes[ii]->setUniforms(mapU, mapV);
                cardShapes[ii]->draw();
            }
        }
        case selectCards: {
            // TODO: Show which player is selecting, show current scores of each player
            // Render the shapes of the outlines
            for (int hoverIndex : hoverIndices) {
                outlineShapes[hoverIndex]->setUniforms();
                outlineShapes[hoverIndex]->draw();
                if (outlineShapes[hoverIndex]->getGreen() == 0) {hoverIndices.pop_back();}
            }
            // Render the card textures
            for (int ii = 0; ii < cardShapes.size(); ii++) {
                // TODO: Add the UV coordinates here and call this
                mapPos = cardsInPlay[ii].getMapPos();
                mapU = int(mapPos / 9) + 1;
                mapV = mapPos % 9;
                cardShapes[ii]->setUniforms(mapU, mapV);
                cardShapes[ii]->draw();
            }
            // Handle the logic for whether the 3 selected cards are a set
            if (selectedCards.size() == 3) {
                if (validSet) {
                    // TODO: Make a message for a valid set
                    if (flagPlayer1) {
                        player1Score++;
                        flagPlayer1 = false;
                    }
                    else if (flagPlayer2) {
                        player2Score++;
                        flagPlayer2 = false;
                    }
                    validSet = false;
                }
                else {
                    // TODO: Make a message for invalid set
                }
                // Clear the selected cards and switch back to the play screen,
                // regardless of whether they're a set
                screen = play;
                selectedCards.clear();
                selectedIndices.clear();
            }
        }
        case over: {
            // TODO: Show which player won and both scores
            break;
        }
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}