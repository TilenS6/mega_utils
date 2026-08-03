#include "sprite.h"

Sprite::Sprite(SDL_Renderer* r, string pathToFolder, int x, int y, int w, int h, Sprite* refererenceSprite, double visibility) {
    _opacity = visibility;
    _refferenceSprite = refererenceSprite;
    _animationTimer.interval();
    setOffset(x, y);
    _w = w;
    _h = h;
    setScale(1.);

    _refferenceSprite = refererenceSprite;
    // Prevent circular dependency:
    Sprite* current = refererenceSprite;
    while (current) {
        if (current == this) {
            cerr << "Error: Circular reference detected in Sprite constructor, removing reference." << endl;
            _refferenceSprite = nullptr;  // Break the circular reference
            break;
        }
        current = current->_refferenceSprite;
    }

    loadAnimations(r, pathToFolder);
}

void Sprite::loadAnimations(SDL_Renderer* r, string pathToFolder) {
    // Check if folder exists
    if (!filesystem::exists(pathToFolder)) {
        cerr << "Error: Folder does not exist: " << pathToFolder << endl;
        return;
    }

    // List all files in the folder and check for .png files matching the pattern <animation_name>_<frame#>.png
    regex framePattern(R"((\w+)_(\d+)\.png)");
    vector<string> frameFiles;
    for (const auto& entry : filesystem::directory_iterator(pathToFolder)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
            if (regex_match(filename, framePattern)) {
                frameFiles.push_back(filename);
            }
        }
    }

    // Sort frame files to ensure correct order
    sort(frameFiles.begin(), frameFiles.end(), [](const string& a, const string& b) {
        regex framePattern(R"((\w+)_(\d+)\.png)");
        smatch matchA, matchB;
        regex_match(a, matchA, framePattern);
        regex_match(b, matchB, framePattern);
        if (matchA[1] != matchB[1]) {
            return matchA[1] < matchB[1];  // Sort by animation name first
        }
        try {
            int frameNumberA = stoi(matchA[2]);
            int frameNumberB = stoi(matchB[2]);
            return frameNumberA < frameNumberB;
        } catch (const std::exception& e) {
            cerr << "Error occurred while sorting frame file names \"" << a << "\" and \"" << b << "\": " << e.what() << endl;
        }
        return false;
    });

    // Load all textures and organize them into animations
    for (const auto& filename : frameFiles) {
        smatch match;
        regex_match(filename, match, framePattern);
        string animationName = match[1];
        int frameNumber;
        try {
            frameNumber = stoi(match[2]);  // Validate frame number
        }
        // Standard exceptions for stoi
        catch (const invalid_argument& e) {
            cerr << "Invalid frame number in filename: " << filename << endl;
        } catch (const out_of_range& e) {
            cerr << "Frame number out of range in filename: " << filename << endl;
        }

        // Load texture
        string fullPath = pathToFolder + "/" + filename;
        SDL_Surface* surface = IMG_Load(fullPath.c_str());
        if (!surface) {
            cerr << "Failed to load image: " << fullPath << " Error: " << SDL_GetError() << endl;
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);
        SDL_FreeSurface(surface);
        if (!texture) {
            cerr << "Failed to create texture from surface: " << fullPath << " Error: " << SDL_GetError() << endl;
            continue;
        }

        // Add texture to the corresponding animation
        _animations[animationName].frameTextures.push_back(texture);
    }

    // Load meta.json
    string metaPath = pathToFolder + "/meta.json";
    ifstream metaFile(metaPath);
    if (!metaFile.is_open()) {
        cout << "WARNING: Failed to open meta.json at: " << metaPath << ", using default values for animations.\n";
    } else {
        json metaJson;
        metaFile >> metaJson;

        for (auto& [animationName, animationData] : metaJson.items()) {
            if (animationData.contains("fps")) {
                _animations[animationName].fps = animationData["fps"];
                animationData.erase("fps");
            }
            if (animationData.contains("type")) {
                string loopTypeStr = animationData["type"];
                transform(loopTypeStr.begin(), loopTypeStr.end(), loopTypeStr.begin(), ::toupper);

                if (loopTypeStr == "NO_LOOP") {
                    _animations[animationName].loopType = NO_LOOP;
                } else if (loopTypeStr == "LOOP") {
                    _animations[animationName].loopType = LOOP;
                } else if (loopTypeStr == "PING_PONG") {
                    _animations[animationName].loopType = PING_PONG;
                } else if (loopTypeStr == "ONCE") {
                    _animations[animationName].loopType = ONCE;
                } else {
                    cerr << "Unknown loop type in meta.json for animation: " << animationName << endl;
                }
                animationData.erase("type");
            }
            if (!animationData.empty()) {
                cout << "WARNING: Unknown properties in " << metaPath << " for animation: " << animationName << endl;
            }
        }
    }
}

Sprite::~Sprite() {
    // Free textures
    for (auto& [name, animation] : _animations) {
        for (auto& frameT : animation.frameTextures) {
            if (frameT) SDL_DestroyTexture(frameT);
        }
    }
}

void Sprite::setOffset(double x, double y) {
    _offset.x = x;
    _offset.y = y;
}

void Sprite::setOffset(Point p) {
    _offset = p;
}

void Sprite::setScale(double scale) {
    _scale = scale;
}

void Sprite::setVisibility(double newVisibility) {
    _opacity = min(max(newVisibility, 0.0), 1.0);
}

