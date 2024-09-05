/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#pragma warning( push , 0)
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>
#include "Flag.h"
#include <iostream>
#pragma warning(pop)

void ClearFlags(std::vector<Flag*>& ff) {
    for (Flag* f : ff) {
        delete f;
    }
    ff.clear();

};

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1600;
    int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");


    std::vector<Flag*> flags;
    float spawnRot = 180;
    size_t lapCounter = 0;
    SetTargetFPS(60);

    bool picking = false;
    bool drawingModeEnabled = true;
    bool enableTrackSmoothing = false;
    //--------------------------------------------------------------------------------------
    Collider* hoveredCollider = nullptr;
    int storedInd = 0;
    Vector2 lastFrameMPose;

    bool justSwapped = false;
        system("CLS");
            std::cout << "DRAWING MODE ENABLED" << '\n';
            Racer* r = nullptr;


    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        DrawText("Swap between drawing and testing mode with V", 2, 0, 20, BLACK);
        DrawText("Clear all flags with C", 2, 20, 20, BLACK);
        DrawText("Enable track smoothing with S", 2, 40, 20, BLACK);
        DrawText("Swap to picking mode with W", 2, 60, 20, BLACK);
        ClearBackground(RAYWHITE);
        Vector2 v = GetMousePosition();

        if (!picking) {
            spawnRot -= GetMouseWheelMove() * 5;
        }
        
        
        if (IsKeyPressed(KEY_V)) {
            system("CLS");
            std::cout << "DRAWING MODE ENABLED" << '\n';
            drawingModeEnabled = !drawingModeEnabled;
            justSwapped = !drawingModeEnabled;

            if (!drawingModeEnabled && flags.size() >= 3) {
                if(r != nullptr) delete r;
                r = new Racer(flags[0], 3, 150, 20, flags);
            }

        }

        if (drawingModeEnabled) {
            if (IsKeyPressed(KEY_C)) {
                
                if (r != nullptr) {
                delete r;
                r = nullptr;

                }
                ClearFlags(flags);
            }

            if (IsKeyPressed(KEY_S)) {
                enableTrackSmoothing = !enableTrackSmoothing;
            }

            if (IsKeyPressed(KEY_W)) {
                picking = !picking;
            }

        }


        if (drawingModeEnabled) {
            if (!picking) {
                Rectangle r{ v.x, v.y, 30, 75};
                DrawRectanglePro(r, Vector2{ 0,0 }, spawnRot, RED);
            }
            else {
                for (Flag* f : flags) {
                    if (f->coll->CheckForOverlap(v)) {
                        hoveredCollider = f->coll;
                        storedInd = f->flagIndex;
                        if (IsMouseButtonDown(1)) {
                            hoveredCollider->picked = true;

                            Vector2 addVec = Vector2Subtract(lastFrameMPose, v);
                            float getNRot = GetMouseWheelMove() * 5;
                            hoveredCollider->UpdateRotation(-getNRot);
                            if (!isnan(addVec.x) || !isnan(addVec.y)) {
                                hoveredCollider->UpdatePosition(Vector2Negate(addVec));

                                if (storedInd != 0) {
                                    flags[storedInd - 1]->coll->UpdatePosition(Vector2Negate(addVec), 2);
                                    flags[storedInd - 1]->coll->UpdateRotation(-getNRot * .2);

                                }
                                if (storedInd != flags.size() - 1) {
                                    flags[storedInd + 1]->coll->UpdatePosition(Vector2Negate(addVec), 2);
                                    flags[storedInd + 1]->coll->UpdateRotation(-getNRot * .2);
                                }

                            }


                        }
                        f->coll->DrawHighlighted();

                    }
                    else {
                        if (hoveredCollider != nullptr) {
                            hoveredCollider->picked = false;
                            hoveredCollider = nullptr;
                        }
                    }
                }
            }

            if (IsMouseButtonPressed(0) && !picking) {
                int addedSegments = 1;
                Flag* f = new Flag(v, Vector2{ 30, 75 }, BLUE, spawnRot);
                std::cout << "FLAG CREATED AT LOCATION: " << v.x << " | " << v.y << '\n';
                f->flagIndex = flags.size();


                if (f->flagIndex != 0) {
                    int amntToAdd;
                    if (enableTrackSmoothing) {
                         amntToAdd = (((int)f->coll->m_origin.x - flags[f->flagIndex - 1]->coll->m_origin.x) +
                            ((int)f->coll->m_origin.y - flags[f->flagIndex - 1]->coll->m_origin.y) * 1.2)/50;
                        amntToAdd = abs(amntToAdd);

                        if (amntToAdd != 0) {
                            Flag* checkF = flags[f->flagIndex - 1];
                            Vector2 diffVec = Vector2Subtract(f->coll->m_origin, checkF->coll->m_origin);
                            diffVec = Vector2Divide(diffVec, amntToAdd + 1);
                            int rotDiff = checkF->coll->m_rot - f->coll->m_rot;
                            float addedRot = 0;

                            for (int i = 0; i < amntToAdd; i++) {
                                addedRot += abs(rotDiff / (amntToAdd * 3));
                                Flag* nf = new Flag(Vector2Add(checkF->coll->m_origin, Vector2MultiplyV(diffVec, Vector2{ (float)i + 1 , (float)i + 1})),
                                    Vector2{ 30, 75 }, BLUE, spawnRot);
                                std::cout << "FLAG CREATED AT LOCATION: " << nf->coll->m_origin.x << " | " << nf->coll->m_origin.y << '\n';

                                flags.push_back(nf);
                                nf->flagIndex = flags.size() - 1;
                                nf->m_previousFlag = flags[nf->flagIndex - 1];
                                flags[nf->flagIndex - 1]->m_nextFlag = nf;

                            }
                        }
                    
                    
                    }
                }
                flags.push_back(f);
                f->flagIndex = flags.size() - 1;
                if (flags.size() != 1) {
                    flags[f->flagIndex - 1]->m_nextFlag = f;
                    f->m_previousFlag = flags[f->flagIndex - 1];
                }
                f->m_nextFlag = flags[0];
                flags[0]->m_previousFlag = f;

                //for (int i = 0; i < flags.size(); i++) {
                //    std::cout << "Flag Data for: " << flags[i]->flagIndex << '\n';
                //    std::cout << "Previous Flag: " << flags[i]->m_previousFlag->flagIndex << '\n';
                //    std::cout << "Next Flag: " << flags[i]->m_nextFlag->flagIndex << '\n';
                //    std::cout << "================================" << '\n';
                //}

            }
        }
        else {
            if (justSwapped) {
                std::cout << "Lap start!" << '\n';
                for (int i = 0; i < flags.size(); i++) {
                    std::cout << "Flag: " << i << " - Status: " << std::boolalpha << flags[i]->m_hasPassed << '\n';
                }
                std::cout << "Lap count: " << lapCounter << '\n';
                justSwapped = false;
            
            }


            for (Flag* f : flags) {
                if (r == nullptr) break;
                if (f->coll->CheckForEntry(r->m_pos)) {

                    //system("CLS");
                    if (f->flagIndex == 0) {
                        std::cout << "Lap start!" << '\n';
                        f->m_hasPassed = true;

                        if (f->m_previousFlag->m_hasPassed) {
                            lapCounter++;
                            for (int i = 1; i < flags.size(); i++)
                            {
                                flags[i]->m_hasPassed = false;
                            }
                        }

                    }
                    else {
                        if (f->m_previousFlag->m_hasPassed) {
                            std::cout << "Passed next flag at " << f->flagIndex << '\n';
                            f->m_hasPassed = true;
                        }
                        else {
                            std::cout << "Flag skipped!!!" << '\n';
                        }
                    }
                    for (int i = 0; i < flags.size(); i++) {
                        std::cout << "Flag: " << i << " - Status: " << std::boolalpha << flags[i]->m_hasPassed << '\n';
                    }
                    std::cout << "Lap count: " << lapCounter << '\n';
                }
            }
        }

        for (Flag* f : flags) {
            if (f->flagIndex == 0 && !drawingModeEnabled) {
                f->coll->DrawStart();
            }
            f->coll->Draw();
            if (f->m_nextFlag->m_hasPassed && f->m_nextFlag->flagIndex != 0) {
                DrawLine(f->coll->m_midPoint.x, f->coll->m_midPoint.y, f->m_nextFlag->coll->m_midPoint.x, f->m_nextFlag->coll->m_midPoint.y, BLUE);
            }
            else {
                DrawLine(f->coll->m_midPoint.x, f->coll->m_midPoint.y, f->m_nextFlag->coll->m_midPoint.x, f->m_nextFlag->coll->m_midPoint.y, RED);
            }
        }


        if (r != nullptr) {
            r->Update();
            r->Draw();
        }


        



        
        lastFrameMPose = v;
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}