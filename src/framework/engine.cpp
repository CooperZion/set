#include "engine.h"

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
                                                 nullptr, "tex");
    cardShader.use();
    cardShader.setMatrix4("tex", this->PROJECTION);

    // Configure text shader and renderer
    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");
    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", FONT_SIZE);

    // Set uniforms
    textShader.use().setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
    cardShader.use().setMatrix4("tex", this->PROJECTION);

    // Texturing magic
    glGenTextures(1, &textureObject);
    glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    char filename[] = "../res/cardAtlas.png";
    imgObject = stbi_load(filename, &imgX, &imgY, &imgN, 4);
    if (imgObject) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgX, imgY, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgObject);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, textureObject);
    glEnable(GL_TEXTURE_2D);

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
        whiteShapes.push_back(make_unique<Rect>(shapeShader, vec2{coordVect[0], coordVect[1]}, vec2{350, 225},
                                                  color{WHITE.red, WHITE.green, WHITE.blue, WHITE.alpha}));
    }

    // Populate the deck
    for (int ii = 0; ii < 3; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            for (int kk = 0; kk < 3; kk++) {
                for (int ll = 0; ll < 3; ll++) {
                    int val = (ii + 1) * (jj + 1) * (kk + 1) * (ll + 1);
                    deck.emplace_back(ii, jj, kk, ll, val);
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
        for (int ii = 0; ii < outlineShapes.size(); ii++) {
            if (outlineShapes[ii]->isOverlapping(*cursor)) {
                hoverIndices.push_back(ii);
                if (!mousePressed && mousePressedLastFrame && selectedCards.size() < 3) {
                    if (selectedCards.empty() || selectedCards[selectedCards.size() - 1] != cardsInPlay[ii]) {
                        selectedCards.push_back(cardsInPlay[ii]);
                        selectedIndices.push_back(ii);
                    }
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

            string instructions_1a = "1. The object of the game is to identify a SET of 3 cards from the cards ";
            string instructions_1b = "on screen. Each card has four distinct features: shape, color, number,";
            string instructions_1c = "and shading. Player with the most SETs at the end of the game wins.";

            string instructions_2a = "2. A SET consists of 3 cards in which each of the cards' features, when";
            string instructions_2b = "looked at individually, are the same on each card, or are different on ";
            string instructions_2c = "each card. All of the features must SEPARATELY satisfy this rule. In";
            string instructions_2d = "other words: shape must be either the same on all 3 cards, or different";
            string instructions_2e = "on all 3 cards; color must be the same on all 3 cards, or different";
            string instructions_2f = "on all 3 cards, etc.";

            string instructions_3a = "3. To begin, the cards are shuffled, and 12 are displayed on the screen.";
            string instructions_3b = "Players pick up SETs of 3 cards from anywhere on the table. If chosen";
            string instructions_3c = "correctly, the SET is kept by the player and the 3 cards are replaced";
            string instructions_3d = "with 3 from the deck.";

            string instructions_4a = "4. There are no turns; the first player to click their assigned button";
            string instructions_4b = "gets control of the board and is then able to pick their SET. [LShift]";
            string instructions_4c = "button is for player 1, [RShift] is for player 2.";

            string instructions_5a = "5. The play continues until the deck is depleted. At the end of the";
            string instructions_5b = "game, there may be cards remaining that do not form a SET. The number of";
            string instructions_5c = "SETs held by each player is then counted. One point is given for each";
            string instructions_5d = "SET. The player with the highest score wins.";

            fontRenderer->renderText(message, 260, 560, 1.2, WHITE_VECT);

            fontRenderer->renderText(instructions_1a, 25, 510, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_1b, 25, 490, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_1c, 25, 470, .45, WHITE_VECT);

            fontRenderer->renderText(instructions_2a, 25, 430, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2b, 25, 410, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2c, 25, 390, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2d, 25, 370, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2e, 25, 350, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_2f, 25, 330, .45, WHITE_VECT);

            fontRenderer->renderText(instructions_3a, 25, 290, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_3b, 25, 270, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_3c, 25, 250, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_3d, 25, 230, .45, WHITE_VECT);

            fontRenderer->renderText(instructions_4a, 25, 190, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_4b, 25, 170, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_4c, 25, 150, .45, WHITE_VECT);

            fontRenderer->renderText(instructions_5a, 25, 130, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_5b, 25, 110, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_5c, 25, 90, .45, WHITE_VECT);
            fontRenderer->renderText(instructions_5d, 25, 70, .45, WHITE_VECT);

            break;
        }
        case play: {
            for (auto &card : whiteShapes) {
                card->setUniforms();
                card->draw();
            }

            // Render the card textures
            for (int ii = 0; ii < cardShapes.size(); ii++) {
                mapPos = cardsInPlay[ii].getMapPos();
                cardShapes[ii]->setVertices(mapPos);
                cardShapes[ii]->setUniforms();
                glBindTexture(GL_TEXTURE_2D, textureObject);
                cardShapes[ii]->draw();
            }

            string score_player1 = "Player 1's score: " + to_string(player1Score);
            string score_player2 = "Player 2's score: " + to_string(player2Score);
            fontRenderer->renderText(score_player1, 37, 50, .45, WHITE_VECT);
            fontRenderer->renderText(score_player2, 557, 50, .45, WHITE_VECT);
        }
        case selectCards: {
            // Render the shapes of the outlines
            for (int hoverIndex : hoverIndices) {
                outlineShapes[hoverIndex]->setUniforms();
                outlineShapes[hoverIndex]->draw();
                if (outlineShapes[hoverIndex]->getGreen() == 0) {hoverIndices.pop_back();}
            }

            for (auto &card : whiteShapes) {
                card->setUniforms();
                card->draw();
            }

            // Render the card textures
            for (int ii = 0; ii < cardShapes.size(); ii++) {
                mapPos = cardsInPlay[ii].getMapPos();
                cardShapes[ii]->setVertices(mapPos);
                cardShapes[ii]->setUniforms();
                glBindTexture(GL_TEXTURE_2D, textureObject);
                cardShapes[ii]->draw();
            }

            //show which player is currently selecting
            if (flagPlayer1) {
                selecting = "Player 1 is currently selecting...";
                fontRenderer->renderText(selecting, 37, 50, .45, WHITE_VECT);
            }
            if (flagPlayer2) {
                selecting = "Player 2 is currently selecting...";
                fontRenderer->renderText(selecting, 37, 50, .45, WHITE_VECT);
            }

            // Handle the logic for whether the 3 selected cards are a set
            if (selectedCards.size() == 3) {
                if (validSet) {
                    // TODO: Make a message for a valid set
                    if (flagPlayer1) {player1Score++;}
                    else if (flagPlayer2) {player2Score++;}
                    validSet = false;
                }
                else {
                    // TODO: Make a message for invalid set
                }
                if (flagPlayer1) {flagPlayer1 = false;}
                else if (flagPlayer2) {flagPlayer2 = false;}
                // Clear the selected cards and switch back to the play screen,
                // regardless of whether they're a set
                screen = play;
                selectedCards.clear();
                selectedIndices.clear();
            }
        }
        case over: {
            if (player1Score > player2Score) {
                fontRenderer->renderText("Player 1 Wins!", 37, 150, 1.4, WHITE_VECT);
            }

            if (player2Score > player1Score) {
                fontRenderer->renderText("Player 2 Wins!", 37, 150, 1.4, WHITE_VECT);
            }

            break;
        }
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}