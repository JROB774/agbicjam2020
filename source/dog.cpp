GLOBAL constexpr float DOG_MOVE_SPEED = (10 * TILE_W);
GLOBAL constexpr float DOG_MAX_VEL = (20 * TILE_W);

GLOBAL constexpr float DOG_JUMP_FORCE = (25 * TILE_W);
GLOBAL constexpr float DOG_WEIGHT = 0.75f;

GLOBAL constexpr int DOG_CLIP_W = 24;
GLOBAL constexpr int DOG_CLIP_H = 24;

GLOBAL constexpr float DOG_BOUNDS_X =  6;
GLOBAL constexpr float DOG_BOUNDS_Y = 13;
GLOBAL constexpr float DOG_BOUNDS_W = 12;
GLOBAL constexpr float DOG_BOUNDS_H =  7;

GLOBAL constexpr float DOG_DEAD_TIME = 2.0f;

INTERNAL void CreateDog (Dog& dog, float x, float y, Flip flip)
{
    dog.state = DOG_STATE_IDLE;

    dog.pos = { x, y };
    dog.vel = { 0, 0 };

    dog.bounds = { DOG_BOUNDS_X,DOG_BOUNDS_Y,DOG_BOUNDS_W,DOG_BOUNDS_H };

    LoadImage(dog.image, "dog.bmp");
    dog.flip = flip;

    dog.footstep_timer = 0.0f;

    LoadSound(dog.snd_footstep, "footstep.wav");
    LoadSound(dog.snd_land,     "land.wav"    );
    LoadSound(dog.snd_hithead,  "hithead.wav" );
    LoadSound(dog.snd_jump,     "jump.wav"    );
    LoadSound(dog.snd_bark,     "bark.wav"    );
    LoadSound(dog.snd_explode0, "explode0.wav");
    LoadSound(dog.snd_explode1, "explode1.wav");

    LoadAnimation(dog.anim[DOG_STATE_IDLE], "dog-idle.anim");
    LoadAnimation(dog.anim[DOG_STATE_BLNK], "dog-blnk.anim");
    LoadAnimation(dog.anim[DOG_STATE_MOVE], "dog-move.anim");
    LoadAnimation(dog.anim[DOG_STATE_JUMP], "dog-jump.anim");
    LoadAnimation(dog.anim[DOG_STATE_FALL], "dog-fall.anim");
    LoadAnimation(dog.anim[DOG_STATE_BARK], "dog-bark.anim");
    LoadAnimation(dog.anim[DOG_STATE_LKDN], "dog-lkdn.anim");
    LoadAnimation(dog.anim[DOG_STATE_LKUP], "dog-lkup.anim");
    LoadAnimation(dog.anim[DOG_STATE_HPPY], "dog-hppy.anim");

    dog.up           = false;
    dog.right        = false;
    dog.down         = false;
    dog.left         = false;
    dog.jump_press   = false;
    dog.jump_release = false;
    dog.action       = false;
    dog.grounded     = true;
    dog.ledge_buffer = 0.0f;
    dog.jump_height  = 0.0f;
    dog.dead_timer   = 0.0f;
    dog.dead         = false;
    dog.deaths       = 0;

    // This gets updated whenever the dog transitions from room-to-room and acts as the respawn point.
    dog.start_state    = dog.state;
    dog.start_pos      = dog.pos;
    dog.start_vel      = dog.vel;
    dog.start_flip     = dog.flip;
    dog.start_grounded = dog.grounded;
}

INTERNAL void DeleteDog (Dog& dog)
{
    FreeAnimation(dog.anim[DOG_STATE_IDLE]);
    FreeAnimation(dog.anim[DOG_STATE_BLNK]);
    FreeAnimation(dog.anim[DOG_STATE_MOVE]);
    FreeAnimation(dog.anim[DOG_STATE_JUMP]);
    FreeAnimation(dog.anim[DOG_STATE_FALL]);
    FreeAnimation(dog.anim[DOG_STATE_BARK]);
    FreeAnimation(dog.anim[DOG_STATE_LKDN]);
    FreeAnimation(dog.anim[DOG_STATE_LKUP]);
    FreeAnimation(dog.anim[DOG_STATE_HPPY]);

    FreeImage(dog.image);

    FreeSound(dog.snd_footstep);
    FreeSound(dog.snd_land);
    FreeSound(dog.snd_hithead);
    FreeSound(dog.snd_jump);
    FreeSound(dog.snd_bark);
    FreeSound(dog.snd_explode0);
    FreeSound(dog.snd_explode1);
}