void Sprite::moveBy(double x, double y) {
    _offset.x += x;
    _offset.y += y;
}
void Sprite::moveBy(Point p) {
    _offset += p;
}

Point Sprite::getPos(Camera* c) {
    Point renderPos = _offset.getRenderPos(c);
    if (_refferenceSprite) {
        Point referenceRenderPos = _refferenceSprite->getPos(c);
        renderPos += referenceRenderPos;
    }
    return renderPos;
}

void Sprite::render(Camera* c) {
    static int __lastFrameIndex = 0;  // Static variable to keep track of the last frame index rendered

    // Position and size calculations
    SDL_FRect destRect;
    Point totalOffset = _offset;
    if (_refferenceSprite) {
        totalOffset += _refferenceSprite->_offset;  // Add reference sprite's offset
    }
    Point renderPos = totalOffset.getRenderPos(c);
    destRect.x = (float)(renderPos.x);
    destRect.y = (float)(renderPos.y);
    destRect.w = (float)(_w * c->scale * _scale);
    destRect.h = (float)(_h * c->scale * _scale);
    
    // Get the current animation and its frame count
    if (_animations.find(_currentAnimation) == _animations.end()) {
        cerr << "Animation not found: " << _currentAnimation << endl;
        return;
    }
    
    Animation& currentAnimation = _animations[_currentAnimation];
    int frameCount = currentAnimation.frameTextures.size();
    if (frameCount == 0) {
        return;  // No frames to render
    }

    // Determine the current frame index based on the loop type and elapsed time
    int currentFrameIndex = __lastFrameIndex;
    if (!_animationStopped) {
        double elapsedTime = _animationTimer.getTime() - _stoppedTime;
        if (elapsedTime < 0) elapsedTime = 0;  // Ensure elapsed time is non-negative
        switch (currentAnimation.loopType) {
            case NO_LOOP:
            default:
                currentFrameIndex = 0;
                break;
            case LOOP:
                if (currentAnimation.fps > 0) {
                    double frameDuration = 1.0 / currentAnimation.fps;
                    currentFrameIndex = static_cast<int>(elapsedTime / frameDuration) % frameCount;
                }
                break;
            case PING_PONG:
                if (currentAnimation.fps > 0) {
                    double frameDuration = 1.0 / currentAnimation.fps;
                    int frameLinear = static_cast<int>(elapsedTime / frameDuration) % (frameCount * 2 - 2);
                    if (frameLinear >= frameCount) {
                        currentFrameIndex = 2 * frameCount - 2 - frameLinear;  // Reverse direction
                    } else {
                        currentFrameIndex = frameLinear;
                    }
                }
                break;
            case ONCE:
                if (currentAnimation.fps > 0) {
                    double frameDuration = 1.0 / currentAnimation.fps;
                    if (elapsedTime >= frameDuration * frameCount) {
                        currentFrameIndex = frameCount - 1;
                    } else {
                        currentFrameIndex = static_cast<int>(elapsedTime / frameDuration) % frameCount;
                    }
                }
                break;
        }
    }

    // Draw the current frame texture
    SDL_Texture* currentFrameTexture = currentAnimation.frameTextures[currentFrameIndex];
    if (currentFrameTexture) {
        SDL_SetTextureAlphaMod(currentFrameTexture, static_cast<Uint8>(_opacity * 255));
        SDL_RenderCopyF(c->r, currentFrameTexture, nullptr, &destRect);
        SDL_SetTextureAlphaMod(currentFrameTexture, 255);  // Reset alpha mod to default
    }

    // Update the last frame index rendered
    __lastFrameIndex = currentFrameIndex;
}

void Sprite::startAnimation(string animationName) {
    if (_animations.find(animationName) != _animations.end()) {
        _currentAnimation = animationName;
        restartAnimation();
    } else {
        cerr << "Animation not found: " << animationName << endl;
    }
}

void Sprite::restartAnimation() {
    _stoppedTime = 0;
    _animationStopped = false;
    _animationTimer.interval();  // Reset the timer to restart the animation
    _stoppedTimer.interval();
}

void Sprite::pauseAnimation(bool pause) {
    _animationStopped = pause;
    double passed = _stoppedTimer.interval();  // Reset the stopped timer when pausing
    if (!pause) _stoppedTime += passed;        // Accumulate the stopped time when resuming
}

void Sprite::changeAnimationLoopType(loopTypes newLoopType) {
    if (_animations.find(_currentAnimation) != _animations.end()) {
        _animations[_currentAnimation].loopType = newLoopType;
    } else {
        cerr << "Current animation not found: " << _currentAnimation << endl;
    }
}

void Sprite::updateEvents(Camera* c, Mouse* m, Keyboard* kb, Controllers* controllers, double dt) {
    // Get the sprite's position and size in world coordinates
    Rectng spriteRect = { (double)_offset.x, (double)_offset.y, (double)(_w * _scale), (double)(_h  * _scale) };
    Point mouseP = m->cameraToWorldPos(c);
    SpriteEventDevices sed = {c, m, kb, controllers, dt};

    // Check for mouse hover
    if (m && collisionPointRectangle(mouseP, spriteRect)) {
        if (onHover) onHover(*this, sed);
        // Check for mouse click
        if (m->last_event & Mouse::M_LClickMask) {
            if (onClick) onClick(*this, sed);
        }
    }

    // Call the update event callback
    if (onUpdate) onUpdate(*this, sed);
}