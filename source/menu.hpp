#ifndef MENU_HPP
#define MENU_HPP

enum MenuMode
{
    MENU_MODE_MAIN,
    MENU_MODE_CONTROLS,
};

GLOBAL struct MenuState
{
    MenuMode mode;
    int selected;

    Image title;
    Image help;
    Image caret;

    Animation caret_anim;

} gMenuState;

INTERNAL void InitMenu   ();
INTERNAL void QuitMenu   ();
INTERNAL void UpdateMenu (float dt);
INTERNAL void RenderMenu (float dt);

#endif /* MENU_HPP */