INTERNAL void UpdateDog (Dog& dog, float dt)
{
    if (IsFading()) return; // Prevents crashes from transitioning too far in the map!

    bool old_grounded = dog.grounded;
    Vec2 old_vel = dog.vel;

    if (IsKeyPressed(SDL_SCANCODE_R)) StartFade(FADE_SPECIAL, [](){ RetryGame(); });

    // Handle controls!!!
    if (!gGameState.doing_win_sequence)
    {
        dog.up           = (IsKeyDown(SDL_SCANCODE_UP   ) || IsButtonDown    (SDL_CONTROLLER_BUTTON_DPAD_UP   ) || IsLeftStickUp   ());
        dog.right        = (IsKeyDown(SDL_SCANCODE_RIGHT) || IsButtonDown    (SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || IsLeftStickRight());
        dog.down         = (IsKeyDown(SDL_SCANCODE_DOWN ) || IsButtonDown    (SDL_CONTROLLER_BUTTON_DPAD_DOWN ) || IsLeftStickDown ());
        dog.left         = (IsKeyDown(SDL_SCANCODE_LEFT ) || IsButtonDown    (SDL_CONTROLLER_BUTTON_DPAD_LEFT ) || IsLeftStickLeft ());
        dog.jump_press   = (IsKeyPressed(SDL_SCANCODE_Z ) || IsButtonPressed (SDL_CONTROLLER_BUTTON_A         )                      );
        dog.jump_release = (IsKeyReleased(SDL_SCANCODE_Z) || IsButtonReleased(SDL_CONTROLLER_BUTTON_A         )                      );
        dog.action       = (IsKeyPressed(SDL_SCANCODE_X ) || IsButtonPressed (SDL_CONTROLLER_BUTTON_X         )                      );
    }
    else
    {
        dog.up           = false;
        dog.right        = false;
        dog.down         = false;
        dog.left         = false;
        dog.jump_press   = false;
        dog.jump_release = true;
        dog.action       = false;

        if (IsKeyPressed(SDL_SCANCODE_Z) || IsButtonPressed(SDL_CONTROLLER_BUTTON_A)) EndWinSequence();
        if (IsKeyPressed(SDL_SCANCODE_X) || IsButtonPressed(SDL_CONTROLLER_BUTTON_X)) EndWinSequence();
    }

    // If the dog is dead respawn when a button is pressed or after some time.
    if (dog.dead)
    {
        dog.dead_timer -= dt;
        if (dog.jump_press || dog.action || dog.dead_timer <= 0.0f)
        {
            StartFade(FADE_SPECIAL, [](){ RespawnDog(gGameState.dog); });
        }
        return;
    }

    // Only if one direction is being pressed will we move the dog.
    if (dog.right != dog.left)
    {
        if (dog.right) { dog.vel.x =  DOG_MOVE_SPEED; dog.flip = FLIP_NONE; } // Move right.
        if (dog.left ) { dog.vel.x = -DOG_MOVE_SPEED; dog.flip = FLIP_HORZ; } // Move left.
    }
    if (!dog.left && !dog.right)
    {
        dog.vel.x = 0.0f;
    }

    // Apply a jump force if the key is presed.
    if (dog.grounded || dog.ledge_buffer > 0)
    {
        if (dog.jump_press)
        {
            dog.vel.y = (-DOG_JUMP_FORCE/2);
            dog.grounded = false;
            dog.ledge_buffer = 0;
            dog.jump_height = 0.18f;
            PlaySound(dog.snd_jump);
        }
    }

    if (dog.jump_release)
    {
        dog.jump_height = 0;
    }

    // Apply a gravity force to the dog.
    if (dog.jump_height <= 0)
    {
        dog.vel.y += DOG_WEIGHT * GRAVITY;
    }

    if (!dog.grounded)
    {
        dog.ledge_buffer -= dt;
        dog.jump_height -= dt;
    }
    else
    {
        dog.ledge_buffer = 0.08f;
    }

    // Handle collision detection and correction!
    Vec2 contact_normal = { 0,0 };
    dog.grounded = false;
    if (EntityAndMapCollision(dog.pos,dog.bounds,dog.vel, gWorld.current_map, contact_normal, dt))
    {
        if (contact_normal.y < 0) dog.grounded = true; // Hit the ground.
        if (contact_normal.y > 0) // Hit the ceiling.
        {
            CreateParticles(PARTICLE_TYPE_BASH, (int)dog.pos.x+12,(int)dog.pos.y+12,(int)dog.pos.x+12,(int)dog.pos.y+12, 4,8);
            dog.vel.y = 0;
            dog.jump_height = 0;
            PlaySound(dog.snd_hithead);
        }
    }

    // Apply velocity to the dog.
    dog.pos.x += (dog.vel.x * dt);
    dog.pos.y += (dog.vel.y * dt);

    // If the dog goes from still to moving on the ground create some dust puff particles.
    if (dog.grounded)
    {
        if (old_vel.x != dog.vel.x)
        {
            if (dog.vel.x < 0.0f) // Left!
            {
                CreateParticles(PARTICLE_TYPE_PUFF, (int)dog.pos.x+16,(int)dog.pos.y+18,(int)dog.pos.x+DOG_CLIP_W,(int)dog.pos.y+DOG_CLIP_H, 2,5);
                dog.footstep_timer = 0.2f;
            }
            else if (dog.vel.x > 0.0f) // Right!
            {
                CreateParticles(PARTICLE_TYPE_PUFF, (int)dog.pos.x,(int)dog.pos.y+18,(int)dog.pos.x+DOG_CLIP_W-16,(int)dog.pos.y+DOG_CLIP_H, 2,5);
                dog.footstep_timer = 0.2f;
            }
        }
    }

    // If the dog landed on the ground spawn some dust puff particles and play the landing sound.
    if ((old_grounded != dog.grounded) && (dog.grounded))
    {
        CreateParticles(PARTICLE_TYPE_PUFF, (int)dog.pos.x+4,(int)dog.pos.y+18,(int)dog.pos.x+DOG_CLIP_W-4,(int)dog.pos.y+DOG_CLIP_H, 2,5);
        PlaySound(dog.snd_land);
    }

    // Try and break blocks if the action button was pressed.
    bool block_broken = false;
    if (dog.action)
    {
        for (auto& bblock: gWorld.current_map.bblocks)
        {
            if (!bblock.dead)
            {
                if (TryBreakABlock(dog, bblock))
                {
                    block_broken = true;
                    ResetAnimation(dog.anim[DOG_STATE_BARK]);
                    dog.state = DOG_STATE_BARK;
                }
            }
        }
    }
    if (block_broken) // Do this out here so the sound only plays once and doesn't layer.
    {
        PlaySound(gBreakableBlockSound);
    }

    // Handle setting the dog's current animation state.
    if (gGameState.doing_win_sequence)
    {
        dog.state = DOG_STATE_HPPY;
    }
    if (!dog.grounded)
    {
        if (dog.vel.y <= 0.0f)
        {
            dog.state = DOG_STATE_JUMP;
        }
        else
        {
            dog.state = DOG_STATE_FALL;
        }
    }
    else
    {
        if (dog.state != DOG_STATE_HPPY)
        {
            if (dog.vel.x != 0.0f)
            {
                dog.state = DOG_STATE_MOVE;
            }
            else
            {
                if (dog.state != DOG_STATE_BARK && dog.action && !block_broken && !dog.up && !dog.right && !dog.left && !dog.down)
                {
                     ResetAnimation(dog.anim[DOG_STATE_BARK]);
                     dog.state = DOG_STATE_BARK;
                     PlaySound(dog.snd_bark);
                }

                if (dog.state == DOG_STATE_BARK || dog.state == DOG_STATE_BLNK)
                {
                    if (IsAnimationDone(dog.anim[dog.state]))
                    {
                        dog.state = DOG_STATE_IDLE;
                    }
                }
                else
                {
                    dog.state = DOG_STATE_IDLE;

                    if (dog.down)
                    {
                        dog.state = DOG_STATE_LKDN;
                    }
                    if (dog.up)
                    {
                        dog.state = DOG_STATE_LKUP;
                    }

                    if (dog.state == DOG_STATE_IDLE)
                    {
                        if (RandomRange(0, 1000) <= 10)
                        {
                            ResetAnimation(dog.anim[DOG_STATE_BLNK]);
                            dog.state = DOG_STATE_BLNK;
                        }
                    }
                }
            }
        }
    }

    // Play the footstep sound periodically whilst moving.
    if (dog.grounded && dog.state == DOG_STATE_MOVE)
    {
        dog.footstep_timer += dt;
        if (dog.footstep_timer >= 0.2f)
        {
            PlaySound(dog.snd_footstep);
            dog.footstep_timer -= 0.2f;
        }
    }

    // The camera tracks the dogs current position!
    float cx = roundf(dog.pos.x + (DOG_CLIP_W/2) - (WINDOW_SCREEN_W/2));
    float cy = roundf(dog.pos.y + (DOG_CLIP_H/2) - (WINDOW_SCREEN_H/2));

    SetCameraTarget(cx,cy);

    // Check dog collision with entities and perform the appropriate actions!

    // We shrink the collision bounds of the dog when dealing with spikes because otherwise colliding
    // horizontally with the spikes seems a bit unfair and feels wrong (smaller hitbox works better).
    Rect shrunken_dog_bounds = dog.bounds;
    shrunken_dog_bounds.x += 3;
    shrunken_dog_bounds.w -= 6;
    for (auto& spike: gWorld.current_map.spikes)
    {
        if (EntityAndEntityCollision(dog.pos,shrunken_dog_bounds, { spike.x,spike.y },spike.bounds))
        {
            KillDog(dog);
            break;
        }
    }
    for (auto& sbone: gWorld.current_map.sbones)
    {
        if (!sbone.dead)
        {
            if (EntityAndEntityCollision(dog.pos,dog.bounds, { sbone.x,sbone.y },sbone.bounds))
            {
                CreateParticles(PARTICLE_TYPE_SBONE, (int)sbone.x+8,(int)sbone.y+8,(int)sbone.x+8,(int)sbone.y+8, 1);
                gTempBoneCollectedIds.push_back(sbone.id);
                sbone.dead = true;
                PlaySound(gSmallBoneSound);
            }
        }
    }
    for (auto& lbone: gWorld.current_map.lbones)
    {
        if (!lbone.dead)
        {
            if (EntityAndEntityCollision(dog.pos,dog.bounds, { lbone.x,lbone.y },lbone.bounds))
            {
                CreateParticles(PARTICLE_TYPE_LBONE, (int)lbone.x+12,(int)lbone.y+12,(int)lbone.x+12,(int)lbone.y+12, 1);
                CreateParticles(PARTICLE_TYPE_SPEC, (int)lbone.x+12,(int)lbone.y+12,(int)lbone.x+12,(int)lbone.y+12, 40,72, 1.5f);
                gTempBoneCollectedIds.push_back(lbone.id);
                lbone.dead = true;
                PlaySound(gSmallBoneSound);
                PlaySound(gBigBoneSound);
                StartWinSequence();
            }
        }
    }
    for (auto& spitboy: gWorld.current_map.spitboys)
    {
        for (auto& spit: spitboy.spit)
        {
            if (!spit.dead)
            {
                if (EntityAndEntityCollision(dog.pos,dog.bounds, spit.pos,spit.bounds))
                {
                    KillSpit(spit);
                    KillDog(dog);
                    break;
                }
            }
        }
    }
    for (auto& cboi: gWorld.current_map.cboi)
    {
        if (EntityAndEntityCollision(dog.pos,dog.bounds, cboi.pos, cboi.bounds))
        {
            KillDog(dog);
            break;
        }
    }
    for (auto& walkboy: gWorld.current_map.walkboys)
    {
        if (EntityAndEntityCollision(dog.pos,dog.bounds, walkboy.pos,walkboy.bounds))
        {
            KillDog(dog);
            break;
        }
    }
    for (auto& chargeboy: gWorld.current_map.chargeboys)
    {
        if (EntityAndEntityCollision(dog.pos,dog.bounds, chargeboy.pos,chargeboy.bounds))
        {
            KillDog(dog);
            break;
        }
    }
}

INTERNAL void DrawDog (Dog& dog, float dt)
{
    if (dog.dead) return;

    UpdateAnimation(dog.anim[dog.state], dt);

    SDL_Rect clip = { 0,0,DOG_CLIP_W,DOG_CLIP_H };
    DrawImage(dog.image, dog.pos.x, dog.pos.y, dog.flip, GetAnimationClip(dog.anim[dog.state]));
}

INTERNAL void KillDog (Dog& dog)
{
    if (dog.dead) return;
    PlaySound(dog.snd_explode0);
    dog.dead_timer = DOG_DEAD_TIME;
    dog.dead = true;
    dog.deaths++;
    CreateParticles(PARTICLE_TYPE_EXPLODE1, (int)dog.pos.x-16,(int)dog.pos.y-16,(int)dog.pos.x+DOG_CLIP_W+16,(int)dog.pos.y+DOG_CLIP_H+16, 4,8);
    CreateParticles(PARTICLE_TYPE_SMOKE1, (int)dog.pos.x,(int)dog.pos.y,(int)dog.pos.x+DOG_CLIP_W,(int)dog.pos.y+DOG_CLIP_H, 4,8);
}

INTERNAL void RespawnDog (Dog& dog)
{
    dog.dead     = false;
    dog.state    = dog.start_state;
    dog.pos      = dog.start_pos;
    dog.vel      = dog.start_vel;
    dog.flip     = dog.start_flip;
    dog.grounded = dog.start_grounded;
    // Update camera.
    float cx = roundf(dog.pos.x + (DOG_CLIP_W/2) - (WINDOW_SCREEN_W/2));
    float cy = roundf(dog.pos.y + (DOG_CLIP_H/2) - (WINDOW_SCREEN_H/2));
    SetCamera(cx,cy);
    ResetCameraShake();
    // Reset the map.
    ResetMap();
}
