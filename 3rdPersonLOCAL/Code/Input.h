#pragma once

#include <unordered_map>

enum KeyState
{
    None,
    Pressed,
    Held,
    Released
};

class Input
{
public:
    static void PrepareKeyStates()
    {
        s_mouseMoved = false;

        for (auto& key : keyStateMap)
        {
            if (key.second == Pressed)
                key.second = Held;
            else if (key.second == Released)
                key.second = None;
        }

        for (auto& key : mouseStateMap)
        {
            if (key.second == Pressed)
                key.second = Held;
            else if (key.second == Released)
                key.second = None;
        }
    }

    static bool GetKey(int key)
    {
        if (keyStateMap.find(key) == keyStateMap.end())
            return false;

        return keyStateMap[key] == Pressed || keyStateMap[key] == Held;
    }

    static bool GetKeyDown(int key)
    {
        if (keyStateMap.find(key) == keyStateMap.end())
            return false;

        return keyStateMap[key] == Pressed;
    }

    static bool GetKeyUp(int key)
    {
        if (keyStateMap.find(key) == keyStateMap.end())
            return false;

        return keyStateMap[key] == Released;
    }

    static bool GetMouseButton(int key)
    {
        if (mouseStateMap.find(key) == mouseStateMap.end())
            return false;

        return mouseStateMap[key] == Pressed || mouseStateMap[key] == Held;
    }

    static bool GetMouseButtonDown(int key)
    {
        if (mouseStateMap.find(key) == mouseStateMap.end())
            return false;

        return mouseStateMap[key] == Pressed;
    }

    static bool GetMouseButtonUp(int key)
    {
        if (mouseStateMap.find(key) == mouseStateMap.end())
            return false;

        return mouseStateMap[key] == Released;
    }

    static void Press(int key)
    {
        keyStateMap[key] = Pressed;
    }

    static void Release(int key)
    {
        keyStateMap[key] = Released;
    }

    static void MousePress(int key)
    {
        mouseStateMap[key] = Pressed;
    }

    static void MouseRelease(int key)
    {
        mouseStateMap[key] = Released;
    }

    static bool IsMouseMoved()
    {
        return s_mouseMoved;
    }

    static void MouseMoved(double x, double y)
    {
        s_mouseMoved = true;
        s_mouseX = x;
        s_mouseY = y;
    }

    static std::pair<double, double> getMousePos()
    {
        return { s_mouseX, s_mouseY };
    }

    static std::pair<double, double> getMousePosChange()
    {
        double changeX = s_mouseX - s_lastMouseX;
        double changeY = s_mouseY - s_lastMouseY;
        s_lastMouseX = s_mouseX;
        s_lastMouseY = s_mouseY;
        return { changeX, changeY };
    }

private:
    inline static std::unordered_map<int, KeyState> keyStateMap;
    inline static std::unordered_map<int, KeyState> mouseStateMap;

    inline static bool s_mouseMoved = false;
    inline static double s_mouseX = 0.0, s_mouseY = 0.0;
    inline static double s_lastMouseX = 0.0, s_lastMouseY = 0.0;
};
